/////////////////////////////////////////////////////////////////
/*
  Mini E-Paper | Ep1. Uploading Emoji to E-Paper from Flutter App(iOS & Android)
  Video Tutorial: https://youtu.be/pP9YXFNOBhY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <GxEPD2_BW.h>
#include "GxEPD2_display_selection_new_style.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "SPIFFS.h"
#define FileClass fs::File

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
#define SERVICE_UUID "713d0001-503e-4c75-ba94-3148f18d941e"
#define CHARACTERISTIC_UUID_RX "713d0002-503e-4c75-ba94-3148f18d941e"

const char temp_filename[] = "/new_image.bmp";
const char BLE_Device_Name[] = "MINI E-PAPER";
class BLEServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer){};

  void onDisconnect(BLEServer *pServer) {
    pServer->startAdvertising();
  }
};


class BLERxCallback : public BLECharacteristicCallbacks {

  uint16_t read16(fs::File &f) {
    // BMP data is stored little-endian, same as Arduino.
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();  // MSB
    return result;
  }

  uint32_t read32(fs::File &f) {
    // BMP data is stored little-endian, same as Arduino.
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read();  // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read();  // MSB
    return result;
  }

  static const uint16_t input_buffer_pixels = 800;  // may affect performance
  static const uint16_t max_row_width = 80;         // for up to 6" display 1448x1072
  static const uint16_t max_palette_pixels = 256;   // for depth <= 8

  uint8_t input_buffer[3 * input_buffer_pixels];         // up to depth 24
  uint8_t output_row_mono_buffer[max_row_width / 8];     // buffer for at least one row of b/w bits
  uint8_t output_row_color_buffer[max_row_width / 8];    // buffer for at least one row of color bits
  uint8_t mono_palette_buffer[max_palette_pixels / 8];   // palette buffer for depth <= 8 b/w
  uint8_t color_palette_buffer[max_palette_pixels / 8];  // palette buffer for depth <= 8 c/w
  uint16_t rgb_palette_buffer[max_palette_pixels];       // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display

  bool isEnd = true;
  File file;

  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    Serial.printf("RX length: %d\n", rxValue.length());
    if (rxValue.length() > 0) {
      file.write((const byte *)pCharacteristic->getData(), rxValue.length());
    } else {
      Serial.println("***GET START/END TRIGGER****");
      isEnd = !isEnd;

      if (!isEnd) {
        file = SPIFFS.open(temp_filename, FILE_WRITE);
        if (!file) {
          Serial.println("File is not available!");
        }else{
          Serial.println("File is ready to write!");  
        }
        
      } else {
        file.close();
        Serial.println("File Closed!");
        drawBitmapFromSpiffs_Buffered(temp_filename, 0, 0, false, false, false);
      }
    }
  }

  void drawBitmapFromSpiffs_Buffered(const char *filename, int16_t x, int16_t y, bool with_color, bool partial_update, bool overwrite) {
    fs::File file;
    bool valid = false;  // valid format to be handled
    bool flip = true;    // bitmap is stored bottom-to-top
    bool has_multicolors = display.epd2.panel == GxEPD2::ACeP565;
    uint32_t startTime = millis();
    if ((x >= display.width()) || (y >= display.height())) return;
    Serial.println();
    Serial.print("Loading image '");
    Serial.print(filename);
    Serial.println('\'');
    file = SPIFFS.open(filename, "r");
    if (!file) {
      Serial.print("File not found");
      return;
    }
    // Parse BMP header
    if (read16(file) == 0x4D42)  // BMP signature
    {
      uint32_t fileSize = read32(file);
      uint32_t creatorBytes = read32(file);
      uint32_t imageOffset = read32(file);  // Start of image data
      uint32_t headerSize = read32(file);
      uint32_t width = read32(file);
      uint32_t height = read32(file);
      uint16_t planes = read16(file);
      uint16_t depth = read16(file);  // bits per pixel
      uint32_t format = read32(file);
      if ((planes == 1) && ((format == 0) || (format == 3)))  // uncompressed is handled, 565 also
      {
        Serial.print("File size: ");
        Serial.println(fileSize);
        Serial.print("Image Offset: ");
        Serial.println(imageOffset);
        Serial.print("Header size: ");
        Serial.println(headerSize);
        Serial.print("Bit Depth: ");
        Serial.println(depth);
        Serial.print("Image size: ");
        Serial.print(width);
        Serial.print('x');
        Serial.println(height);
        // BMP rows are padded (if needed) to 4-byte boundary
        uint32_t rowSize = (width * depth / 8 + 3) & ~3;
        if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
        if (height < 0) {
          height = -height;
          flip = false;
        }
        uint16_t w = width;
        uint16_t h = height;
        if ((x + w - 1) >= display.width()) w = display.width() - x;
        if ((y + h - 1) >= display.height()) h = display.height() - y;
        //if (w <= max_row_width) // handle with direct drawing
        {
          valid = true;
          uint8_t bitmask = 0xFF;
          uint8_t bitshift = 8 - depth;
          uint16_t red, green, blue;
          bool whitish, colored;
          if (depth == 1) with_color = false;
          if (depth <= 8) {
            if (depth < 8) bitmask >>= depth;
            //file.seek(54); //palette is always @ 54
            file.seek(imageOffset - (4 << depth));  // 54 for regular, diff for colorsimportant
            for (uint16_t pn = 0; pn < (1 << depth); pn++) {
              blue = file.read();
              green = file.read();
              red = file.read();
              file.read();
              whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);  // whitish
              colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                   // reddish or yellowish?
              if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
              mono_palette_buffer[pn / 8] |= whitish << pn % 8;
              if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
              color_palette_buffer[pn / 8] |= colored << pn % 8;
              rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
            }
          }
          if (partial_update) display.setPartialWindow(x, y, w, h);
          else
            display.setFullWindow();
          display.firstPage();
          do {
            if (!overwrite) display.fillScreen(GxEPD_WHITE);
            uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
            for (uint16_t row = 0; row < h; row++, rowPosition += rowSize)  // for each line
            {
              uint32_t in_remain = rowSize;
              uint32_t in_idx = 0;
              uint32_t in_bytes = 0;
              uint8_t in_byte = 0;  // for depth <= 8
              uint8_t in_bits = 0;  // for depth <= 8
              uint16_t color = GxEPD_WHITE;
              file.seek(rowPosition);
              for (uint16_t col = 0; col < w; col++)  // for each pixel
              {
                // Time to read more pixel data?
                if (in_idx >= in_bytes)  // ok, exact match for 24bit also (size IS multiple of 3)
                {
                  in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                  in_remain -= in_bytes;
                  in_idx = 0;
                }
                switch (depth) {
                  case 24:
                    blue = input_buffer[in_idx++];
                    green = input_buffer[in_idx++];
                    red = input_buffer[in_idx++];
                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);  // whitish
                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                   // reddish or yellowish?
                    color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                    break;
                  case 16:
                    {
                      uint8_t lsb = input_buffer[in_idx++];
                      uint8_t msb = input_buffer[in_idx++];
                      if (format == 0)  // 555
                      {
                        blue = (lsb & 0x1F) << 3;
                        green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                        red = (msb & 0x7C) << 1;
                        color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                      } else  // 565
                      {
                        blue = (lsb & 0x1F) << 3;
                        green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                        red = (msb & 0xF8);
                        color = (msb << 8) | lsb;
                      }
                      whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);  // whitish
                      colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                   // reddish or yellowish?
                    }
                    break;
                  case 1:
                  case 4:
                  case 8:
                    {
                      if (0 == in_bits) {
                        in_byte = input_buffer[in_idx++];
                        in_bits = 8;
                      }
                      uint16_t pn = (in_byte >> bitshift) & bitmask;
                      whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                      colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                      in_byte <<= depth;
                      in_bits -= depth;
                      color = rgb_palette_buffer[pn];
                    }
                    break;
                }
                if (with_color && has_multicolors) {
                  // keep color
                } else if (whitish) {
                  color = GxEPD_WHITE;
                } else if (colored && with_color) {
                  color = GxEPD_COLORED;
                } else {
                  color = GxEPD_BLACK;
                }
                uint16_t yrow = y + (flip ? h - row - 1 : row);
                display.drawPixel(x + col, yrow, color);
              }  // end pixel
            }    // end line
          } while (display.nextPage());
          Serial.print("loaded in ");
          Serial.print(millis() - startTime);
          Serial.println(" ms");
        }
      }
    }
    file.close();
    if (!valid) {
      Serial.println("bitmap format not handled.");
    }
  }
};
 
void setup() {
  Serial.begin(115200);
  Serial.println();
  initDisplay();
  initSPIFFS();
  initBLE();
}

void loop(void) {}

void initDisplay() {
  SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
  display.init(115200);
}

void initSPIFFS() {

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }

  Serial.println("SPIFFS started");
  listSPIFFS();
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

void initBLE() {
  Serial.println("**** INIT BLE ****");
  // Create the BLE Device

  BLEDevice::init(BLE_Device_Name);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCB());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new BLERxCallback());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Start advertising");
}
