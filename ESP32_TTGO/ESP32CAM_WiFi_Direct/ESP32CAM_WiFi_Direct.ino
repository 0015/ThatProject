#include <SPI.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>
#include "Button2.h";

#define RIGHT_BUTTON_PIN  35
#define LEFT_BUTTON_PIN  0
#define YPOS_MAX 0
#define YPOS_MIN -105
#define MOVE_FORCE 20

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();
Button2 rButton = Button2(RIGHT_BUTTON_PIN);
Button2 lButton = Button2(LEFT_BUTTON_PIN);

int yPos = -52;

void setup() {
  Serial.begin(115200);

  buttonInit();
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextFont(4);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  
  tft.println("> Setting AP...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  tft.println("> AP IP Address");
  tft.print(">" ); 
  tft.println(IP);
  tft.println("> Waiting for");
  tft.println("> ESP32CAM");

  server.listen(8888);
}

void loop() {
  rButton.loop();
  lButton.loop();
  
  if(server.poll()){
    client = server.accept();
    }

  if(client.available()){
    client.poll();
    
    WebsocketsMessage msg = client.readBlocking();
    uint32_t t = millis();
    
    TJpgDec.drawJpg(-30, yPos, (const uint8_t*)msg.c_str(), msg.length());
    
    t = millis() - t;
    Serial.print(t); Serial.println(" ms");
  }
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void buttonInit(){
   rButton.setReleasedHandler(released);
   lButton.setReleasedHandler(released);
}

void released(Button2& btn) {
  if(btn == rButton){
    Serial.println("right click> ");
    moveYOffset(1);
  }else{
    Serial.println("left click> "); 
    moveYOffset(-1);
  }
}

void moveYOffset(int dir){
   yPos += (dir * MOVE_FORCE);

  if(yPos >= YPOS_MAX){
    yPos = YPOS_MAX;
  }else if(yPos <= YPOS_MIN){    
    yPos = YPOS_MIN; 
  }
  
  Serial.printf("yPos : %d\n", yPos);
}
