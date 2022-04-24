/////////////////////////////////////////////////////////////////
/*
  My ESP32Tag App for my own ESP32Tag Device!
  For More Information: https://youtu.be/vaYJydZcObU
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
//ESP version 1.0.6
/////////////////////////////////////////////////////////////////

#include <math.h>
#include <lvgl.h>
#include "MyDisplaySetup.h"

static void (*functionPointer)(float);
#include "MyDW1000Ranging.h"

static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
static const uint16_t timer_divider = 80;
static const uint64_t timer_max_count = 1000;
static hw_timer_t *timer = NULL;
static SemaphoreHandle_t bin_sem = NULL;
uint32_t cp0_regs[18];

void IRAM_ATTR onTimer() {

  uint32_t cp_state = xthal_get_cpenable();
  if (cp_state) {
    xthal_save_cp0(cp0_regs);
  } else {
    xthal_set_cpenable(1);
  }

  portENTER_CRITICAL_ISR(&mux);
  DW1000Ranging.loop();
  portEXIT_CRITICAL_ISR(&mux);

  BaseType_t task_woken = pdFALSE;
  xSemaphoreGiveFromISR(bin_sem, &task_woken);
  if (task_woken == pdTRUE) {
    portYIELD_FROM_ISR();
  }

  if (cp_state) {
    xthal_restore_cp0(cp0_regs);
  } else {
    xthal_set_cpenable(0);
  }
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);
  functionPointer = &updateRange;

  tft.begin();
  tft.setRotation(0);
  tft.setBrightness(255);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  drawing_screen();

  bin_sem = xSemaphoreCreateBinary();
  if (bin_sem == NULL) {
    Serial.println("Could not create semaphore");
    ESP.restart();
  }

  xTaskCreatePinnedToCore(lvgl_update_task,
                          "Task LVGL",
                          10000,
                          NULL,
                          1,
                          NULL,
                          1);


  initDW1000();
  setTimerInterrupt();
}

void setTimerInterrupt() {
  timer = timerBegin(0, timer_divider, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, timer_max_count, true);
  timerAlarmEnable(timer);
}

void loop() {}

void lvgl_update_task(void *parameters) {
  while (1) {
    if (xSemaphoreTake(bin_sem, portMAX_DELAY) == pdTRUE) {
      portENTER_CRITICAL(&mux);
      lv_timer_handler();
      portEXIT_CRITICAL(&mux);
    }
    vTaskDelay(80);
  }
}

static lv_obj_t *scr;
static lv_obj_t *chart;
static lv_style_t main_black_style;
static lv_style_t title_style;
static lv_style_t text_style;
static lv_obj_t *distance_label;

static const int chartSize = screenWidth - 40;
static const int center_x = 140;
static const int center_y = 140;
static const float degree = 3.14159 / 180;
static int radius = 130;
static int angle = 0;
static volatile float projectedRange = -1;

static void draw_event_cb(lv_event_t *e) {
  lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
  if (dsc->part == LV_PART_ITEMS) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_chart_series_t *ser = lv_chart_get_series_next(obj, NULL);
    uint32_t cnt = lv_chart_get_point_count(obj);
    dsc->rect_dsc->bg_opa = (LV_OPA_COVER * dsc->id) / (cnt - 1);

    lv_coord_t *x_array = lv_chart_get_x_array(obj, ser);
    lv_coord_t *y_array = lv_chart_get_y_array(obj, ser);

    uint32_t start_point = lv_chart_get_x_start_point(obj, ser);
    uint32_t p_act = (start_point + dsc->id) % cnt;
    lv_opa_t x_opa = (x_array[p_act] * LV_OPA_50) / 320;
    lv_opa_t y_opa = (y_array[p_act] * LV_OPA_50) / 320;

    dsc->rect_dsc->bg_color = lv_color_mix(lv_color_hex(0xFFFFFF),
                                           lv_palette_main(LV_PALETTE_GREY),
                                           x_opa + y_opa);
  }
}

static void drawRandomPoints() {
  lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(0, chartSize), lv_rand(0, chartSize));
}

static void drawPoints() {
  if (angle >= 360) {
    angle = 0;
  }

  int x = center_x + radius * cos(-angle * degree);
  int y = center_y + radius * sin(-angle * degree);

  lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(x - 10, x + 10), lv_rand(y - 10, y + 10));
  angle += 3;
}

static void add_data(lv_timer_t *timer) {
  LV_UNUSED(timer);
  if (projectedRange < 0) {
    drawRandomPoints();
  } else {
    radius = map(projectedRange, 0, 5, 0, 130);
    drawPoints();
  }
}

static void drawing_screen(void) {
  lv_style_init(&main_black_style);
  lv_style_set_border_width(&main_black_style, 0);
  lv_style_set_radius(&main_black_style, 0);
  lv_style_set_bg_color(&main_black_style, lv_color_hex(0x000000));

  //#define LV_FONT_MONTSERRAT_22    1
  lv_style_init(&title_style);
  lv_style_set_text_color(&title_style, lv_color_white());
  lv_style_set_text_font(&title_style, &lv_font_montserrat_22);

  //#define LV_FONT_MONTSERRAT_48    1
  lv_style_init(&text_style);
  lv_style_set_text_color(&text_style, lv_color_white());
  lv_style_set_text_font(&text_style, &lv_font_montserrat_48);

  scr = lv_obj_create(lv_scr_act());
  lv_obj_add_style(scr, &main_black_style, 0);
  lv_obj_set_size(scr, screenWidth, screenHeight);

  lv_obj_t *subtitle_label = lv_label_create(scr);
  lv_label_set_recolor(subtitle_label, true);
  lv_label_set_text(subtitle_label, "#ffffff I'm ESP32TAG#");

  lv_obj_t *title_label = lv_label_create(scr);
  lv_label_set_text(title_label, "DISTANCE FROM ANCHOR");
  lv_obj_add_style(title_label, &title_style, 0);
  lv_obj_align_to(title_label, subtitle_label, LV_ALIGN_TOP_LEFT, -2, 16);

  distance_label = lv_label_create(scr);
  lv_obj_add_style(distance_label, &text_style, 0);
  lv_label_set_text(distance_label, "?");
  lv_obj_align(distance_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  chart = lv_chart_create(scr);
  lv_obj_add_style(chart, &main_black_style, 0);
  lv_obj_set_size(chart, chartSize, chartSize);
  lv_obj_center(chart);
  lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
  lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);

  lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
  lv_chart_set_div_line_count(chart, 0, 0);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, chartSize);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, chartSize);
  lv_chart_set_point_count(chart, 50);

  lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_white(), LV_CHART_AXIS_PRIMARY_Y);
  for (int i = 0; i < 50; i++) {
    lv_chart_set_next_value2(chart, ser, lv_rand(0, chartSize), lv_rand(0, chartSize));
  }

  lv_timer_create(add_data, 3, chart);
}

static void updateRange(float range) {
  if (range <= -1) {
    projectedRange = -1;
    lv_label_set_text(distance_label, "?");
  } else {
    projectedRange = range * 2 / 5;
    String strDistance = String(projectedRange);
    strDistance += " m";
    lv_label_set_text(distance_label, String(strDistance).c_str());
  }
}