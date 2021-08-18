#ifndef Display_H_
#define Display_H_

#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "SPIFFS.h"

class Display {
private:
  TFT_eSPI* tft;
  friend bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t * bitmap);

public:
  Display();
  ~Display();

  void initTFT();
  void centerMsg(String text);
  void tempUpdates(String temp, String hum, String status);
  void showWiFiIcon(bool isOn);
  void showFirebaseIcon(bool isOn);
};


#endif