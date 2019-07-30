// BLE

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// SSD1351 Driver

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN   17
#define CS_PIN   5
#define RST_PIN  4
#define TRIG_PIN 0
#define ECHO_PIN 2

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);
long prev_distance;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  initDrawing();
  initBT();

}

void initDrawing(){
  tft.begin();
  tft.fillRect(0, 0, 128, 128, WHITE);
  tft.setRotation(2);

  updateBTInfo(false);
  

  tft.setCursor(54, 100);
  tft.setTextColor(MAGENTA);
  tft.setTextSize(2);
  tft.print("CM");
}

void updateBTInfo(bool isConnected){
  tft.fillRect(0, 0, 128, 30, WHITE);
  tft.setCursor(6, 0);
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.print(isConnected ? "BT::Connected!" :"BT::Not connected!");
}

void initBT(){
  // Create the BLE Device
  BLEDevice::init("ESP32 Ultrasonic Range");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = microsecondsTocentimeters(duration);
  Serial.print("Ultrasonic Distance : " );
  Serial.print(distance);
  Serial.println(" CM");
  updateDistance(distance);
  delay(30);

  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
      updateBTInfo(false);
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
      updateBTInfo(true);
  }
}

void updateDistance(long distance){
  if(prev_distance != distance){
    tft.fillRect(0, 30, 128, 70, WHITE);
    tft.setCursor(34, 40);

    if((int)distance < 10){
      tft.setTextColor(RED);
    }else if((int)distance > 10 && (int)distance <=99){
      tft.setTextColor(BLACK);
    }else if((int)distance > 100){
      tft.setTextColor(GREEN);
    }

    tft.setTextSize(4);
    String tempString = "";
    tempString += (int)distance;
    tft.print(tempString);
    prev_distance = distance;

    if (deviceConnected) {

      String str = "";
      str += (int)distance;
      
      pCharacteristic->setValue((char*)str.c_str());
      pCharacteristic->notify();
    }
  }
}

long microsecondsTocentimeters(long duration){
  return duration /29 / 2;
}
