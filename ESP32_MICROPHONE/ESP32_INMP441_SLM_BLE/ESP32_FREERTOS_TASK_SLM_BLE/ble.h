#define BLE_TASK_PRI 4
#define BLE_TASK_STACK 10000
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
uint8_t clientNum;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      clientNum++;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      clientNum--;
    }
};

void setupBLE() {
  clientNum = 0;
  // Create the BLE Device
  BLEDevice::init("ESP32-THAT-PROJECT");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
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

void ble_Notifier(void *pvParameter) {

  double _Leq_dB;
  while(xQueueReceive(samples_queue, &_Leq_dB, portMAX_DELAY)){
      // notify changed value
    if (clientNum > 0) {
        String tempString = String(_Leq_dB);
        Serial.println(tempString);
        pCharacteristic->setValue((char*)tempString.c_str());
        pCharacteristic->notify();
        vTaskDelay(100);
    }
    // disconnecting
    if (clientNum <= 0) {
        vTaskDelay(500);
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
    }
  }
}
