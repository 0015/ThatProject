/////////////////////////////////////////////////////////////////
/*
  LVGL 9, [Ep. 03] Creating Stunning UI on ESP32 with LVGL: A Step-by-Step Guide #LVGL #UI #ESP32
  For More Information: https://youtu.be/PdpRFgsxTwg
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// Version Info
// ESP32                v2.0.14
// LVGL                 v9.0.0
// TFT_eSPI             v2.5.43
// FT6236(Dustin Watts) v1.0.2
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FT6236.h>

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

void ui_init() {
  static lv_style_t style_base;
  lv_style_init(&style_base);
  lv_style_set_border_width(&style_base, 0);

  lv_obj_t *screen = lv_obj_create(lv_screen_active());
  lv_obj_set_size(screen, TFT_HOR_RES, TFT_VER_RES);
  lv_obj_center(screen);
  lv_obj_add_style(screen, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  LV_IMAGE_DECLARE(img_bg);
  lv_obj_t *main_bg = lv_image_create(screen);
  lv_image_set_src(main_bg, &img_bg);
  lv_obj_align(main_bg, LV_ALIGN_CENTER, 0, 0);

  LV_IMAGE_DECLARE(img_user);
  lv_obj_t *user_placeholder = lv_image_create(screen);
  lv_image_set_src(user_placeholder, &img_user);
  lv_obj_align(user_placeholder, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *user_label = lv_label_create(screen);
  lv_label_set_text(user_label, "Windows Insider");
  lv_obj_set_style_text_color(user_label, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_set_style_text_font(user_label, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_align(user_label, LV_ALIGN_CENTER, 0, -30);

  static lv_style_t style_textarea;
  lv_style_init(&style_textarea);
  lv_style_set_border_width(&style_textarea, 2);
  lv_style_set_border_color(&style_textarea, lv_color_hex(0xffffff));
  lv_style_set_radius(&style_textarea, 0);
  lv_style_set_bg_color(&style_textarea, lv_color_hex(0x000000));
  lv_style_set_bg_opa(&style_textarea, LV_OPA_70);

  lv_obj_t *user_password = lv_textarea_create(screen);
  lv_obj_align(user_password, LV_ALIGN_CENTER, 0, 20);
  lv_textarea_set_placeholder_text(user_password, "PIN");
  lv_obj_set_size(user_password, 200, 40);
  lv_obj_add_style(user_password, &style_textarea, LV_PART_MAIN);

  static lv_style_t style_panel;
  lv_style_init(&style_panel);
  lv_style_set_border_width(&style_panel, 0);
  lv_style_set_bg_opa(&style_panel, LV_OPA_TRANSP);
  lv_style_set_pad_all(&style_panel, 0);

  lv_obj_t *menu_panel = lv_obj_create(screen);
  lv_obj_set_size(menu_panel, 140, 70);
  lv_obj_align_to(menu_panel, user_password, LV_ALIGN_TOP_MID, 0, 40);
  lv_obj_add_style(menu_panel, &style_panel, LV_PART_MAIN);

  lv_obj_t *sign_in_label = lv_label_create(menu_panel);
  lv_label_set_text(sign_in_label, "Sign-in options");
  lv_obj_set_style_text_color(sign_in_label, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(sign_in_label, LV_ALIGN_TOP_MID, 0, 0);

  LV_IMAGE_DECLARE(icon_key);
  lv_obj_t *img_btn_key = lv_imagebutton_create(menu_panel);
  lv_imagebutton_set_src(img_btn_key, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &icon_key, NULL);
  lv_obj_align(img_btn_key, LV_ALIGN_BOTTOM_LEFT, 10, 0);

  LV_IMAGE_DECLARE(icon_numpad);
  lv_obj_t *img_btn_pad = lv_imagebutton_create(menu_panel);
  lv_imagebutton_set_src(img_btn_pad, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &icon_numpad, NULL);
  lv_obj_align(img_btn_pad, LV_ALIGN_BOTTOM_RIGHT, -10, 0);
}
