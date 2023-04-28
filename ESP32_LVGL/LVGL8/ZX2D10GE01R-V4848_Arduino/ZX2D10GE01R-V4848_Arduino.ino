/*
 * Project ZX2D10GE01R-V4848 for Arduino
 * Description: The base project that can be used in the Arduino environment using the ZX2D10GE01R-V4848 device
 * Author: Eric Nam
 * Date: 03-18-2023
 */

// ** Prerequisites **
// ESP32 Arduino 2.0.7 based on ESP-IDF 4.4.4
// https://github.com/espressif/arduino-esp32

// LVGL version 8.3.5
// https://github.com/lvgl/lvgl

// GFX Library for Arduino 1.3.2
// https://github.com/moononournation/Arduino_GFX

// ZX2D10GE01R-V4848 for ESP-IDF
// https://github.com/wireless-tag-com/ZX2D10GE01R-V4848

#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "button.hpp"
#include "mt8901.hpp"

#define ECO_O(y) (y > 0) ? -1 : 1
#define ECO_STEP(x) x ? ECO_O(x) : 0
#define GFX_BL 38

Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED /* DC */, 21 /* CS */,
  47 /* SCK */, 41 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  39 /* DE */, 48 /* VSYNC */, 40 /* HSYNC */, 45 /* PCLK */,
  10 /* R0 */, 16 /* R1 */, 9 /* R2 */, 15 /* R3 */, 46 /* R4 */,
  8 /* G0 */, 13 /* G1 */, 18 /* G2 */, 12 /* G3 */, 11 /* G4 */, 17 /* G5 */,
  47 /* B0 */, 41 /* B1 */, 0 /* B2 */, 42 /* B3 */, 14 /* B4 */,
  1 /* hsync_polarity */, 10 /* hsync_front_porch */, 10 /* hsync_pulse_width */, 10 /* hsync_back_porch */,
  1 /* vsync_polarity */, 14 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 12 /* vsync_back_porch */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  bus, GFX_NOT_DEFINED /* RST */, st7701_type7_init_operations, sizeof(st7701_type7_init_operations));

static button_t *g_btn;
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;
static lv_group_t *lv_group;
static lv_obj_t *lv_btn_1;
static lv_obj_t *lv_btn_2;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  static int16_t cont_last = 0;
  int16_t cont_now = mt8901_get_count();
  data->enc_diff = ECO_STEP(cont_now - cont_last);
  cont_last = cont_now;
  if (button_isPressed(g_btn)) {
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200);

  gfx->begin();
  gfx->fillScreen(BLACK);

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  lv_init();

  // Hardware Button
  g_btn = button_attch(3, 0, 10);

  // Magnetic Encoder
  mt8901_init(5, 6);

  screenWidth = gfx->width();
  screenHeight = gfx->height();

  // Must use PSRAM
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 32, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 32);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = encoder_read;
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    lv_indev_drv_register(&indev_drv);

    init_lv_group();
    lv_example_get_started_1();
  }
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}

void init_lv_group() {
  lv_group = lv_group_create();
  lv_group_set_default(lv_group);

  lv_indev_t *cur_drv = NULL;
  for (;;) {
    cur_drv = lv_indev_get_next(cur_drv);
    if (!cur_drv) {
      break;
    }

    if (cur_drv->driver->type == LV_INDEV_TYPE_ENCODER) {
      lv_indev_set_group(cur_drv, lv_group);
    }
  }
}

static void btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {

    uint8_t cnt = 0;
    if (btn == lv_btn_1) {
      static uint8_t cnt_1 = 0;
      cnt_1++;
      cnt = cnt_1;
    } else {
      static uint8_t cnt_2 = 0;
      cnt_2++;
      cnt = cnt_2;
    }
    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
  }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1(void) {
  lv_btn_1 = lv_btn_create(lv_scr_act());                          /*Add a button the current screen*/
  lv_obj_set_size(lv_btn_1, 120, 50);                              /*Set its size*/
  lv_obj_align(lv_btn_1, LV_ALIGN_CENTER, 0, -40);                 /*Set its position*/
  lv_obj_add_event_cb(lv_btn_1, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t *label = lv_label_create(lv_btn_1); /*Add a label to the button*/
  lv_label_set_text(label, "Button");          /*Set the labels text*/
  lv_obj_center(label);

  lv_btn_2 = lv_btn_create(lv_scr_act());                                             /*Add a button the current screen*/
  lv_obj_set_style_bg_color(lv_btn_2, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN); /*Set its main color*/

  lv_obj_set_size(lv_btn_2, 120, 50);                              /*Set its size*/
  lv_obj_align(lv_btn_2, LV_ALIGN_CENTER, 0, 40);                  /*Set its position*/
  lv_obj_add_event_cb(lv_btn_2, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t *label2 = lv_label_create(lv_btn_2); /*Add a label to the button*/
  lv_label_set_text(label2, "Button");          /*Set the labels text*/
  lv_obj_center(label2);
}
