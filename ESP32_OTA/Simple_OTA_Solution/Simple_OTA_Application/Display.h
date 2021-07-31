#include <TFT_eSPI.h>

class Display {
private:
  TFT_eSPI* tft;
  void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor);

public:
  Display();
  ~Display();
  void initTFT();
  void showVersion(int buildNum);
  void showVersionBelow(int buildNum);
  void fillBlackScreen();
  void timeUpdate(String date, String time);
  void newMessage(String msg);
  void downloadScreen(int percent);
  void downloadFailure(String cpName);
  void downloadSuccess();
  void firmwareScreen(bool isStart, bool isDone);
};
