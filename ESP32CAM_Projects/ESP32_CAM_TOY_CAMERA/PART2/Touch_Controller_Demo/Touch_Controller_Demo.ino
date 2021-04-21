//Original Source Code
//Bodmer TFT_eSPI
//https://github.com/Bodmer/TFT_eSPI/blob/master/examples/480%20x%20320/Touch_Controller_Demo/Touch_Controller_Demo.ino

#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup(void) {
  uint16_t calibrationData[5];

  Serial.begin(115200);
  Serial.println("starting");

  tft.init();

  tft.setRotation(1);
  tft.fillScreen((0xFFFF));

  tft.setCursor(20, 0, 2);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(1);
  tft.println("calibration run");

  tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
  for(int i=0; i<5; i++){
    Serial.printf("%d, %d\n", i,calibrationData[i] );
    }

  tft.fillScreen((0xFFFF));
}

void loop() {
  uint16_t x, y;
  static uint16_t color;

  if (tft.getTouch(&x, &y)) {

    tft.setCursor(5, 5, 2);
    tft.printf("x: %i     ", x);
    tft.setCursor(5, 20, 2);
    tft.printf("y: %i    ", y);

    tft.drawPixel(x, y, color);
    color += 155;
  }
}
