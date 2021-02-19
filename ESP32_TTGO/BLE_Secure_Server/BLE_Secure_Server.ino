#include <Pangodream_18650_CL.h>
#include "SPIFFS.h" 
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "Free_Fonts.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define ICON_WIDTH 70
#define ICON_HEIGHT 36
#define STATUS_HEIGHT_BAR ICON_HEIGHT
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ICON_POS_X (tft.width() - ICON_WIDTH)
#define MIN_USB_VOL 4.9
#define ADC_PIN 34
#define CONV_FACTOR 1.8
#define READS 20

Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);
char *batteryImages[] = {"/battery_01.jpg", "/battery_02.jpg", "/battery_03.jpg", "/battery_04.jpg", "/battery_05.jpg"};

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define PASSKEY 999999

TFT_eSPI tft = TFT_eSPI();

SemaphoreHandle_t xGuiSemaphore;

void setup() {
  Serial.begin(115200);
  pinoutInit();
  SPIFFSInit();
  displayInit();
  bleInit();
  xGuiSemaphore = xSemaphoreCreateMutex();
  xTaskCreate(battery_info, "battery_info", 2048, NULL, 1, NULL);
  delay(500);
  xTaskCreate(ble_task, "ble_task", 2048, NULL, 2, NULL); 
}

void loop() {}

/////////////////////
//BLE Secure Server//
/////////////////////

class ServerCallback: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println(" - ServerCallback - onConnect");
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println(" - ServerCallback - onDisconnect");
    }
};

class SecurityCallback : public BLESecurityCallbacks {

  uint32_t onPassKeyRequest(){
    return 000000;
  }

  void onPassKeyNotify(uint32_t pass_key){}

  bool onConfirmPIN(uint32_t pass_key){
    vTaskDelay(5000);
    return true;
  }

  bool onSecurityRequest(){
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
    if(cmpl.success){
      Serial.println("   - SecurityCallback - Authentication Success");       
    }else{
      Serial.println("   - SecurityCallback - Authentication Failure*");
      pServer->removePeerDevice(pServer->getConnId(), true);
    }
    BLEDevice::startAdvertising();
  }
};

void bleSecurity(){
  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
  esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;          
  uint8_t key_size = 16;     
  uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint32_t passkey = PASSKEY;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

void bleInit(){
  BLEDevice::init("BLE-Secure-Server");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new SecurityCallback());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallback());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );

  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  bleSecurity();
}

static void ble_task(void *arg){
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(FF22);
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft.drawString("BLE", 0, 50, GFXFF);
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("Secure", 0, 75, GFXFF);
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft.drawString("Server", 0, 95, GFXFF);
  tft.drawString("Multi-Connect", 0, 115, GFXFF);
  tft.setTextColor(TFT_RED, TFT_GREEN);
  tft.drawString("PIN#: " + String(PASSKEY), 0, 135, GFXFF);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Clients#", 0, 160, GFXFF);
  
  uint32_t value = 0;

  while(1){
    
      xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
      int connectedCount = pServer->getConnectedCount();
       Serial.printf("connectedCount: %d\n", connectedCount);
  
      if (connectedCount >0) { 
          pCharacteristic->setValue((uint8_t*)&value, 4);
          pCharacteristic->notify();   
      }
      value++;

      tft.fillRect(0, 180, tft.width(), tft.height() - 180, TFT_NAVY);
      tft.setTextColor(TFT_GOLD, TFT_NAVY);
      tft.setTextDatum(TC_DATUM); // Centre text on x,y position
      tft.setFreeFont(FF24);
      tft.drawString(String(connectedCount), tft.width()/2, 190, GFXFF);
      TJpgDec.drawFsJpg(94, 40, value%2 == 0 ? "/bt_white.jpg" : "/bt_blue.jpg");
      
      xSemaphoreGive(xGuiSemaphore);
      vTaskDelay(pdMS_TO_TICKS(1000));     
  }
}

/////////////////////
//Battery Indicator//
/////////////////////

void pinoutInit(){
  pinMode(14, OUTPUT);
}
void SPIFFSInit(){
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
}

void displayInit(){
  tft.begin();
  tft.setRotation(0);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextFont(2);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
}

static void battery_info(void *arg){
  while(1){  
    digitalWrite(14, HIGH);
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    if(BL.getBatteryVolts() >= MIN_USB_VOL){        
      for(int i=0; i< ARRAY_SIZE(batteryImages); i++){
        drawingBatteryIcon(batteryImages[i]);
        drawingText("Chrg");
        vTaskDelay(200 / portTICK_PERIOD_MS);
      }
    }else{
      int imgNum = 0;
      int batteryLevel = BL.getBatteryChargeLevel();
      if(batteryLevel >=80){
        imgNum = 3;
      }else if(batteryLevel < 80 && batteryLevel >= 50 ){
        imgNum = 2;
      }else if(batteryLevel < 50 && batteryLevel >= 20 ){
        imgNum = 1;
      }else if(batteryLevel < 20 ){
        imgNum = 0;
      }  

      drawingBatteryIcon(batteryImages[imgNum]);    
      drawingText(String(batteryLevel) + "%");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    xSemaphoreGive(xGuiSemaphore);
    vTaskDelay(pdMS_TO_TICKS(1000)); 
    digitalWrite(14, LOW);
  }  
}

void drawingBatteryIcon(String filePath){
   TJpgDec.drawFsJpg(ICON_POS_X, 0, filePath);
}

void drawingText(String text){
  tft.fillRect(0, 0, ICON_POS_X, ICON_HEIGHT,TFT_BLACK);
  tft.setTextDatum(5);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(text, ICON_POS_X-2, STATUS_HEIGHT_BAR/2, 4);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}