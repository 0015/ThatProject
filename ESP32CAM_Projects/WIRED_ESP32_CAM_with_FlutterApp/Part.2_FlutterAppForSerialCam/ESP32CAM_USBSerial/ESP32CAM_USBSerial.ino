/////////////////////////////////////////////////////////////////
/*
  ESP32CAM | Android(Flutter App) Wired Communication - Base64 Encoding/Decoding [2/2]
  Video Tutorial: https://youtu.be/-jygTxRmroA
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include "esp_camera.h"
#include <base64.h>
#define CAMERA_MODEL_AI_THINKER
//#define TTGO_T_CAMERA_PLUS
#include "camera_pins2.h"

#define USE_TFT_ESPI 0

#if USE_TFT_ESPI
  #include <TFT_eSPI.h>
  #include <TJpg_Decoder.h>
  TFT_eSPI tft = TFT_eSPI();
#endif

#define BAUD_RATE 2000000

void setup()
{
  Serial.begin(BAUD_RATE);
  
  displayInit();
  cameraInit();
}

void loop() {
  grabImage();
}

void cameraInit(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA; //320x240
  config.jpeg_quality = 10;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 1);   
  s->set_vflip(s, 1);   
}

void grabImage(){

  camera_fb_t* fb = esp_camera_fb_get();
  if(!fb || fb->format != PIXFORMAT_JPEG){
  }else{
    #if USE_TFT_ESPI
      TJpgDec.drawJpg(-40, 0, (const uint8_t*)fb->buf, fb->len);
    #else
      delay(40);    
    #endif
    
    String encoded = base64::encode(fb->buf, fb->len);
    Serial.write(encoded.c_str(), encoded.length());    
    Serial.println();
  }
  esp_camera_fb_return(fb);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  #if USE_TFT_ESPI
    if( y>= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap); 
    return 1;
  #endif
}

void displayInit(){
  #if USE_TFT_ESPI
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);

    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
  #endif
}
