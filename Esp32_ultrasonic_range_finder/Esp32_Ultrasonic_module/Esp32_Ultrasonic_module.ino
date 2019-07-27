#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN   17
#define CS_PIN   5
#define RST_PIN  4
#define TRIG_PIN 0
#define ECHO_PIN 2

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);
long prev_distance;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  initDrawing();

}

void initDrawing(){
  tft.begin();
  tft.fillRect(0, 0, 128, 128, WHITE);
  tft.setRotation(2);
  tft.setCursor(4, 0);
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.print("BT::Not connected!");

  tft.setCursor(54, 100);
  tft.setTextColor(MAGENTA);
  tft.setTextSize(2);
  tft.print("CM");
}

void loop() {
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = microsecondsTocentimeters(duration);
  Serial.print("Ultrasonic Distance : " );
  Serial.print(distance);
  Serial.println(" CM");
  updateDistance(distance);
  delay(30);
}

void updateDistance(long distance){
  if(prev_distance != distance){
    tft.fillRect(0, 30, 128, 70, WHITE);
    tft.setCursor(34, 40);

    if((int)distance < 10){
      tft.setTextColor(RED);
    }else if((int)distance > 10 && (int)distance <=99){
      tft.setTextColor(BLACK);
    }else if((int)distance > 100){
      tft.setTextColor(GREEN);
    }

    tft.setTextSize(4);
    String tempString = "";
    tempString += (int)distance;
    tft.print(tempString);
    prev_distance = distance;
  }
}

long microsecondsTocentimeters(long duration){
  return duration /29 / 2;
}
