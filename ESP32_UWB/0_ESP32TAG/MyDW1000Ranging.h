#include <SPI.h>
#include <DW1000.h>
#include <DW1000Ranging.h>

// connection pins
const uint8_t PIN_SCK = 18;
const uint8_t PIN_MOSI = 23;
const uint8_t PIN_MISO = 19;
const uint8_t PIN_SS = 15;
const uint8_t PIN_RST = 2;
const uint8_t PIN_IRQ = 22;

void newRange() {
  Serial.print("from: ");
  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t Range: ");
  Serial.print(DW1000Ranging.getDistantDevice()->getRange());
  Serial.print(" m");
  Serial.print("\t RX power: ");
  Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
  Serial.println(" dBm");
  functionPointer(DW1000Ranging.getDistantDevice()->getRange());
}

void newDevice(DW1000Device* device) {
  // Serial.print("ranging init; 1 device added ! -> ");
  // Serial.print(" short:");
  // Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  //Serial.print("delete inactive device: ");
  //Serial.println(device->getShortAddress(), HEX);
  functionPointer(-1);
}

void initDW1000() {
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);  //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);

  DW1000.enableDebounceClock();
  DW1000.enableLedBlinking();
  DW1000.setGPIOMode(MSGP0, LED_MODE);

  //we start the module as a tag
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}