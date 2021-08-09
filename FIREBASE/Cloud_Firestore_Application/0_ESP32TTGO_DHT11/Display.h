#ifndef Display_H_
#define Display_H_

#include <TFT_eSPI.h>

class Display {
private:
  TFT_eSPI* tft;

public:
  Display();
  ~Display();

  void initTFT();
  void centerMsg(String text);
  void tempUpdates(String temp, String hum, String status);
};


#endif