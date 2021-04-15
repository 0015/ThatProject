/////////////////////////////////////////////////////////////////
/*
  What's different TTGO Camera Plus vs ESP32CAM AI-THINKER?
  Video Tutorial: https://youtu.be/ixtT7kt4YdI
  Created by Eric Nam (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// TFT_eSPI User_Setup_Select.h 
// #include <User_Setups/Setup44_TTGO_CameraPlus.h>   // Setup file for ESP32 and TTGO T-CameraPlus ST7789 SPI bus TFT    240x240
//

#include "esp_camera.h"
#include <WiFi.h>
#include <TFT_eSPI.h>

// Select camera model
#define PWDN_GPIO_NUM       -1
#define RESET_GPIO_NUM      -1
#define XCLK_GPIO_NUM       4
#define SIOD_GPIO_NUM       18
#define SIOC_GPIO_NUM       23

#define Y9_GPIO_NUM         36
#define Y8_GPIO_NUM         37
#define Y7_GPIO_NUM         38
#define Y6_GPIO_NUM         39
#define Y5_GPIO_NUM         35
#define Y4_GPIO_NUM         26
#define Y3_GPIO_NUM         13
#define Y2_GPIO_NUM         34
#define VSYNC_GPIO_NUM      5
#define HREF_GPIO_NUM       27
#define PCLK_GPIO_NUM       25

const char* ssid = "ThatProject";
const char* password = "California";

TFT_eSPI tft = TFT_eSPI();

void startCameraServer(TFT_eSPI *_tft);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  tft.begin();
  tft.setRotation(0);
  tft.setTextColor(TFT_BLACK, TFT_WHITE); 
  tft.fillScreen(TFT_WHITE);
  tft.setTextFont(4);

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
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 1);   
  s->set_vflip(s, 1);    

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  tft.println("");
  tft.println("WiFi connected");
  startCameraServer(&tft);

  tft.print("Camera Ready! Use 'http://");
  tft.print(WiFi.localIP());
  tft.println("'");
  tft.println("to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
