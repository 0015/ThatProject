/////////////////////////////////////////////////////////////////
/*
  Home Assistant with multiple ESP32 Cameras! (Part. 2)
  For More Information: https://youtu.be/MjZjyK1WgjY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

/*
- ESP32 Arduino Core: 2.0.9
- TestDevice: ESP32-CAM AI-Thinker, XIAO ESP32S3 Sense

**Required Library**
- MQTT: 2.5.1
https://github.com/256dpi/arduino-mqtt
*/

#include "esp_camera.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>

//Select Your Camera Board
//#define CAMERA_MODEL_XIAO_ESP32S3
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char ssid[] = "<YOUR_WIFI_SSID>";
const char pass[] = "<YOUR_WIFI_PASSWORD>";

#define CONFIG_BROKER_URL "<YOUR_HA_IP_ADDRESS>"

#define CONFIG_BROKER_USERNAME "MQTT_Camera"
#define CONFIG_BROKER_PASSWORD "1234"

//For the First Camera
#define ESP32CAM_DEVICE "ESP32-CAM-0"
#define ESP32CAM_PUBLISH_TOPIC "esp32/cam_0"
//For the Second Camera
//#define ESP32CAM_DEVICE "ESP32-CAM-1"
//#define ESP32CAM_PUBLISH_TOPIC "esp32/cam_1"

const int bufferSize = 1024 * 25;  // 25KB

WiFiClient net;
MQTTClient client = MQTTClient(bufferSize);

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(ESP32CAM_DEVICE, CONFIG_BROKER_USERNAME, CONFIG_BROKER_PASSWORD)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

bool camInit() {
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
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
      config.frame_size = FRAMESIZE_VGA;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_VGA;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }

  return true;
}

void uploadImage() {
  Serial.println("uploadImage via MQTT");
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("Non-JPEG data not implemented");
    esp_camera_fb_return(fb);
    return;
  }

  if (!client.publish(ESP32CAM_PUBLISH_TOPIC, (const char*)fb->buf, fb->len)) {
    Serial.println("[Failure] Uploading Image via MQTT");
  }

  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  if (!camInit()) {
    Serial.println("[Failure] Cam Init");
    return;
  }
  WiFi.begin(ssid, pass);
  client.begin(CONFIG_BROKER_URL, 1883, net);
  connect();
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  } else {
    uploadImage();
  }
}
