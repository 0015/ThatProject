// LVGL 8 + SquareLine Studio 3D Printer Example for Arduino Giga Display Bundle(Arduino GIGA R1 WiFi and GIGA Display Shield)
// https://youtube.com/shorts/bhMVPaYz91A

// Resolution: 480x800 RGB
// Arduino Mbed OS Giga Boards    4.1.1
// Arduino GigaDisplay            1.0.1
// Arduino GigaDisplayTouch       1.0.1
// Arduino GigaDisplayGFX         1.0.0
// LVGL Version                   8.4.0

// LVGL Settings (lv_conf.h)
// #define LV_MEM_SIZE (128U * 1024U)
// #define LV_FONT_MONTSERRAT_14 1
// #define LV_FONT_MONTSERRAT_26 1

#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"

#include "lvgl.h"
#include "ui.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield); /* Arduino_H7_Video Display(1024, 768, USBCVideo); */
Arduino_GigaDisplayTouch  TouchDetector;

void setup()
{
  Display.begin();
  TouchDetector.begin();
  Serial.begin(115200);

  ui_init();
}

void loop()
{
  lv_timer_handler();
}