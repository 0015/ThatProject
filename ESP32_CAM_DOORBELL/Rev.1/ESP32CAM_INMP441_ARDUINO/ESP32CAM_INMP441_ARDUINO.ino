#include "esp_camera.h"
#include <WiFi.h>
#include <esp_http_client.h>
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include "slm.h"
#define CAPTURE_TASK_PRI 1
#define CAPTURE_TASK_STACK 4096
#define THRESHOLD 68

const char* ssid = "anelalove";
const char* password = "lifeisegg";
TaskHandle_t TaskHandle_SLM;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  slm_task();
  double _Leq_dB;
  while(xQueueReceive(samples_queue, &_Leq_dB, portMAX_DELAY)){
    Serial.println(_Leq_dB);

    if(_Leq_dB > THRESHOLD){
      vTaskDelete(TaskHandle_SLM);
      mic_i2s_uninstall();

      Serial.println("Capture TASK START");
      vTaskDelay(1000);
      xTaskCreate(capture_task, "Capture Task", CAPTURE_TASK_STACK, NULL, CAPTURE_TASK_PRI, NULL);
    }
  }
}

void camInit(){
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
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; //1600x1200
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void capture_task(void* parameter){
  camInit();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    ESP.restart();
  }

  if(fb->format != PIXFORMAT_JPEG){
    Serial.println("Non-JPEG data not implemented");
    ESP.restart();
  }

  esp_http_client_config_t config = {
    .url = "http://192.168.1.124:8888/imageUpdate",
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_post_field(client, (const char*)fb->buf, fb->len);
  esp_http_client_set_method(client, HTTP_METHOD_POST);
  esp_http_client_set_header(client, "Content-type", "image/jpeg");
  esp_err_t err = esp_http_client_perform(client);
  if(err == ESP_OK)
    Serial.println("Frame Uploaded");
  else
    Serial.printf("Failed to upload frame, error %d\r\n", err);

  esp_http_client_cleanup(client);
  esp_camera_fb_return(fb);
  WiFi.disconnect(true);
  ESP.restart();
}

void slm_task(){
  samples_queue = xQueueCreate(8, sizeof(sum_queue_t));
  xTaskCreate(mic_i2s_reader_task, "MIC I2S Reader", I2S_TASK_STACK, NULL, I2S_TASK_PRI, &TaskHandle_SLM);
}

void loop() {}
