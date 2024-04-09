/////////////////////////////////////////////////////////////////
/*
  LVGL 9, [Ep. 04] Creating Interactive UI/UX Screens with LVGL 9 on ESP32: Implementing On-Screen Keyboard
  For More Information: https://youtu.be/ogXSDtYLQ7M
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// Version Info
// ESP32                v2.0.14
// LVGL                 v9.1.0
// TFT_eSPI             v2.5.43
// FT6236(Dustin Watts) v1.0.2
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FT6236.h>

#define SDA_FT6236 8
#define SCL_FT6236 3
FT6236 ts = FT6236();

#define TFT_HOR_RES 320
#define TFT_VER_RES 480

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
void *draw_buf_1;
unsigned long lastTickMillis = 0;

static lv_display_t *disp;
lv_obj_t *screen;
TFT_eSPI tft = TFT_eSPI();

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (ts.touched()) {

    TS_Point p = ts.getPoint();
    // data->point.x = TFT_HOR_RES - p.y;
    // data->point.y = p.x;

    // Portrait(2) tft.setRotation(2);
    data->point.x = TFT_HOR_RES - p.x;
    data->point.y = TFT_VER_RES - p.y;
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

  tft.setRotation(2);

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

static void anim_y_cb(void *var, int32_t v) {
  lv_obj_set_y((lv_obj_t *)var, v);
}

static void anim_screen(bool isMovingUp) {
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, screen);

  if (isMovingUp) {
    lv_anim_set_duration(&a, 500);
    lv_anim_set_values(&a, lv_obj_get_y(screen), -100);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
  } else {
    lv_anim_set_duration(&a, 100);
    lv_anim_set_values(&a, lv_obj_get_y(screen), 0);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
  }
  lv_anim_set_exec_cb(&a, anim_y_cb);
  lv_anim_start(&a);
}

static void ta_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);

  if (code == LV_EVENT_FOCUSED) {
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
    anim_screen(true);

  } else if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(kb, NULL);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    anim_screen(false);
  }
}

void ui_init() {
  // Base Style
  static lv_style_t style_base;
  lv_style_init(&style_base);
  lv_style_set_border_width(&style_base, 0);
  lv_style_set_pad_all(&style_base, 0);

  // Main screen
  screen = lv_obj_create(lv_screen_active());         // Get the active screen of the default display
  lv_obj_set_size(screen, TFT_HOR_RES, TFT_VER_RES);  // Set the size of lv object
  lv_obj_center(screen);                              // Set make it center
  lv_obj_add_style(screen, &style_base, LV_PART_MAIN);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  // Draw background img
  LV_IMAGE_DECLARE(bg);
  lv_obj_t *main_bg = lv_image_create(screen);
  lv_image_set_src(main_bg, &bg);
  lv_obj_add_style(main_bg, &style_base, LV_PART_MAIN);
  lv_obj_align(main_bg, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *hello_label = lv_label_create(screen);
  lv_label_set_text(hello_label, "Hello There!");
  lv_obj_set_style_text_color(hello_label, lv_color_hex(0x68fbb4), LV_PART_MAIN);
  lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_align(hello_label, LV_ALIGN_TOP_LEFT, 30, 30);

  lv_obj_t *hello_sub_label = lv_label_create(screen);
  lv_label_set_text(hello_sub_label, "Nice to see you again...");
  lv_obj_set_style_text_color(hello_sub_label, lv_color_hex(0x68fbb4), LV_PART_MAIN);
  lv_obj_set_style_text_font(hello_sub_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_align_to(hello_sub_label, hello_label, LV_ALIGN_TOP_LEFT, 2, 30);

  // Style for textarea
  static lv_style_t style_textarea;
  lv_style_init(&style_textarea);
  lv_style_set_border_width(&style_textarea, 0);
  lv_style_set_radius(&style_textarea, 16);
  lv_style_set_bg_color(&style_textarea, lv_color_hex(0x84a2a4));
  lv_style_set_bg_opa(&style_textarea, LV_OPA_70);
  lv_style_set_text_color(&style_textarea, lv_color_white());

  // Create On-Screen Keyboard
  lv_obj_t *kb = lv_keyboard_create(lv_screen_active());
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);  // Hide Keyboard

  lv_obj_t *user_id = lv_textarea_create(screen);
  lv_obj_align(user_id, LV_ALIGN_CENTER, 0, -80);
  lv_textarea_set_placeholder_text(user_id, "Id");
  lv_textarea_set_one_line(user_id, true);
  lv_obj_set_size(user_id, 240, 40);
  lv_obj_add_style(user_id, &style_textarea, LV_PART_MAIN);
  lv_obj_add_event_cb(user_id, ta_event_cb, LV_EVENT_ALL, kb);

  lv_obj_t *user_pw = lv_textarea_create(screen);
  lv_obj_align(user_pw, LV_ALIGN_CENTER, 0, -20);
  lv_textarea_set_placeholder_text(user_pw, "Password");
  lv_textarea_set_one_line(user_pw, true);
  lv_textarea_set_password_mode(user_pw, true);
  lv_obj_set_size(user_pw, 240, 40);
  lv_obj_add_style(user_pw, &style_textarea, LV_PART_MAIN);
  lv_obj_add_event_cb(user_pw, ta_event_cb, LV_EVENT_ALL, kb);

  // Style for login button
  static lv_style_t style_button;
  lv_style_init(&style_button);
  lv_style_set_border_width(&style_button, 0);
  lv_style_set_radius(&style_button, 16);
  lv_style_set_bg_color(&style_button, lv_color_hex(0x64efae));
  lv_style_set_text_color(&style_button, lv_color_black());

  lv_obj_t *login_btn = lv_button_create(screen); /*Add a button the current screen*/
  lv_obj_align(login_btn, LV_ALIGN_LEFT_MID, 40, 40);
  lv_obj_set_size(login_btn, 120, 40); /*Set its size*/
  lv_obj_add_style(login_btn, &style_button, LV_PART_MAIN);

  lv_obj_t *login_btn_label = lv_label_create(login_btn); /*Add a label to the button*/
  lv_label_set_text(login_btn_label, "Login");            /*Set the labels text*/
  lv_obj_center(login_btn_label);
}