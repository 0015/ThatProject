#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#define CHANNEL 1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN 17
#define CS_PIN 5
#define RST_PIN 4
#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
    tft.fillCircle(tft.width() - 10, 10, 6, RED);
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    tft.fillCircle(tft.width() - 10, 10, 6, GREEN);
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.fillScreen(WHITE);
  tftDrawText(4, 4, "SLAVE", BLACK, 2);
  tftDrawRect();
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");

  String textString = String(*data);

  tftDrawRect();
  tftDrawText(tft.width()/2 -25, tft.height()/2 -10, (char*)textString.c_str(), YELLOW, 5);
}

void loop() {
  // Chill
}

void tftDrawText(int x, int y, char *text, uint16_t color, int fontSize){
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(fontSize);
  tft.print(text);
}

void tftDrawRect(){
  tft.fillRect(0, 20, 128, 128, BLACK);
}
