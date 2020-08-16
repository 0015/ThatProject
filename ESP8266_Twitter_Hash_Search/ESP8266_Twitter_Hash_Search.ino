#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ArduinoJson.h>            
#include <TwitterWebAPI.h>
#include <TJpg_Decoder.h>
// Include SPIFFS
#define FS_NO_GLOBALS
#include <FS.h>
#include "List_SPIFFS.h"
#include "Web_Fetch.h"
#include "SPI.h"
#include "TFT_eSPI.h"
TFT_eSPI tft = TFT_eSPI();

#ifndef WIFICONFIG
const char* ssid = "<YOUR_WIFI_SSID>";    
const char* password = "<YOUR_WIFI_PW>";
#endif

std::string search_str = "#WeWantToPlay"; // Default search word for twitter
const char *ntp_server = "pool.ntp.org";  // time1.google.com, time.nist.gov, pool.ntp.org
int timezone = -7;                        // Pacific Daylight Time -07:00 HRS
unsigned long twi_update_interval = 10;   // (seconds) minimum 5s (180 API calls/15 min). Any value less than 5 is ignored!

#ifndef TWITTERINFO  // Obtain these by creating an app @ https://apps.twitter.com/
  static char const consumer_key[]    = "<API_Key>";
  static char const consumer_sec[]    = "<API_Key_Secret>";
  static char const accesstoken[]     = "<Access_Token>";
  static char const accesstoken_sec[] = "<Access_Token_Secret>";
#endif

const char* userProfileImage = "/profile_user.jpg";
const char* placeholderImage = "/placeholder.jpg";

unsigned long api_mtbs = twi_update_interval * 1000; //mean time between api requests
unsigned long api_lasttime = 0; 
String search_msg = "No Message Yet!";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, timezone*3600, 60000);  // NTP server pool, offset (in seconds), update interval (in milliseconds)
TwitterClient tcr(timeClient, consumer_key, consumer_sec, accesstoken, accesstoken_sec);

void initSPIFFS(){
  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  //SPIFFS.format(); //Only for the first time!
  listSPIFFS();
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void initTFT(){
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setCursor(0, 30);
  tft.println("Twitter Hashtag Search");

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);  
}

void printToTFT(String msg){
  tft.println(msg);
}

void showTweet(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.setCursor(120, 40);
  tft.println(search_str.c_str());

  tft.setFreeFont(&FreeSans9pt7b);
  tft.setCursor(0, 80);
  tft.println(search_msg.c_str());
}

void setup(void){
  Serial.begin(115200);
  
  initSPIFFS();
  initTFT();
  
  // WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected. yay!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  printToTFT("WIFI Connected!");
  
  delay(100);
  // Connect to NTP and force-update time
  tcr.startNTP();
  Serial.println("NTP Synced");

  printToTFT("NTP Synced!");
  
  delay(100);
  
  printToTFT("Loading...");
}

void extractJSON(String tmsg) {
  //Serial.println(tmsg);
  size_t len = tmsg.length();
  Serial.printf("msg length: %d\n", len);
  if(len < 1000) return;

  DynamicJsonDocument doc(len*2);
  DeserializationError error = deserializeJson(doc, tmsg, DeserializationOption::NestingLimit(20));
  if(error){
    Serial.print(F("DeserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  if(doc.containsKey("statuses")){
    String userT = doc["statuses"][0]["user"]["screen_name"];
    String text = doc["statuses"][0]["text"];
    if(text != ""){
      search_msg = "@" + userT + " says " + text;
    }

    showTweet();
    TJpgDec.drawFsJpg(10, 10, placeholderImage);

    String profile_image_url = doc["statuses"][0]["user"]["profile_image_url"];
    Serial.println(profile_image_url);
    
    if(profile_image_url.indexOf("_normal") > 0){

      bool loaded_ok = getFile(profile_image_url, userProfileImage);
      listSPIFFS();
      
      if(loaded_ok){
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, userProfileImage);
        if(w !=0 && h != 0){
          TJpgDec.drawFsJpg(10, 10, userProfileImage);          
        }
      }
    }
    
  }else if(doc.containsKey("errors")){
    String err = doc["errors"][0];
    search_msg = err;
  }else{
    Serial.println("No Useful Data");
  }
}

void loop(void){
  if (millis() > api_lasttime + api_mtbs)  {
    extractJSON(tcr.searchTwitter(search_str));
    api_lasttime = millis();
  }
  delay(2);
  yield();
}
