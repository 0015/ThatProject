/////////////////////////////////////////////////////////////////
/*
  LVGL 9, [Ep. 06] My Air Quality Sensor, Draw IKEA UI
  For More Information: https://youtu.be/LQ_iq_gYlqY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
// Version Info
// ESP32                v2.0.16
// LVGL                 v9.1.0
// LovyanGFX            v1.1.12

#include "LGFX_WT32-SC01-Plus.h"
#include <lvgl.h>

#define TFT_HOR_RES 320
#define TFT_VER_RES 480

static LGFX tft;
/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
void *draw_buf_1;

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

void setup() {
  Serial.begin(115200);

  display_init();

  lv_init();

  lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf_1, NULL, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();
}

void loop() {
  // LVGL Tick Interface
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();

  // LVGL Task Handler
  lv_task_handler();
}

void display_init() {
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(32);
  tft.fillScreen(TFT_BLACK);
}

static void anim_rotate_cb(void *var, int32_t v) {
  lv_image_set_rotation((lv_obj_t *)var, v);
}

void ui_init() {
  // Base Style
  static lv_style_t style_base;
  lv_style_init(&style_base);
  lv_style_set_border_width(&style_base, 0);
  lv_style_set_pad_all(&style_base, 0);
  lv_style_set_bg_opa(&style_base, LV_OPA_TRANSP);
  lv_style_set_radius(&style_base, 0);

  // Main screen
  lv_obj_t *screen = lv_obj_create(lv_screen_active());
  lv_obj_set_size(screen, TFT_HOR_RES, TFT_VER_RES);
  lv_obj_center(screen);
  lv_obj_add_style(screen, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  // Draw
  LV_IMAGE_DECLARE(top_img);
  lv_obj_t *ui_top_img = lv_image_create(screen);
  lv_image_set_src(ui_top_img, &top_img);
  lv_obj_align(ui_top_img, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *ui_pm_panel = lv_obj_create(screen);
  lv_obj_set_size(ui_pm_panel, TFT_HOR_RES, 160);
  lv_obj_align_to(ui_pm_panel, ui_top_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_add_style(ui_pm_panel, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(ui_pm_panel, LV_OBJ_FLAG_SCROLLABLE);

  LV_FONT_DECLARE(font_arial_25);
  lv_obj_t *ui_pm_unit_label = lv_label_create(ui_pm_panel);
  lv_label_set_text(ui_pm_unit_label, "PM2.5");
  lv_obj_set_style_text_font(ui_pm_unit_label, &font_arial_25, 0);
  lv_obj_align(ui_pm_unit_label, LV_ALIGN_TOP_MID, 0, 0);

  LV_FONT_DECLARE(font_segment_90);
  lv_obj_t *ui_pm_label = lv_label_create(ui_pm_panel);
  lv_label_set_text(ui_pm_label, "0");
  lv_obj_set_style_text_font(ui_pm_label, &font_segment_90, 0);
  lv_obj_align(ui_pm_label, LV_ALIGN_RIGHT_MID, -108, 4);

  static lv_style_t square_style;
  lv_style_init(&square_style);
  lv_style_set_radius(&square_style, 5);
  lv_style_set_border_width(&square_style, 0);
  lv_style_set_bg_opa(&square_style, LV_OPA_COVER);

  lv_obj_t *red_square_obj = lv_obj_create(ui_pm_panel);
  lv_obj_add_style(red_square_obj, &square_style, 0);
  lv_obj_set_style_bg_color(red_square_obj, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_align(red_square_obj, LV_ALIGN_TOP_RIGHT, -52, 32);
  lv_obj_set_size(red_square_obj, 32, 32);

  lv_obj_t *yellow_square_obj = lv_obj_create(ui_pm_panel);
  lv_obj_add_style(yellow_square_obj, &square_style, 0);
  lv_obj_align(yellow_square_obj, LV_ALIGN_TOP_RIGHT, -52, 68);
  lv_obj_set_size(yellow_square_obj, 32, 32);
  lv_obj_set_style_bg_color(yellow_square_obj, lv_palette_main(LV_PALETTE_YELLOW), 0);

  lv_obj_t *green_square_obj = lv_obj_create(ui_pm_panel);
  lv_obj_add_style(green_square_obj, &square_style, 0);
  lv_obj_set_style_bg_color(green_square_obj, lv_palette_main(LV_PALETTE_GREEN), 0);
  lv_obj_align(green_square_obj, LV_ALIGN_TOP_RIGHT, -52, 104);
  lv_obj_set_size(green_square_obj, 32, 32);

  LV_IMAGE_DECLARE(pm_cursor);
  lv_obj_t *ui_pm_cursor = lv_image_create(ui_pm_panel);
  lv_image_set_src(ui_pm_cursor, &pm_cursor);
  lv_obj_align(ui_pm_cursor, LV_ALIGN_TOP_RIGHT, -36, 104);

  lv_obj_t *ui_temp_panel = lv_obj_create(screen);
  lv_obj_set_size(ui_temp_panel, TFT_HOR_RES, 160);
  lv_obj_align_to(ui_temp_panel, ui_pm_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_add_style(ui_temp_panel, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(ui_temp_panel, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *ui_f_unit_label = lv_label_create(ui_temp_panel);
  lv_label_set_text(ui_f_unit_label, "°F");
  lv_obj_set_style_text_font(ui_f_unit_label, &font_arial_25, 0);
  lv_obj_align(ui_f_unit_label, LV_ALIGN_TOP_RIGHT, -80, 0);

  LV_FONT_DECLARE(font_segment_60);
  lv_obj_t *ui_temp_label = lv_label_create(ui_temp_panel);
  lv_label_set_text(ui_temp_label, "00");
  lv_obj_set_style_text_font(ui_temp_label, &font_segment_60, 0);
  lv_obj_align_to(ui_temp_label, ui_f_unit_label, LV_ALIGN_TOP_RIGHT, -30, 0);

  LV_IMAGE_DECLARE(temp_icon);
  lv_obj_t *ui_temp_icon = lv_image_create(ui_temp_panel);
  lv_image_set_src(ui_temp_icon, &temp_icon);
  lv_obj_align(ui_temp_icon, LV_ALIGN_TOP_LEFT, 50, 10);

  lv_obj_t *ui_percent_unit_label = lv_label_create(ui_temp_panel);
  lv_label_set_text(ui_percent_unit_label, "%");
  lv_obj_set_style_text_font(ui_percent_unit_label, &font_arial_25, 0);
  lv_obj_align(ui_percent_unit_label, LV_ALIGN_BOTTOM_RIGHT, -80, 0);

  lv_obj_t *ui_humidity_label = lv_label_create(ui_temp_panel);
  lv_label_set_text(ui_humidity_label, "00");
  lv_obj_set_style_text_font(ui_humidity_label, &font_segment_60, 0);
  lv_obj_align_to(ui_humidity_label, ui_percent_unit_label, LV_ALIGN_BOTTOM_RIGHT, -30, 0);

  LV_IMAGE_DECLARE(humi_icon);
  lv_obj_t *ui_humi_icon = lv_image_create(ui_temp_panel);
  lv_image_set_src(ui_humi_icon, &humi_icon);
  lv_obj_align(ui_humi_icon, LV_ALIGN_BOTTOM_LEFT, 50, -10);

  lv_obj_t *ui_tvoc_panel = lv_obj_create(screen);
  lv_obj_set_size(ui_tvoc_panel, TFT_HOR_RES, 95);
  lv_obj_align_to(ui_tvoc_panel, ui_temp_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_add_style(ui_tvoc_panel, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(ui_tvoc_panel, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *ui_tvoc_label = lv_label_create(ui_tvoc_panel);
  lv_label_set_text(ui_tvoc_label, "tVOC");
  lv_obj_set_style_text_font(ui_tvoc_label, &font_arial_25, 0);
  lv_obj_align(ui_tvoc_label, LV_ALIGN_LEFT_MID, 40, 0);

  LV_IMAGE_DECLARE(tvoc_arrow);
  lv_obj_t *ui_tvoc_arrow = lv_image_create(ui_tvoc_panel);
  lv_image_set_src(ui_tvoc_arrow, &tvoc_arrow);
  lv_obj_align_to(ui_tvoc_arrow, ui_tvoc_label, LV_ALIGN_OUT_RIGHT_MID, 25, 0);

  lv_anim_t anim_rotate_img;
  lv_anim_init(&anim_rotate_img);
  lv_anim_set_var(&anim_rotate_img, ui_tvoc_arrow);
  lv_anim_set_exec_cb(&anim_rotate_img, anim_rotate_cb);
  lv_anim_set_duration(&anim_rotate_img, 1000);
  lv_anim_set_repeat_count(&anim_rotate_img, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_playback_duration(&anim_rotate_img, 1000);
  lv_anim_set_values(&anim_rotate_img, 0, 3600);
  lv_anim_start(&anim_rotate_img);

  Serial.println("UI DRAW Done");
}
