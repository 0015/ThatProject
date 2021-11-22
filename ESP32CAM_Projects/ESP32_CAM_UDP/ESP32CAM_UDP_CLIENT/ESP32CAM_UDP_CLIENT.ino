/////////////////////////////////////////////////////////////////
/*
  ESP32CAM | Sending images via UDP?
  Video Tutorial: https://youtu.be/1kHxd5FOUEU
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiUdp.h>

#define CHUNK_LENGTH 1460

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

const char* ssid = "That-Project";
const char* password = "California";
const int udpPort = 8000;
const char* udpAddress = "192.168.4.1";

boolean connected = false;
WiFiUDP udp;

void setup() {

  Serial.begin(115200);

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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_CIF;  // 400x296
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //Connect to the WiFi network
  connectToWiFi(ssid, password);
}

void loop() {
  //only send data when connected
  if (connected) {
    camera_fb_t* fb = NULL;
    esp_err_t res = ESP_OK;
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      esp_camera_fb_return(fb);
      return;
    }

    if (fb->format != PIXFORMAT_JPEG) {
      Serial.println("PIXFORMAT_JPEG not implemented");
      esp_camera_fb_return(fb);
      return;
    }
    sendPacketData((const char*)fb->buf, fb->len, CHUNK_LENGTH);
    esp_camera_fb_return(fb);
  }
}

void connectToWiFi(const char* ssid, const char* pwd) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
  }
}

void sendPacketData(const char* buf, uint16_t len, uint16_t chunkLength) {
  uint8_t buffer[chunkLength];
  size_t blen = sizeof(buffer);
  size_t rest = len % blen;

  for (uint8_t i = 0; i < len / blen; ++i) {
    memcpy(buffer, buf + (i * blen), blen);
    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, chunkLength);
    udp.endPacket();
  }

  if (rest) {
    memcpy(buffer, buf + (len - rest), rest);
    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, rest);
    udp.endPacket();
  }
}