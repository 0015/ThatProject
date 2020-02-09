#define WIFI_SSID "<YOUR_WIFI_SSID_HERE>"
#define PASSWORD "<YOUR_WIFI_PW_HERE>"

#include "SPI.h"
#include <TFT_eSPI.h>   
#include <TJpg_Decoder.h>
#include <CertStoreBearSSL.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include "List_SPIFFS.h"

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


#define BUTTONSIZE 50
#define PButtonFrame_X 20
#define PButtonFrame_Y 240
#define NButtonFrame_X 90
#define NButtonFrame_Y 240

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
BearSSL::CertStore certStore;
BearSSL::WiFiClientSecure *client;

const char * host = "maps.googleapis.com";
const String defaultPath = "/maps/api/staticmap?center=";
const String API_KEY = "<YOUR_API_KEY_HERE>";
const char * mapFile = "/map.jpg";

const long buttonInterval = 500; 
const int maxZoomLevel = 21;
const int minZoomLevel = 1;

static const int RXPin = 5, TXPin = 16;
static const uint32_t GPSBaud = 9600;

const int offset = -8; // Pacific Standard Time
time_t prevDisplay = 0;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

char timeBuffer[16];
char dateBuffer[16];
char zoomBuffer[10];

String prevCoarseLatitude, prevCoarseLongitude;
String latitude = "";
String longitude = "";
int zoomLevel = 17;

long previousMillis = 0;
bool buttonAvailable = false;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  
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
  uint16_t calData[5] = {277, 3614, 317, 3509, 1};
  tft.setTouch(calData);
  initScreen();

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  WiFi.begin(WIFI_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

 if(!hasCerts){
  return;
 }

  listSPIFFS();
  menuScreen();
}

void loop()
{
  buttonEvent();
  GPSEvent();
}

bool hasCerts(){
  setClock(); // Required for X.509 validation

  int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the SPIFFS directory before running?\n");
    return false; // Can't connect to anything w/o certs!
  }
  
  client = new BearSSL::WiFiClientSecure();
  return true;
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

String getPath(){
  String newPath = defaultPath;
  newPath += latitude;
  newPath += ",";
  newPath += longitude;
  newPath += "&zoom=";
  newPath += String(zoomLevel);
  newPath += "&size=320x320";
  newPath += "&maptype=roadmap";
  newPath += "markers=color:red%7C";
  newPath += latitude;
  newPath += ",";
  newPath += longitude;
  newPath += "&format=jpg-baseline";
  newPath += API_KEY;
}

void GPSEvent()
{
  while (ss.available() > 0){
    bool isMapNeedUpdate = false;
    
      if (gps.encode(ss.read())){
   
        int Year = gps.date.year();
        byte Month = gps.date.month();
        byte Day = gps.date.day();
        byte Hour = gps.time.hour();
        byte Minute = gps.time.minute();
        byte Second = gps.time.second();

        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);

        if(gps.location.isValid()){
          tft.setTextDatum(TL_DATUM);
          tft.setTextSize(1);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);

          latitude = String(gps.location.lat(), 6);
          tft.drawString(latitude, 0, 120, 4);
          String coarseLatitude = String(gps.location.lat(), 3);
          
          if(coarseLatitude != prevCoarseLatitude){
            isMapNeedUpdate = true;
          }
          prevCoarseLatitude = coarseLatitude;
          
          longitude = String(gps.location.lng(), 6);
          tft.drawString(longitude, 0, 150, 4);
          String coarseLongitude = String(gps.location.lng(), 3); 
          
          if(coarseLongitude != prevCoarseLongitude){            
            isMapNeedUpdate = true;
          }
          prevCoarseLongitude = coarseLongitude;
          
        }       
      }

      if(timeStatus() != timeNotSet){
        if(now() != prevDisplay){
          prevDisplay = now();
          dateTimeDisplay();

          if(isMapNeedUpdate){
            if(getStaticMapImage(host, getPath().c_str(), mapFile)){
              TJpgDec.drawFsJpg(160, 0, mapFile);
            }
          }
        }
      }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    errorScreen();
    while(true);
  }
}

void initScreen(){
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextColor(TFT_RED, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Waiting for GPS...", 240, 140, 4);
}

void errorScreen(){
  tft.fillScreen(TFT_RED);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("No GPS detected: Check Wiring!", 240, 140, 4);
}

void dateTimeDisplay(){
  sprintf(timeBuffer, "%02u:%02u:%02u", hour(), minute(), second());
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeBuffer, 0, 30, 4);
  
  sprintf(dateBuffer, "%02u/%02u/%04u", month(), day(), year());
  tft.drawString(dateBuffer, 0, 60, 4);
}

void menuScreen(){
  tft.fillScreen(TFT_BLACK);
  plusBtn();
  negativeBtn();
  showingZoomLevel();
  TJpgDec.drawFsJpg(160, 0, mapFile); //previous saved image
}

void plusBtn(){
  tft.fillRect(PButtonFrame_X, PButtonFrame_Y, BUTTONSIZE, BUTTONSIZE, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("+", PButtonFrame_X + (BUTTONSIZE / 2) + 1, PButtonFrame_Y + (BUTTONSIZE / 2));
}

void negativeBtn(){
  tft.fillRect(NButtonFrame_X, NButtonFrame_Y, BUTTONSIZE, BUTTONSIZE, TFT_RED);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("-", NButtonFrame_X + (BUTTONSIZE / 2) + 1, NButtonFrame_Y + (BUTTONSIZE / 2));
}

void showingZoomLevel(){
  sprintf(zoomBuffer, "Zoom : %02d", zoomLevel);
  
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.drawString(zoomBuffer, 20, 200);
}

void buttonEvent(){
  uint16_t x, y;
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > buttonInterval){
    previousMillis = currentMillis;
    buttonAvailable = false;
  }

  if(tft.getTouch(&x, &y) && !buttonAvailable){
    if((x > PButtonFrame_X) && (x < (PButtonFrame_X + BUTTONSIZE))){
      if((y > PButtonFrame_Y) && (x < (PButtonFrame_Y + BUTTONSIZE))){
        buttonAvailable = true;

        zoomLevel++;
        if(zoomLevel > maxZoomLevel){
          zoomLevel = maxZoomLevel;
        }
      }
    }

    if((x > NButtonFrame_X) && (x < (NButtonFrame_X + BUTTONSIZE))){
      if((y > NButtonFrame_Y) && (x < (NButtonFrame_Y + BUTTONSIZE))){
        buttonAvailable = true;

        zoomLevel--;
        if(zoomLevel < minZoomLevel){
          zoomLevel = minZoomLevel;
        }
      }
    }

    showingZoomLevel();
  }
}
