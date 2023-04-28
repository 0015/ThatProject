/////////////////////////////////////////////////////////////////
/*
  [ESP32 TTGO] Getting Started! #StartingLine #BasicSetup (ft. Image Animation)
  For More Information: https://youtu.be/Bl1C7Vdl3GA
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

#include "SPIFFS.h"
#include <TJpg_Decoder.h>
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
int imgNum = 0;

void setup() {
  Serial.begin(115200);

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  listSPIFFS();

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
}

void loop() {
  if(imgNum >23) imgNum = 0;
  String imgPath = "/image_";
  imgPath += imgNum++;
  imgPath +=".jpg";

  TJpgDec.drawFsJpg(0, 0, imgPath);
}

void listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}
