#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

static const int RXPin = 5, TXPin = 16;
static const uint32_t GPSBaud = 9600;

const int offset = -8; // Pacific Standard Time
time_t prevDisplay = 0;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

bool isReadyToShow;
char timeBuffer[16];
char dateBuffer[16];
float prevLatitude, prevLongitude;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  tft.begin();
  tft.setRotation(3);

  isReadyToShow = false;
  initScreen();
}

void loop()
{

  while (ss.available() > 0){
      if (gps.encode(ss.read())){

        if(!isReadyToShow){
            isReadyToShow = true;
            tft.fillScreen(TFT_BLACK);
          }
          
        int Year = gps.date.year();
        byte Month = gps.date.month();
        byte Day = gps.date.day();
        byte Hour = gps.time.hour();
        byte Minute = gps.time.minute();
        byte Second = gps.time.second();

        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);

        if(gps.location.isValid()){
          tft.setTextDatum(TR_DATUM);
          tft.setTextSize(2);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);

          float latitude = gps.location.lat();
          if(latitude != prevLatitude){
            tft.drawString(String(latitude, 6), 460, 120, 4);
          }
          prevLatitude = latitude;

          float longitude = gps.location.lng();
          if(longitude != prevLongitude){
            tft.drawString(String(longitude, 6), 460, 120, 4);
          }
          prevLongitude = longitude;
          
        }       
      }

      if(timeStatus() != timeNotSet){
        if(now() != prevDisplay){
          prevDisplay = now();
          dateTimeDisplay();
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
  sprintf(timeBuffer, "Time: %02u:%02u:%02u", hour(), minute(), second());
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeBuffer, 0, 0, 4);

  sprintf(dateBuffer, "Date: %02u/%02u/%04u", month(), day(), year());
  tft.drawString(dateBuffer, 0, 26, 4);
}
