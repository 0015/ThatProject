// Example from TJpg_Decoder library:
// https://github.com/Bodmer/TJpg_Decoder
// https://github.com/Bodmer/TJpg_Decoder/blob/master/examples/ESP32_Dual_Core_Flash_Jpg/ESP32_Dual_Core_Flash_Jpg.ino

#include <SPI.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();

TaskHandle_t Task1;                 // Global variables available to BOTH processors 0 and 1
const uint8_t* arrayName;           // Name of FLASH array containing Jpeg
uint16_t arrayLength;               // Length of the Jpeg Image
bool doDecoding = false;            // Mutex flag to start decoding
bool mcuReady = false;              // Mutex flag to indicate an MCU block is ready for rendering
uint16_t mcuBuffer[16*16];          // Buffer to grab a snapshot of decoded MCU block
int32_t mcu_x, mcu_y, mcu_w, mcu_h; // Snapshot of the place to render the MCU

void setup() {
  Serial.begin(115200);
  
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextFont(4);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(mcu_decoded);
  
  WiFi.softAP(ssid, password);

  tft.println("> Version.2 ");
  tft.println(">         w/ JPEG v2");
  tft.println("> Ready To Go!");
  tft.println("> Waiting For");
  tft.println(">         ESP32CAM");

  server.listen(8888);  
}

void loop() {

   if(server.poll()){
      xTaskCreate(decodeJpg, "decodeJpg", 10000, NULL, 0, &Task1);
      client = server.accept();
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE,TFT_BLACK); 
      tft.setCursor(163, 10, 2);   
      tft.print("File Size");
      tft.setCursor(163, 50, 2);   
      tft.print("Network ms");
      tft.setCursor(163, 90, 2);   
      tft.print("Drawing ms");
    }
    
    if(client.available()){
      client.poll();
      
      uint32_t t = millis();
      WebsocketsMessage msg = client.readBlocking();
      drawingTimeText(msg.length(), 25);
      drawingTimeText(millis() - t, 65);        
      
      arrayName = (const uint8_t*)msg.c_str();
      arrayLength = msg.length();
      
      mcuReady   = false; // Flag which is set true when a MCU block is ready for display
      doDecoding = true;  // Flag to tell task to decode the image
      
      t = millis();
      
      while(doDecoding || mcuReady) {
        if (mcuReady) {
          tft.pushImage(mcu_x, mcu_y, mcu_w, mcu_h, mcuBuffer);
          mcuReady = false;
        }
        yield();
      }
      
     drawingTimeText(millis() - t, 105);
  }
}

// This next function will be called by the TJpg_Decoder library during decoding of the jpeg file
// A copy of the decoded MCU block is grabbed for rendering so decoding can then continue while
// the MCU block is rendered on the TFT. Note: This function is called by processor 0
bool mcu_decoded(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  while(mcuReady) yield(); // Wait here if rendering of last MCU block to TFT is still in progress

  memcpy(mcuBuffer, bitmap, 16*16*2); // Grab a copy of the MCU block image
  mcu_x = x;                          // Grab postion and size of MCU block
  mcu_y = y;
  mcu_w = w;
  mcu_h = h;
  mcuReady = true; // Flag to tell processor 1 that rendering of MCU can start

  // Return 1 to decode next Jpeg MCU block
  return 1;
}

// This is the task that runs on processor 0 (Arduino sketch runs on processor 1)
// It decodes the Jpeg image
void decodeJpg(void* pvParameter) {
  // This is an infinite loop, effectively the same as the normal sketch loop()
  // but this function and loop is running on processor 0
  for(;;) {
    // Decode the Jpeg image
    if (doDecoding) { // Only start decoding if main sketch sets this flag
      TJpgDec.drawJpg(0, 7, arrayName, arrayLength); // Runs until complete image decoded
      doDecoding = false; // Set mutex false to indicate decoding has ended
    }
    yield(); // Must yield in this loop
  }
}

void drawingTimeText(uint32_t deltaT, int yPos){ 
   tft.setTextColor(TFT_GREEN,TFT_BLACK); 
   tft.fillRect(163, yPos, 77, 20,TFT_BLACK);
   tft.setCursor(163, yPos, 4);   
   tft.print(deltaT);
}
