/////////////////////////////////////////////////////////////////
/*
  T-SIMCAM, Upload images anywhere with an 4G LTE modem.
  For More Information: https://youtu.be/kOYJ-4oZ8Ws
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

/*Verison*/
//ESP32 Arduino 2.0.7
//TinyGSM 0.11.5

#define TINY_GSM_MODEM_SIM7600
#include "config.h"
#include "esp_camera.h"
#include <HardwareSerial.h>
#include <TinyGsmClient.h>

const char server[] = "34.127.93.100";
const char resource[] = "/upload";
const int port = 3000;

#define CHUNK_SIZE        1500
#define TINY_GSM_USE_GPRS true

const char apn[] = "h2g2";
const char gprsUser[] = "";
const char gprsPass[] = "";

TinyGsm modem(Serial1);
TinyGsmClient client(modem, 0);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(PWR_ON_PIN, OUTPUT);
  digitalWrite(PWR_ON_PIN, HIGH);

  Serial1.begin(115200, SERIAL_8N1, PCIE_RX_PIN, PCIE_TX_PIN);
  pinMode(PCIE_PWR_PIN, OUTPUT);
  digitalWrite(PCIE_PWR_PIN, 1);
  delay(500);
  digitalWrite(PCIE_PWR_PIN, 0);
  Serial.println("Wait...");
  delay(3000);

  Serial.println("Initializing modem...");
  modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  // 2 Automatic , 13 GSM only , 38 LTE only , 51 GSM and LTE only
  int res;
  do {
    res = modem.setNetworkMode(38);
    Serial.print("Modem Set LTE: ");
    Serial.println(res);
    delay(500);

  } while (res != 1);

  Serial1.println("AT+CPSI?"); // Get connection type and band
  delay(500);
  if (Serial1.available()) {
    String r = Serial1.readString();
    Serial.println(r);
  }

  Serial.println("\n---End of GPRS TEST---\n");

  camera_init();
}

void loop() {
  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println("Modem Fail");
    delay(10000);
    return;
  }
  Serial.println(" success");

  if (modem.isNetworkConnected()) {
    Serial.println("Network connected");
  }

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("GPRS fail");
    delay(10000);
    return;
  }

  if (modem.isGprsConnected()) {
    Serial.println("GPRS connected");
    takeShot();
  }

  delay(10000); // 10sec
}

void takeShot() {

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[ERROR] GRAB Image");
    return;
  }

  Serial.println("[SUCCESS] Take a Shot!");

  Serial.printf("[Photo] Photo Size : %d Bytes\n", fb->len);

  if (!client.connect(server, port)) {
    Serial.println("... failed");
  } else {

    unsigned long currentTotalTime = millis();
    client.print(String("POST ") + resource + " HTTP/1.1\r\n");
    client.print(String("Host: ") + server + "\r\n");
    client.print("Content-Length: ");
    client.print(fb->len);
    client.print("\r\n");
    client.print("Content-Type: image/jpeg\r\n");
    client.println();
    client.flush();

    uint8_t tmp[CHUNK_SIZE] = {0};
    size_t blen = sizeof(tmp);
    size_t i = 0;
    size_t len = fb->len;
    int sent_size = 0;

    for (i = 0; i < len / blen; ++i) {
      memcpy(tmp, fb->buf + (i * blen), blen);
      sent_size = client.write(tmp, CHUNK_SIZE);
    }

    if (len % blen) {
      size_t rest = len % blen;
      memcpy(tmp, fb->buf + (len - len % blen), rest);
      sent_size = client.write(tmp, rest);
    }
    Serial.printf("Time taken to upload a single image: %d ms\n", millis() - currentTotalTime);

    // Wait for data to arrive
    uint32_t start = millis();
    while (client.connected() && !client.available() && millis() - start < 30000L) {
      delay(100);
    };

    // Read data
    start = millis();
    char logo[640] = {
        '\0',
    };
    int read_chars = 0;
    while (client.connected() && millis() - start < 10000L) {
      while (client.available()) {
        logo[read_chars] = client.read();
        logo[read_chars + 1] = '\0';
        read_chars++;
        start = millis();
      }
    }
    Serial.println(logo);

    client.stop();
  }

  esp_camera_fb_return(fb);
}

void camera_init() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = CAM_Y2_PIN;
  config.pin_d1 = CAM_Y3_PIN;
  config.pin_d2 = CAM_Y4_PIN;
  config.pin_d3 = CAM_Y5_PIN;
  config.pin_d4 = CAM_Y6_PIN;
  config.pin_d5 = CAM_Y7_PIN;
  config.pin_d6 = CAM_Y8_PIN;
  config.pin_d7 = CAM_Y9_PIN;
  config.pin_xclk = CAM_XCLK_PIN;
  config.pin_pclk = CAM_PCLK_PIN;
  config.pin_vsync = CAM_VSYNC_PIN;
  config.pin_href = CAM_HREF_PIN;
  config.pin_sscb_sda = CAM_SIOD_PIN;
  config.pin_sscb_scl = CAM_SIOC_PIN;
  config.pin_pwdn = CAM_PWDN_PIN;
  config.pin_reset = CAM_RESET_PIN;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // camera init
  Serial.println("Camera init");
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
    return;
  }
}