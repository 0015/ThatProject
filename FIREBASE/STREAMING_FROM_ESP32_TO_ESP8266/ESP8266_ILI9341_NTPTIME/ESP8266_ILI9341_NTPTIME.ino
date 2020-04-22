#include <TimeLib.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h> 
#include <FS.h>  
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"

#define FIREBASE_HOST "<YOUR_PROJECT_ID>.firebaseio.com"
#define FIREBASE_AUTH "<YOUR_DATABASE_KEY>"
#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASSWORD "<YOUR_WIFI_PASSWORD>"

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = -7;  // Pacific Daylight Time (USA)

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
time_t prevDisplay = 0; // when the digital clock was displayed

TFT_eSPI tft = TFT_eSPI();

char timeBuffer[16];
char dateBuffer[16];

FirebaseData firebaseData;

String heatEffectText[] = {
  "Caution: fatigue is possible with prolonged exposure and activity.",
  "Extreme caution: heat cramps and heat exhaustion are possible.",
  "Danger: heat cramps and heat exhaustion are likely.",
  "Extreme danger: heat stroke is imminent."
};

void setup()
{
  Serial.begin(115200);
  WIFInit();
  FirebaseInit();
  TFTInit();
}

void loop()
{
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
  }
}

void WIFInit(){
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

void FirebaseInit(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);

  if(!Firebase.beginStream(firebaseData, "/DHT")){
    Serial.println("Can't begin stream connection...");
    Serial.println("Reason: " + firebaseData.errorReason());
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
}

void streamCallback(StreamData data){
  if(data.dataType() == "json"){
    FirebaseJson &json = data.jsonObject();
    FirebaseJsonData jsonData;

    tft.loadFont(NotoSansBold36);
    
    json.get(jsonData, "/temp");
    if(jsonData.success){
      Serial.print("Temp : " );
      Serial.println(String(jsonData.doubleValue));
      tft.setTextColor(TFT_RED, TFT_WHITE);
      tft.drawString(String(jsonData.doubleValue) +"F", 34, 160, 4);
    }

    json.get(jsonData, "/hum");
    if(jsonData.success){
      Serial.print("Humidity : " );
      Serial.println(String(jsonData.doubleValue));
      tft.setTextColor(TFT_BLUE, TFT_WHITE);
      tft.drawString(String(jsonData.doubleValue) +"%", 174, 160, 4);
    }

    json.get(jsonData, "/ht");
    if(jsonData.success){
      Serial.print("Heat Index : " );
      Serial.println(String(jsonData.doubleValue));
      String heatIndex = "Heat Index: ";
      heatIndex += String(jsonData.doubleValue) + "F, ";
      heatIndex += getHeatEffectText(jsonData.doubleValue);
      tft.loadFont(NotoSansBold15);
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
      tft.fillRect(0, 190, 320, 240, TFT_WHITE);
      tft.drawString(heatIndex, 4, 190, 4);
    }
  }
}

String getHeatEffectText(double hi){
  if(hi > 80 && hi <= 90){
    return heatEffectText[0];
  }else if(hi > 90 && hi <= 105){
    return heatEffectText[1];
  }else if(hi > 105 && hi <= 130){
    return heatEffectText[2];
  }else if(hi > 130){
    return heatEffectText[3];
  }else{
    return "Normal";
  }
}

void streamTimeoutCallback(bool timeout){
  if(timeout){
    Serial.println("Stream timeout, resume streaming...");
  }
}

void TFTInit(){

  if(!SPIFFS.begin()){
    while(1) yield();
  }
  
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawFsJpg(0, 0, "/bg.jpg");
}

void digitalClockDisplay()
{
  
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  sprintf(dateBuffer, "%02u/%02u/%04u", month(), day(), year());
  tft.loadFont(NotoSansBold15);
  tft.drawString(dateBuffer, 4, 6, 4);


  tft.fillRect(0, 18, 160, 40, TFT_WHITE);
  sprintf(timeBuffer, "%02u:%02u:%02u", hour(), minute(), second());
  tft.loadFont(NotoSansBold36);
  tft.drawString(timeBuffer, 4, 20, 4);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}


/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
