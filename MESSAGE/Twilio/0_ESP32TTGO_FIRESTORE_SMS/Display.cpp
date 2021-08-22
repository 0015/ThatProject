#include "Display.h"

static Display* instance = NULL;

Display::Display(){
  instance = this;
  tft = new TFT_eSPI();
}

Display::~Display(){
  delete tft;
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t * bitmap){
  if (y >= instance->tft->height()) return 0;
  instance->tft->pushImage(x, y, w, h, bitmap);
  return 1;
}

void Display::initTFT(){
  tft->init();
  tft->fillScreen(TFT_BLACK);
  tft->setRotation(1);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setTextDatum(MC_DATUM);
  tft->setFreeFont(&Orbitron_Light_24);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  if(!SPIFFS.begin()){
    while(1) yield();
  }
}

void Display::centerMsg(String text){
  tft->drawString(text, tft->width()/2, 60);
}

void Display::tempUpdates(String temp, String hum, String status){
  tft->setTextPadding(tft->width());
  tft->drawString(temp, tft->width()/2, 40);
  tft->drawString(hum, tft->width()/2, 70);
  tft->drawString(status, tft->width()/2, 110);
}

void Display::showWiFiIcon(bool isOn){
  tft->fillRect(tft->width() -30, 0, 30, 30, TFT_BLACK);
  TJpgDec.drawFsJpg(tft->width() -30, 0, isOn ? "/icon_wifi_on.jpg" : "/icon_wifi_off.jpg");
}

void Display::showFirebaseIcon(bool isOn){
  tft->fillRect(tft->width() -60, 0, 30, 30, TFT_BLACK);
  TJpgDec.drawFsJpg(tft->width() -60, 0, isOn ? "/icon_firebase_on.jpg" : "/icon_firebase_off.jpg");
}










