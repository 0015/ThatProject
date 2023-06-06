/////////////////////////////////////////////////////////////////
/*
  Android Phone + ESP32-LoRa Module + Chat App = Text Transceiver!
  For More Information: https://youtu.be/4GVH3JicaPo
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <RadioLib.h>
#include "boards.h"

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

int transmissionState = RADIOLIB_ERR_NONE;
bool transmitFlag = false;
volatile bool operationDone = false;

TinyGPSPlus gps;

StaticJsonDocument<1024> doc;
StaticJsonDocument<512> msg;

String userId, userName;
String gpsJson = "";

long previousMillis = 0;
long GPSupdateInterval = 10000;  // 10sec

void setFlag(void) {
  // we sent or received a packet, set the flag
  operationDone = true;
}

void setup() {
  initBoard();
  delay(1500);

  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin(LoRa_frequency);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setOutputPower(22);
  Serial.print(F("LoRa_output Power Set"));
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  xTaskCreatePinnedToCore(Serial_Task, "Serial_Task", 4096, NULL, 10, NULL, 1);
}

void loop() {
  // check if the previous operation finished
  if (operationDone) {
    // reset flag
    operationDone = false;

    if (transmitFlag) {
      // listen for response
      radio.startReceive();
      transmitFlag = false;

    } else {
      // the previous operation was reception
      // print data and send another packet
      String str;
      int state = radio.readData(str);

      if (state == RADIOLIB_ERR_NONE) {
        // packet was successfully received
        if (!str.isEmpty()) {
          Serial.println(str);
          String chatId = parsingJson(str);
          if (chatId != "null" && !chatId.isEmpty()) {
            updateGPSInfo(chatId);
          }
        }
      }

      // wait a second before transmitting again
      delay(500);

      if (!gpsJson.isEmpty()) {
        transmissionState = radio.startTransmit(gpsJson);
        transmitFlag = true;
        gpsJson = "";
      }
    }
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > GPSupdateInterval) {
      previousMillis = currentMillis;
      smartDelay(0);
    }
  }
}

void Serial_Task(void *arg) {
  String incomingString = "";

  while (1) {

    while (Serial.available()) {
      incomingString = Serial.readStringUntil('\n');
    }

    if (incomingString.length() > 0 && !operationDone) {
      parsingSerial(incomingString);
      incomingString = "";
    }

    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

void parsingSerial(String strData) {
  String opcode = strData.substring(0, 1);
  String command = strData.substring(2);
  switch (opcode.toInt()) {
    case 0:
      // Set User Id and Name
      userId = command.substring(0, 8);
      userName = command.substring(9);
      Serial.println("userId : " + userId + " userName: " + userName);
      break;
    case 1:
      // Send Chat Data
      startTransmit(command);
      break;

    case 2:
      // Update MY GPS
      updateGPSInfo("0");
      // Request Other GPS
      requestGPS(command);
      break;

    default:
      break;
  }
}

void requestGPS(String chatId) {
  msg.clear();
  msg["id"] = chatId;
  String temp;
  serializeJson(msg, temp);
  startTransmit(temp);
}

void startTransmit(String temp) {
  transmissionState = radio.startTransmit(temp);
  transmitFlag = true;
}

String parsingJson(String json) {
  msg.clear();
  DeserializationError error = deserializeJson(msg, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return "";
  }

  String chatId = msg["id"];
  return chatId;
}

void updateGPSInfo(String chatId) {

  if (userId == NULL || userName == NULL) {
    return;
  }

  doc.clear();
  doc["appId"] = "LoRa_GPS";
  doc["chatId"] = chatId.c_str();
  doc["userId"] = userId.c_str();
  doc["sys"]["RSSI"] = String(radio.getRSSI(), 0);
  doc["sys"]["SNR"] = String(radio.getSNR(), 2);
  doc["sys"]["BAT"]["CNT"] = PMU.isBatteryConnect();
  doc["sys"]["BAT"]["VOL"] = String((PMU.getBattVoltage() / 1000), 2);
  doc["loc"]["lat"] = String(gps.location.lat(), 6);
  doc["loc"]["lng"] = String(gps.location.lng(), 6);
  doc["sat"] = gps.satellites.value();
  doc["alt"] = String((gps.altitude.feet() / 3.2808), 2);
  char gpsDateTime[20];
  sprintf(gpsDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
          gps.date.year(),
          gps.date.month(),
          gps.date.day(),
          gps.time.hour(),
          gps.time.minute(),
          gps.time.second());
  doc["time"] = gpsDateTime;

  if (chatId == "0") {
    String temp;
    serializeJson(doc, temp);
    Serial.println(temp);
  } else {
    serializeJson(doc, gpsJson);
    Serial.println(gpsJson);
  }
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
