/////////////////////////////////////////////////////////////////
/*
  LVGL 9, [Ep. 05] Custom fonts are essential in GUI!
  For More Information: https://youtu.be/YCTbiHk1MHM
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
// Version Info
// ESP32                v2.0.14
// LVGL                 v9.1.0
// LovyanGFX            v1.1.12

#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "Matouch7.h"

#define TFT_HOR_RES 1024
#define TFT_VER_RES 600

static LGFX tft;

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

void *draw_buf_1;
void *draw_buf_2;

unsigned long lastTickMillis = 0;

/* Display flushing */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)px_map, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  if (!touched) {
    data->state = LV_INDEV_STATE_RELEASED;
  } else {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touchX;
    data->point.y = touchY;
    Serial.printf("x:%d, y:%d\n", data->point.x, data->point.y);
  }
}

void setup(void) {
  Serial.begin(115200);

  // LovyanGFX
  display_init();

  // Initialize LVGL
  lv_init();

  lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  draw_buf_2 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf_1, draw_buf_2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();
}


void loop(void) {
  // LVGL Tick Interface
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();

  // LVGL Task Handler
  lv_timer_handler();
}

void display_init() {
  tft.begin();
  tft.setRotation(0);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
}

const char *demo_stirng = "Everyone has the right to freedom of thought, conscience and religion";
const char *demo_number = "0123456789";

void ui_init() {
  // Base Style
  static lv_style_t style_base;
  lv_style_init(&style_base);
  lv_style_set_border_width(&style_base, 0);
  lv_style_set_pad_all(&style_base, 0);

  // Main screen
  lv_obj_t *screen = lv_obj_create(lv_screen_active());         // Get the active screen of the default display
  lv_obj_set_size(screen, TFT_HOR_RES, TFT_VER_RES);  // Set the size of lv object
  lv_obj_center(screen);                              // Set make it center
  lv_obj_add_style(screen, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
  
  LV_FONT_DECLARE(font_mate_60_1bpp);
  lv_obj_t *ui_font_demo1 = lv_label_create(screen);
  lv_label_set_text(ui_font_demo1, demo_stirng);
  lv_obj_align(ui_font_demo1, LV_ALIGN_TOP_LEFT, 10, 30);
  lv_obj_set_style_text_font(ui_font_demo1, &font_mate_60_1bpp, 0);

  LV_FONT_DECLARE(font_mate_60_4bpp);
  lv_obj_t *ui_font_demo2 = lv_label_create(screen);
  lv_label_set_text(ui_font_demo2, demo_stirng);
  lv_obj_align(ui_font_demo2, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_set_style_text_font(ui_font_demo2, &font_mate_60_4bpp, 0);

  LV_FONT_DECLARE(font_mate_60_4bpp_comp);
  lv_obj_t *ui_font_demo3 = lv_label_create(screen);
  lv_label_set_text(ui_font_demo3, demo_stirng);
  lv_obj_align(ui_font_demo3, LV_ALIGN_BOTTOM_LEFT, 10, -30);
  lv_obj_set_style_text_font(ui_font_demo3, &font_mate_60_4bpp_comp, 0);

  LV_FONT_DECLARE(font_segment_90_bpp1);
  lv_obj_t *ui_font_demo4 = lv_label_create(screen);
  lv_label_set_text(ui_font_demo4, demo_number);
  lv_obj_align(ui_font_demo4, LV_ALIGN_RIGHT_MID, -10, -100);
  lv_obj_set_style_text_font(ui_font_demo4, &font_segment_90_bpp1, 0);

  LV_FONT_DECLARE(font_segment_90);
  lv_obj_t *ui_font_demo5 = lv_label_create(screen);
  lv_label_set_text(ui_font_demo5, demo_number);
  lv_obj_align(ui_font_demo5, LV_ALIGN_RIGHT_MID, -10, 100);
  lv_obj_set_style_text_font(ui_font_demo5, &font_segment_90, 0);
}