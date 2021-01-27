#include <SPI.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define FRAMESIZE_QQVGA_WIDTH  160
#define FRAMESIZE_QQVGA_HEGIHT 120

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextFont(4);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  WiFi.softAP(ssid, password);

  tft.println("> Version.2 ");
  tft.println(">         w/ JPEG");
  tft.println("> Ready To Go!");
  tft.println("> Waiting For");
  tft.println(">         ESP32CAM");

  server.listen(8888);
}

void loop() {
  if(server.poll()){
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
    
    t = millis();
    
    TJpgDec.drawJpg(0, 7, (const uint8_t*)msg.c_str(), msg.length());
   
    drawingTimeText(millis() - t, 105);
  }
}

void drawingTimeText(uint32_t deltaT, int yPos){ 
   tft.setTextColor(TFT_GREEN,TFT_BLACK); 
   tft.fillRect(163, yPos, 77, 20,TFT_BLACK);
   tft.setCursor(163, yPos, 4);   
   tft.print(deltaT);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}
