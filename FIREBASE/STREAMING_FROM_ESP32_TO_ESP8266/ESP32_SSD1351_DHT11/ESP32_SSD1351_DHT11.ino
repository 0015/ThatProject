#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 13
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define DC_PIN 17
#define CS_PIN 5
#define RST_PIN 4
#define BLACK 0x0000
#define RED   0xF800
#define WHITE 0xFFFF
#define BLUE  0x001F

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  TFTInit();
  DHTInit();
}

void loop() {
  delay(1000);
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);
  if(isnan(h) || isnan(f)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hi = dht.computeHeatIndex(f, h);
  Serial.printf("Temp: %f, Humidity: %f, Heat Index: %f\n", f, h, hi);

  String temp = String(f);
  temp += "F";
  tft.fillRect(0, 28, 80, 24, BLACK);
  tftDrawText(4, 46, temp, RED);

  String hum = String(h);
  hum += "%";
  tft.fillRect(46, 82, SCREEN_WIDTH-46, 24, BLACK);
  tftDrawText(SCREEN_WIDTH - 80, SCREEN_HEIGHT -26, hum, BLUE);

  String ht = "*";
  ht += String(hi);
  ht += "*";
  tft.fillRect(0, SCREEN_HEIGHT/3 + 10, SCREEN_WIDTH, 30, BLACK);
  tftDrawText(SCREEN_WIDTH/3 - 20, SCREEN_HEIGHT/2 + 10, ht, WHITE);
}

void TFTInit(){
  tft.begin();
  tft.fillScreen(BLACK);
  tft.drawLine(0, SCREEN_HEIGHT, SCREEN_WIDTH, 0, WHITE);
  tft.setFont(&FreeSansBold12pt7b);
  tftDrawText(4, 20, "Temp", RED);
  tftDrawText(SCREEN_WIDTH - 36, SCREEN_HEIGHT -2, "RH", BLUE);
  tft.setFont(&FreeSans12pt7b);
}

void tftDrawText(int x, int y, String text, uint16_t color){
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(text);
}

void DHTInit(){
  dht.begin();
}
