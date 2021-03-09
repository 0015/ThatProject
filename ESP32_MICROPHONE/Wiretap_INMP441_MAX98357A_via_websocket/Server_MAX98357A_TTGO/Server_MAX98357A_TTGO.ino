#include <SPI.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "driver/i2s.h"
#include "Free_Fonts.h"

#define I2S_WS_TX  12
#define I2S_SCK_TX 13
#define I2S_DATA_OUT_TX  15

#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (32)
#define UPDATE_INTERVAL   (500)

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite graph = TFT_eSprite(&tft);

const i2s_config_t i2s_config_tx = {
  .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = I2S_SAMPLE_RATE,
  .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 32,
  .dma_buf_len = 64
};

const i2s_pin_config_t pin_config_tx = {
  .bck_io_num = I2S_SCK_TX,
  .ws_io_num = I2S_WS_TX,
  .data_out_num = I2S_DATA_OUT_TX,
  .data_in_num = I2S_PIN_NO_CHANGE
};

TaskHandle_t drawGraphTaskHandler = NULL;

unsigned long last_update_sent = 0;
bool isConnected; 
static int sampleValue;

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.listen(8888);
  
  displayInit();
  readyScreen();

  xTaskCreate(server_task, "server_task", 4096, NULL, 1, NULL);
}
static void draw_graph_task(void *arg)
{
  int preHeight = 0;

  while(1){
    if(isConnected){
      int calcHeight = sampleValue;
      graph.drawLine(tft.width() - 2, preHeight, tft.width() - 1, calcHeight, 1);
      preHeight = calcHeight;
      tft.setBitmapColor(TFT_WHITE, TFT_BLACK);
      graph.pushSprite(0, 0);
      graph.scroll(-2, 0);
    }else{
      vTaskDelay(100);
    } 
  }
}

static void server_task(void *arg)
{
  while(1){
    if(server.poll()){
        client = server.accept();
        i2sInit();
        last_update_sent = millis();
        isConnected = true;
        xTaskCreate(draw_graph_task, "draw_graph_task", 2048, NULL, 1, &drawGraphTaskHandler);
      }

     if(isConnected && millis() - last_update_sent > UPDATE_INTERVAL) {
      vTaskDelete(drawGraphTaskHandler);
      Serial.println("Client disconnected! Uninstall I2S driver");
      isConnected = false;      
      i2s_driver_uninstall(I2S_PORT);
      readyScreen();
    }
    vTaskDelay(100);
  }
}

void loop() {
  i2s_write_from_client();
}

void i2s_write_from_client()
{   
  if(client.available()){
    last_update_sent = millis();
    client.poll();
    WebsocketsMessage msg = client.readBlocking();
    char *buf_ptr = (char*)msg.c_str();
    int bytes_written = msg.length();
    i2s_write_bytes(I2S_PORT, buf_ptr, bytes_written, portMAX_DELAY);

    int32_t sample = (int32_t) buf_ptr[3] << 24;
    sample |=  (int32_t) buf_ptr[2] << 16;
    sample |= (int32_t) buf_ptr[1] << 8;
    sample |= (int32_t) buf_ptr[0];   

    sampleValue = map(sample / 100000, -4000, 4000, 0, tft.height());
  }
}

void i2sInit(){
  i2s_driver_install(I2S_PORT, &i2s_config_tx, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config_tx);
}

void displayInit(){
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  graph.setColorDepth(1);
  graph.createSprite(tft.width(), tft.height());
  graph.fillSprite(TFT_BLACK);  
}

void readyScreen(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("WIRELESS", 10, 10, GFXFF);
 
  tft.setFreeFont(FF24);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("WIRETAP", 10, 40, GFXFF);

  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Waiting for the other side", 0, 95, GFXFF);
  tft.setFreeFont(FF9);
  tft.drawString("Version 0.1", 0, 115, GFXFF);
}
