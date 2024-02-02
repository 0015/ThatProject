/////////////////////////////////////////////////////////////////
/*
  LVGL 9, From Scratch to Benchmark, For newbie/beginner/starter #LVGL #UI #ESP32 #Benchmark
  For More Information: https://youtu.be/s4_fUwhw-dI
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// Version Info
// ESP32                v2.0.14
// LVGL                 v9.0.0
// TFT_eSPI             v2.5.34
// FT6236(Dustin Watts) v1.0.2

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FT6236.h>
#include <demos/lv_demos.h>

#define SDA_FT6236 8
#define SCL_FT6236 3
FT6236 ts = FT6236();

#define TFT_HOR_RES 480
#define TFT_VER_RES 320

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
void *draw_buf_1;
unsigned long lastTickMillis = 0;

static lv_display_t *disp;

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (ts.touched()) {

    TS_Point p = ts.getPoint();
    data->point.x = TFT_HOR_RES - p.y;
    data->point.y = p.x;
    Serial.printf("x:%d, y:%d\n", data->point.x, data->point.y);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);

  if (!ts.begin(40, SDA_FT6236, SCL_FT6236)) {
    Serial.println("Unable to start the capacitive touch Screen.");
  }

  lv_init();

  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  String LVGL_Arduino = "Hello Arduino!\n";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);

  lv_demo_benchmark();
}

void loop() {
  // LVGL Tick Interface
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();

  // LVGL Task Handler
  lv_task_handler();
}