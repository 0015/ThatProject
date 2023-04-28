/////////////////////////////////////////////////////////////////
/*
  ESP32TTGO - Battery Indicator 🔋⚡
  For More Information: https://youtu.be/osAOdmZuvDc
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// Updated: 2023-04-27
// Arduino core for the ESP32: 2.0.8
// https://github.com/espressif/arduino-esp32

// TFT_eSPI: 2.5.0
// https://github.com/Bodmer/TFT_eSPI

// TJpg_Decoder: 1.0.8
// https://github.com/Bodmer/TJpg_Decoder

// arduino-esp32fs-plugin: 1.1
// https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/

// TFT_eSPI Display Setting
// Open user_setup_select.h in Text Editor
// Comment-> #include <User_Setup.h>  
// Uncomment-> #include <User_Setups/Setup25_TTGO_T_Display.h>

// SPIFFS Uploder Tool 
// Downlaod https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/ 
// Copy esp32fs.jar file to Arduino/tools/ESP32FS/tool/esp32fs.jar
// Since the tool cannot be used in Arduino IDE 2.x, you must use Arduion IDE 1.x.

#include <Pangodream_18650_CL.h>
#include "SPIFFS.h" 
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
TFT_eSPI tft = TFT_eSPI();

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
                    
void setup() {
  Serial.begin(115200);
  pinoutInit();
  SPIFFSInit();
  displayInit();
  xTaskCreate(battery_info, "battery_info", 2048, NULL, 1, NULL);
}

void loop() { 
}

void pinoutInit(){
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
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
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextFont(2);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
}

void battery_info(void *arg)
{
  while (1) {
    tft.setCursor (0, STATUS_HEIGHT_BAR);
    tft.println("");
    tft.print("Average value from pin: ");
    tft.println(BL.pinRead());
    tft.print("Volts: ");
    tft.println(BL.getBatteryVolts());
    tft.print("Charge level: ");
    tft.println(BL.getBatteryChargeLevel());
    
    if(BL.getBatteryVolts() >= MIN_USB_VOL){
      for(int i=0; i< ARRAY_SIZE(batteryImages); i++){
        drawingBatteryIcon(batteryImages[i]);
        drawingText("Chrg");
        vTaskDelay(500);
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
        vTaskDelay(1000);
    }
      tft.print("Never Used Stack Size: ");
      tft.println(uxTaskGetStackHighWaterMark(NULL));
    }  
}

void drawingBatteryIcon(String filePath){
   TJpgDec.drawFsJpg(ICON_POS_X, 0, filePath);
}

void drawingText(String text){
  tft.fillRect(0, 0, ICON_POS_X, ICON_HEIGHT,TFT_BLACK);
  tft.setTextDatum(5);
  tft.drawString(text, ICON_POS_X-2, STATUS_HEIGHT_BAR/2, 4);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}
