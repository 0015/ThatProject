#include "Display.h"

Display::Display() {
  tft = new TFT_eSPI();
}

Display::~Display() {
  delete tft;
}

void Display::initTFT() {
  tft->init();
  tft->fillScreen(TFT_BLACK);
  tft->setRotation(1);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setTextDatum(MC_DATUM);
  tft->setFreeFont(&Orbitron_Light_24);
  tft->drawString("Waiting for WIFI-", tft->width() / 2, 50);
}

void Display::fillBlackScreen() {
  tft->fillScreen(TFT_BLACK);
}

void Display::showVersion(int buildNum) {
  String text = "Build#: ";
  text += buildNum;
  tft->drawString(text, tft->width() / 2, 80);
}

void Display::timeUpdate(String date, String time) {
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setTextDatum(MC_DATUM);
  //tft->setFreeFont(&Orbitron_Light_24);
  tft->setFreeFont(&Satisfy_24);
  tft->setTextPadding(0);
  tft->drawString(date, tft->width() / 2, 50);
  tft->setTextPadding(tft->width() - 20);
  tft->drawString(time, tft->width() / 2, 80);
}

void Display::showVersionBelow(int buildNum) {
  tft->setFreeFont(&FreeSansBold9pt7b);
  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  tft->setTextDatum(BL_DATUM);
  String text = "Current Build#: ";
  text += buildNum;
  tft->drawString(text, 0, tft->height());
}

void Display::newMessage(String msg) {
  tft->setFreeFont(&FreeSansBold9pt7b);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->setTextDatum(TR_DATUM);
  tft->drawString(msg, tft->width(), 0);
}

void Display::downloadScreen(int percent) {
  if (percent == 0) {
    this->fillBlackScreen();
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->setFreeFont(&Orbitron_Light_24);
    tft->setTextDatum(MC_DATUM);
    tft->drawString("Downloading...", tft->width() / 2, 50);
  }

  this->drawProgressBar(20, tft->height() / 2 + 20, tft->width() - 40, 20, percent, TFT_RED, TFT_YELLOW);
}

void Display::drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor) {
  if (percent == 0) {
    tft->fillRoundRect(x, y, w, h, 3, TFT_BLACK);
  }
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth = w - 2 * margin;
  tft->drawRoundRect(x, y, w, h, 3, frameColor);
  tft->fillRect(x + margin, y + margin, barWidth * percent / 100.0, barHeight, barColor);
}

void Display::downloadFailure(String cpName) {
  this->fillBlackScreen();
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setFreeFont(&Orbitron_Light_24);
  tft->setTextDatum(MC_DATUM);
  tft->drawString("MD5 checksum", tft->width() / 2, 10);
  tft->drawString("Wrong!", tft->width() / 2, 40);
  tft->drawString("Contact to ", tft->width() / 2, 70);
  tft->drawString(cpName, tft->width() / 2, 100);
}

void Display::downloadSuccess() {
  this->fillBlackScreen();
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setFreeFont(&Orbitron_Light_24);
  tft->setTextDatum(MC_DATUM);
  tft->drawString("MD5 checksum", tft->width() / 2, 10);
  tft->drawString("Correct!", tft->width() / 2, 40);
  tft->drawString("Countdown", tft->width() / 2, 70);

  tft->setTextColor(TFT_RED, TFT_BLACK);
  tft->setTextPadding(tft->width() - 20);
  for (int i = 3; i > -1; --i) {
    tft->drawString(String(i), tft->width() / 2, 100);
    delay(1000);
  }
}

void Display::firmwareScreen(bool isStart, bool isDone) {
  this->fillBlackScreen();
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setFreeFont(&Orbitron_Light_24);
  tft->setTextDatum(MC_DATUM);

  if (isStart) {
    tft->drawString("Updating", tft->width() / 2, 40);
    tft->drawString("Firmware!", tft->width() / 2, 70);
    return;
  }

  if (isDone) {
    tft->drawString("OTA Done!", tft->width() / 2, 10);
    tft->drawString("Rebooting", tft->width() / 2, 40);
    tft->drawString("Countdown", tft->width() / 2, 70);

  } else {
    tft->drawString("Sorry.", tft->width() / 2, 10);
    tft->drawString("OTA Failure!", tft->width() / 2, 40);
    tft->drawString("Try it again.", tft->width() / 2, 70);
  }

  tft->setTextColor(TFT_RED, TFT_BLACK);
  tft->setTextPadding(tft->width() - 20);
  for (int i = 3; i > -1; --i) {
    tft->drawString(String(i), tft->width() / 2, 100);
    delay(1000);
  }
}
