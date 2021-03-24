/////////////////////////////////////////////////////////////////
/*
  ESP32 | Walkie-Talkie, Half-duplex communication based on WebSocket
  Video Tutorial: https://youtu.be/kw30vLdrGE8
  Created by Eric Nam (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h"
#include "driver/i2s.h"
#include "Button2.h";
#include "I2s_Setting.h"
#include "Const.h"
#define RIGHT_BUTTON_PIN  35
Button2 rButton = Button2(RIGHT_BUTTON_PIN);

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite logScreen = TFT_eSprite(&tft);

unsigned long requestTimestamp;
TaskHandle_t i2sReadTaskHandler = NULL;

void setup() {
  Serial.begin(115200);
  
  WiFi.softAP(ssid, password);
  server.listen(8888);
  
  display_init();
  display_ready_screen();
  button_init();
  i2s_buff_init();
  print_log_screen("+ All Ready");

  xTaskCreate(pong_task, "pong_task", 2048, NULL, 1, NULL);
}

void loop() {
  rButton.loop();
  listenForClient();
  pollClient();
}

void listenForClient() {
  if (server.poll()) {
    print_log_screen("+ Client Connected!");
    client = server.accept();
    client.onMessage(handleMessage);
    client.onEvent(handleEvent);
    client.send(getStrTimestamp());
    requestTimestamp = millis();
  }
}

void pollClient() {
  client.poll();
}

void handleMessage(WebsocketsClient &client, WebsocketsMessage message) {
  requestTimestamp = millis();
  int msgLength = message.length();
  
  if(message.type() == MessageType::Binary){
    if(states == Listening && msgLength > 0){
      i2s_write_data((char*)message.c_str(), msgLength);
    }

  }else if(message.type() == MessageType::Text){
    if(states == Idle){
      states = Listening;
      i2s_TX_init();
      client.send(message.data());
      print_log_screen("* Listening Mode *");
    }else if(states == Listening){
      states = Idle;
      i2s_TX_uninst();
      print_log_screen("* IDLE Mode *");
    }
  }
}

void handleEvent(WebsocketsClient &client, WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionClosed) {
    print_log_screen("- Client Disconnected!");
    if(states == Listening){
        states = Idle;
        i2s_TX_uninst();
      }
  }
}

static void i2s_read_task(void *arg)
{
  while (1) {
      i2s_read_data();
      if(client.available() && states == Speaking){
        requestTimestamp = millis();
        client.sendBinary((const char*)flash_write_buff, I2S_READ_LEN);
      }
  }
}

static void pong_task(void *arg){
  while (1) {
      if(client.available() && millis() - requestTimestamp > 5000)  {
        client.close();
      }
      vTaskDelay(1000);
  }
}

void pressed(Button2& btn) {
   if(btn == rButton && states == Idle){
    print_log_screen("* Speaking Mode *");
    
    client.send(getStrTimestamp());
    states = Speaking;
    i2s_RX_init();
    
    xTaskCreate(i2s_read_task, "i2s_read_task", 4096, NULL, 1, &i2sReadTaskHandler);
  }
}

void released(Button2& btn) {
  if(btn == rButton && states == Speaking){
    states = Idle;
    print_log_screen("* IDLE Mode *");
    
    delay(100);
    if( i2sReadTaskHandler != NULL ){
      vTaskDelete( i2sReadTaskHandler );
      i2sReadTaskHandler = NULL;
    }
    
    delay(100);      
    client.send(getStrTimestamp());
    i2s_RX_uninst();
  }
}

String getStrTimestamp(){
  return (String)millis();
}

void button_init(){
   rButton.setPressedHandler(pressed);
   rButton.setReleasedHandler(released);
}

void display_init(){
  tft.begin();
  tft.setRotation(0);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  
  logScreen.setColorDepth(8);
  logScreen.createSprite(tft.width(), tft.height()/2);
  logScreen.fillSprite(TFT_BLUE); 
  logScreen.setScrollRect(0, 0, tft.width(), tft.height()/2, TFT_BLUE);
  logScreen.setTextColor(TFT_WHITE);
  logScreen.setTextDatum(TL_DATUM);
}

void display_ready_screen(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft.drawString("ESP32-ESP32", 0, 10, GFXFF);
 
  tft.setFreeFont(FF23);
  tft.setTextColor(TFT_SILVER, TFT_BLACK);
  tft.drawString("Walkie", 0, 35, GFXFF);
  tft.setTextColor(TFT_GOLD, TFT_BLACK);
  tft.drawString("Talkie", 30, 60, GFXFF);

  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(deviceRole == Server ? "[Server Ready]" : "[Client Ready]", tft.width()/2, 100, GFXFF);
}

void print_log_screen(String text){
  logScreen.drawString(text, 0, -1, 2);
  logScreen.pushSprite(0, tft.height()/2);
  logScreen.scroll(0,16);
}