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
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite logScreen = TFT_eSprite(&tft);

unsigned long timeStampOffset = 0;
unsigned long requestTimestamp;
TaskHandle_t i2sReadTaskHandler = NULL;

void setup() {
  Serial.begin(115200);
  display_init();
  display_ready_screen();
  button_init();
  i2s_buff_init();
  start_to_connect();
  
  xTaskCreate(ping_task, "ping_task", 2048, NULL, 1, NULL);
}

void loop() {
  rButton.loop();
  if (client.available()) {
    client.poll();
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionClosed) {
        ESP.restart();
    }
}

void onMessageCallback(WebsocketsMessage message) {
    int msgLength = message.length();
    if(message.type() == MessageType::Binary){
      if(states == Listening && msgLength > 0){
        i2s_write_data((char*)message.c_str(), msgLength);
      }

    }else if(message.type() == MessageType::Text){
      unsigned long timeResponse = message.data().toInt();
      actionCommand(timeResponse);
    }
}

void actionCommand(unsigned long timestamp){
  if(timeStampOffset == 0){
    unsigned long currentTimestamp = millis();
    timeStampOffset = currentTimestamp >= timestamp? currentTimestamp - timestamp : timestamp - currentTimestamp;
    return;
  }

  if(requestTimestamp == timestamp && states == Idle){
    states = Speaking;
    print_log_screen("* Speaking Mode *");
    i2s_RX_init();
    xTaskCreate(i2s_read_task, "i2s_read_task", 4096, NULL, 1, &i2sReadTaskHandler);
  }else if(requestTimestamp != timestamp ){
    if(states == Idle){
      states = Listening;
      i2s_TX_init();
      print_log_screen("* Listening Mode *");
    }else if(states == Listening){
      states = Idle;
      i2s_TX_uninst();
      print_log_screen("* IDLE Mode *");
    }
  }
}

void start_to_connect(){
  WiFi.begin(ssid, password);
  print_log_screen("- WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  print_log_screen("+ WiFi Connected");
  print_log_screen("- Socket Connecting");
  
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  while(!client.connect(websockets_server_host, websockets_server_port, "/")){
    delay(500);
    Serial.print(".");
  }

  print_log_screen("+ Socket Connected");
}

static void i2s_read_task(void *arg){
  while (1) {
      i2s_read_data();
      client.sendBinary((const char*)flash_write_buff, I2S_READ_LEN);
  }
}

static void ping_task(void *arg){
  char dummy_buffer [1];
  while (1) {
    if(states == Idle){
      client.sendBinary(dummy_buffer, 1); 
    }
    vTaskDelay(1000);
  }
}

void pressed(Button2& btn) {
   if(btn == rButton && states == Idle){
    client.send(getStrTimestamp());
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
  requestTimestamp = millis() + timeStampOffset;
  return (String)requestTimestamp;
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
  tft.drawString(deviceRole == Server ? "[Server Ready]" : "[Clinet Ready]", tft.width()/2, 100, GFXFF);
}

void print_log_screen(String text){
  logScreen.drawString(text, 0, -1, 2);
  logScreen.pushSprite(0, tft.height()/2);
  logScreen.scroll(0,16);
}