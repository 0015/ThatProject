/////////////////////////////////////////////////////////////////
/*
  [Viewer] LoRa, GPS Data showing on the Map 
  For More Information: https://youtu.be/zJvDw4UVDLc
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <TinyGPS++.h>                       
#include <ArduinoJson.h>
#include <RadioLib.h>
#include "boards.h"
#define 	ERR_NONE   0

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
TinyGPSPlus gps;          

int transmissionState = ERR_NONE;
volatile bool transmittedFlag = false;
volatile bool enableInterrupt = true;
unsigned long gpsLastMillis;
StaticJsonDocument<512> doc;

void setFlag(void)
{
    if (!enableInterrupt) {
        return;
    }
    transmittedFlag = true;
}

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    // initialize SX1262 with default settings
    Serial.print(F("[SX1262] Initializing ... "));
    int state = radio.begin(LoRa_frequency);
    Serial.print(F("LoRa_frequency:" ));
    Serial.println(LoRa_frequency);

    if (state == ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    // set the function that will be called
    // when packet transmission is finished
    radio.setDio1Action(setFlag);

    // start transmitting the first packet
    Serial.print(F("[SX1262] Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    transmissionState = radio.startTransmit("***Sending First Packet!***");
}


void loop()
{
    // check if the previous transmission finished
    if (transmittedFlag) {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;

        // reset flag
        transmittedFlag = false;

        if (transmissionState == ERR_NONE) {
            // packet was successfully sent
            Serial.println(F("transmission finished!"));
        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        delay(1000);
        fetchingGPS();
 
       
        enableInterrupt = true;
    }   
}

void fetchingGPS(){
  doc.clear();
  doc["appid"] = "LoRa_GPS";  
  doc["system"]["Name"] = "Transmitter";
  doc["system"]["RSSI"] = radio.getRSSI();
  doc["system"]["SNR"] = radio.getSNR();
  doc["system"]["Status"] = transmissionState;
  doc["system"]["Battery"]["Connection"] = PMU.isBatteryConnect();
  doc["system"]["Battery"]["Voltage"] = String((PMU.getBattVoltage() / 1000),2);
  doc["location"]["lat"] = gps.location.lat();
  doc["location"]["lng"] = gps.location.lng();
  doc["satellites"] = gps.satellites.value();
  doc["altitude"] =  String((gps.altitude.feet() / 3.2808),2);
  char gpsDateTime[20]; 
  sprintf(gpsDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
   gps.date.year(), 
   gps.date.month(),
   gps.date.day(),
   gps.time.hour(),
   gps.time.minute(),
   gps.time.second());
  doc["gpsDateTime"]= gpsDateTime;
  
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  transmissionState = radio.startTransmit(output);
  smartDelay(1000);        
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}


