#define WIFI_SSID "<YOUR_WIFI_SSID_HERE>"
#define PASSWORD "<YOUR_WIFI_PW_HERE>"
#include <TJpg_Decoder.h>
#include <CertStoreBearSSL.h>
#include <time.h>

// Include SPIFFS
#define FS_NO_GLOBALS
#include <FS.h>

// Include WiFi and http client
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#else
  #include "SPIFFS.h" // Required for ESP32 only
  #include <WiFi.h>
  #include <HTTPClient.h>
#endif

// Load tabs attached to this sketch
#include "List_SPIFFS.h"
#include "Web_Fetch.h"
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

BearSSL::CertStore certStore;

const char * host = "maps.googleapis.com";
const char * path = "/maps/api/staticmap?center=40.714728,-73.998672&zoom=14&size=480x320&format=jpg-baseline&key=<YOUR_API_KEY_HERE>";
const char * mapFile = "/map.jpg";

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");


  // Initialise the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  WiFi.begin(WIFI_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  setClock(); // Required for X.509 validation

  int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the SPIFFS directory before running?\n");
    return; // Can't connect to anything w/o certs!
  }

  listSPIFFS();

  if(getStaticMapImage(host, path, mapFile)){
    listSPIFFS();
    TJpgDec.drawFsJpg(0, 0, mapFile);
  }

}

void loop()
{
  
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

bool getStaticMapImage(const char *host, const char *path, String fileName){
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }

  int contentLength = 0;
  int httpCode;

  BearSSL::WiFiClientSecure *client = new BearSSL::WiFiClientSecure();
  // Integrate the cert store with this connection
  client->setCertStore(&certStore);
  client->connect(host, 443); //HTTPS

  Serial.printf("Trying: %s:443...", host);
  
  if(!client->connected()){
    client->stop();
    Serial.printf("*** Can't connect. ***\n-------\n");
    return false;
  }

  Serial.println("HTTPS Connected!");
  client->write("GET ");
  client->write(path);
  client->write(" HTTP/1.0\r\nHost: ");
  client->write(host);
  client->write("\r\nUser-Agent: ESP8266\r\n");
  client->write("\r\n");

  while(client->connected()){
    String header = client-> readStringUntil('\n');
    if(header.startsWith(F("HTTP/1."))){
      httpCode = header.substring(9, 12).toInt();
      if(httpCode != 200){
        client->stop();
        return false;
      }
    }

    if(header.startsWith(F("Content-Length: "))){
      contentLength = header.substring(15).toInt();
    }
    
    if(header == F("\r")){
      break;
    }
    
  }

  if(!(contentLength > 0)){
    client->stop();
    return false;
  }

  fs::File f = SPIFFS.open(fileName, "w");
  if(!f){
    Serial.println(F("FILE OPEN FAILED"));
    client->stop();
    return false;
  }

  int remaining = contentLength;
  int received;
  uint8_t buff[512] = {0};

  while(client->available() && remaining > 0){
    received = client->readBytes(buff, ((remaining > sizeof(buff)) ? sizeof(buff) : remaining));
    f.write(buff, received);

    if(remaining > 0){
      remaining -= received;
    }
    yield();
  }

  f.close();
  client->stop();
  Serial.println("DOWNLOAD END");
  return (remaining == 0 ? true : false);
}
