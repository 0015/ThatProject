#include <SPI.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

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
  tft.setSwapBytes(false);
  tft.setTextFont(4);
  
  WiFi.softAP(ssid, password);

  tft.println("> Version.2 ");
  tft.println(">         w/ RGB565");
  tft.println("> Ready To Go!");
  tft.println("> Waiting For");
  tft.println(">         ESP32CAM");

  server.listen(8888);
}

void loop() {
  if(server.poll()){
    client = server.accept();
    
    tft.fillScreen(TFT_BLACK);
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
    tft.pushImage(0, 7, FRAMESIZE_QQVGA_WIDTH, FRAMESIZE_QQVGA_HEGIHT, (uint16_t*) ((const uint8_t*)msg.c_str()));
   
    drawingTimeText(millis() - t, 105);
  }
}

void drawingTimeText(uint32_t deltaT, int yPos){ 
   tft.setTextColor(TFT_GREEN,TFT_BLACK); 
   tft.fillRect(163, yPos, 77, 20,TFT_BLACK);
   tft.setCursor(163, yPos, 4);   
   tft.print(deltaT);
}
