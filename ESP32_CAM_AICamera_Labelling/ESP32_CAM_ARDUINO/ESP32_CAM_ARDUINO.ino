#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include <TJpg_Decoder.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define GFXFF 1
#define FSB9 &FreeSerifBold9pt7b

TFT_eSPI tft = TFT_eSPI();

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";
const unsigned long timeout = 30000; // 30 seconds

const int buttonPin = 4;    // the number of the pushbutton pin
int buttonState;             
int lastButtonState = LOW;   
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
bool isNormalMode = true;


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}



void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  pinMode(buttonPin, INPUT);

  Serial.println("INIT DISPLAY");
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_YELLOW);
  tft.setFreeFont(FSB9);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
  
  Serial.println("INIT CAMERA");
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
    config.frame_size = FRAMESIZE_QVGA; // 320x240
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

bool wifiConnect(){
  unsigned long startingTime = millis();
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    if((millis() - startingTime) > timeout){
      return false;
    }
  }
  return true;
}

void buttonEvent(){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == HIGH) {
        isNormalMode = !isNormalMode;

        //Additional Code 
        if(!isNormalMode)
          sendingImage();
        //   
      }
    }
  }
  lastButtonState = reading;
}

camera_fb_t* capture(){
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  fb = esp_camera_fb_get();
  return fb;
}

void showingImage(){
  camera_fb_t *fb = capture();
  if(!fb || fb->format != PIXFORMAT_JPEG){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }else{
    TJpgDec.drawJpg(0,0,(const uint8_t*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  }
}


void parsingResult(String response){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  JsonArray array = doc.as<JsonArray>();
  int yPos = 4;
  for(JsonVariant v : array){
    JsonObject object = v.as<JsonObject>();
    const char* description = object["description"];
    float score = object["score"];
    String label = "";
    label += description;
    label += ":";
    label += score;

    tft.drawString(label, 8, yPos, GFXFF);
    yPos += 16;
  }
}

void postingImage(camera_fb_t *fb){
  HTTPClient client;
  client.begin("http://34.94.219.131:8888/imageUpdate");
  client.addHeader("Content-Type", "image/jpeg");
  int httpResponseCode = client.POST(fb->buf, fb->len);
  if(httpResponseCode == 200){
    String response = client.getString();
    parsingResult(response);
  }else{
    //Error
    tft.drawString("Check Your Server!!!", 8, 4, GFXFF);
  }

  client.end();
  WiFi.disconnect();
}

void sendingImage(){
  camera_fb_t *fb = capture();
  if(!fb || fb->format != PIXFORMAT_JPEG){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }else{
    TJpgDec.drawJpg(0,0,(const uint8_t*)fb->buf, fb->len);

    tft.drawString("Wifi Connecting!", 8, 4, GFXFF);

    if(wifiConnect()){
      //tft.drawString("Wifi Connected!", 8, 4, GFXFF);
      TJpgDec.drawJpg(0,0,(const uint8_t*)fb->buf, fb->len);
      postingImage(fb);
    }else{
      tft.drawString("Check Wifi credential!", 8, 4, GFXFF);
    }
    esp_camera_fb_return(fb);
  }
}

void loop() {
  buttonEvent();
  
  if(isNormalMode)
    showingImage();
  
}
