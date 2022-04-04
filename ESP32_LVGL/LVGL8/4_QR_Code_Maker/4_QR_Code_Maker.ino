/////////////////////////////////////////////////////////////////
/*
  QR Code Maker (ESP32+LVGL8)
  For More Information: https://youtu.be/Ss3zBO-V9kI
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
#include <lvgl.h>
#include "MyDisplay.h"
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

lv_obj_t * mainScreen;
lv_obj_t * titleImage;
lv_obj_t * qrCode;
LV_IMG_DECLARE(ui_logo_img_obj);

void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready( disp );
}

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  if (ts.touched()) {
    data->state = LV_INDEV_STATE_PR;
    TS_Point p = ts.getPoint();
    data->point.x = p.x;
    data->point.y = p.y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup()
{
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(0);
  tft.setBrightness(255);

  if (!ts.begin(40, SDA_FT6236, SCL_FT6236)) {
    Serial.println("Unable to start the capacitive touch Screen.");
  }

  lv_init();
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();
}

void loop() {
  lv_timer_handler();
  delay( 5 );
}

void ui_init() {
  ui_background();
  ui_dynamic_obj();
  lv_disp_load_scr(mainScreen);
}

void ui_background() {
  mainScreen = lv_obj_create(NULL);
  lv_obj_clear_flag(mainScreen, LV_OBJ_FLAG_SCROLLABLE);

  titleImage = lv_img_create(mainScreen);
  lv_img_set_src(titleImage, &ui_logo_img_obj);
  lv_obj_set_size(titleImage, 320, 117);
  lv_obj_set_pos(titleImage, 0, 0);
  lv_obj_set_align(titleImage, LV_ALIGN_TOP_MID);
  lv_obj_add_flag(titleImage, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(titleImage, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t * titleLabel = lv_label_create(mainScreen);
  lv_obj_set_pos(titleLabel, 0, 120);
  lv_obj_set_align(titleLabel, LV_ALIGN_TOP_MID);
  lv_label_set_text(titleLabel, "QR Code Maker");
  lv_obj_clear_flag(titleLabel, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ta_event_cb(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * ta = lv_event_get_target(e);
  lv_obj_t * kb = (lv_obj_t*)lv_event_get_user_data(e);

  if (code == LV_EVENT_READY) {
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    const char * text = lv_textarea_get_text(ta);
    if (strlen(text) == 0) return;
    lv_qrcode_update(qrCode, text, strlen(text));
    lv_obj_clear_flag(qrCode, LV_OBJ_FLAG_HIDDEN);
  }

  if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(qrCode, LV_OBJ_FLAG_HIDDEN);
  }

  if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(kb, NULL);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
  }
}

void ui_dynamic_obj(void) {
  lv_obj_t * kb = lv_keyboard_create(mainScreen);
  lv_obj_t * ta = lv_textarea_create(mainScreen);
  lv_obj_align(ta, LV_ALIGN_CENTER, 0, -40);
  lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
  lv_obj_set_size(ta, screenWidth - 40, 60);
  lv_keyboard_set_textarea(kb, ta);

  qrCode = lv_qrcode_create(mainScreen, 200, lv_color_hex3(0x000), lv_color_hex3(0xeef));
  lv_obj_set_pos(qrCode, 0, -20);
  lv_obj_set_align(qrCode, LV_ALIGN_BOTTOM_MID);
  lv_obj_add_flag(qrCode, LV_OBJ_FLAG_HIDDEN);
}
