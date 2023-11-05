/*
- Hardware
Main Board: WT32-SC01 Plus
https://youtu.be/KPiDBOOTJ8g

UWB: REYAX TECHNOLOGY RYUW122 Lite

- Library
Arduino ESP32: 2.0.14
LVGL: 8.3.19
LovyanGFX: 1.1.9
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <vector>
#include <lvgl.h>
#include "ui.h"
#include "LGFX_WT32-SC01-Plus.h"

static const int RXPin = 11, TXPin = 10;
static const uint32_t UWBBaud = 115200;

std::vector<char> vBuf;

static LGFX tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {

  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;

  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  initSerial();
  initDisplay();
  initUWB();
  initLVGL();

  xTaskCreatePinnedToCore(uwb_task,
                          "uwb_task",
                          6000,
                          NULL,
                          0,
                          NULL,
                          1);
}

void loop() {
  lv_timer_handler();
  delay(5);
}

void initSerial(){
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXPin, TXPin);
}

void initDisplay() {
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(8, 16);
}

void initLVGL() {
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();
}

void initUWB() {
  tft.println("***Init UWB***");
  Serial2.println("AT");
  update_serial();  // Serial Flush

  delay(1000);
  tft.println();
  tft.println("AT Check");
  Serial2.println("AT");
  tft.println(update_serial());

  delay(1000);
  tft.println("AT MODE");
  Serial2.println("AT+MODE=1");
  tft.println(update_serial());

  delay(1000);
  tft.println("AT+NETWORKID\nSet the network ID.");
  Serial2.println("AT+NETWORKID=REYAX123");
  tft.println(update_serial());

  delay(1000);
  tft.println("AT+ADDRESS\nSet the ADDRESS ID of module.");
  Serial2.println("AT+ADDRESS=REYAX003");
  tft.println(update_serial());

  delay(1000);
  tft.println("AT+CPIN");
  Serial2.println("AT+CPIN=FABC0002EEDCAA90FABC0002EEDCAA90");
  tft.println(update_serial());

  delay(2000);
}

String update_serial() {

  vBuf.clear();
  while (Serial2.available()) {
    vBuf.push_back(char(Serial2.read()));
  }
  std::string str(vBuf.begin(), vBuf.end());
  return str.c_str();
}

void uwb_task(void *pvParameters) {

  while (1) {
    Serial2.println("AT+ANCHOR_SEND=DAVID123,4,TEST");
    String data = update_serial();

    if (data.indexOf("OK") > 0) {
      int lastData = data.lastIndexOf(',') + 1;
      String distance = data.substring(lastData, data.length());
      lv_label_set_text(ui_contentText, distance.c_str());
    }

    vTaskDelay(500);
  }
}
