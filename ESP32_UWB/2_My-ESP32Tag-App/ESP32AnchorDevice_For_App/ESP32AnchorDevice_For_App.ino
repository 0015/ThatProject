/////////////////////////////////////////////////////////////////
/*
  My ESP32Tag App for my own ESP32Tag Device!
  For More Information: https://youtu.be/vaYJydZcObU
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <SPI.h>
#include "DW1000Ranging.h"
#include <ArduinoJson.h>

const char *app_id = "MyESP32TAGApp";
enum device_status {
  unknown,
  found,
  inactive,
  update
};

// connection pins
const uint8_t PIN_SCK = 18;
const uint8_t PIN_MOSI = 23;
const uint8_t PIN_MISO = 19;
const uint8_t PIN_SS = 15;
const uint8_t PIN_RST = 2;
const uint8_t PIN_IRQ = 22;

StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  //init the configuration
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);  //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachBlinkDevice(newBlink);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  DW1000.enableDebounceClock();
  DW1000.enableLedBlinking();
  DW1000.setGPIOMode(MSGP0, LED_MODE);
  
  DW1000Ranging.startAsAnchor("FF:EE:5B:D5:A9:9A:E2:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);  
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  float projectedRange = DW1000Ranging.getDistantDevice()->getRange() * 2 / 5;
  printJson(update, projectedRange);
}

void newBlink(DW1000Device* device) {
  printJson(found, 0);
}

void inactiveDevice(DW1000Device* device) {
  printJson(inactive, -1);
}

void printJson(int status, float meter){
  doc["app_id"] = app_id;
  doc["status_code"] = status;
  doc["meter"] = meter;
  String output;
  serializeJson(doc, output);
  Serial.println(output);
}
