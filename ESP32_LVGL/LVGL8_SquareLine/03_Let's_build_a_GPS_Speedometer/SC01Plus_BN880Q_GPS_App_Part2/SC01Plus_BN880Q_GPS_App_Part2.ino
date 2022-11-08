/////////////////////////////////////////////////////////////////
/*
  Developing Embedded GUI with Square Line Studio [2/2]
  For More Information: https://youtu.be/b_4m1A3A2yw
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <Wire.h>
// TinyGpsPlus
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPSPlus.h>
// EspSoftwareSerial
// https://github.com/plerup/espsoftwareserial/
#include <SoftwareSerial.h>
// DFRobot_QMC5883
// https://github.com/DFRobot/DFRobot_QMC5883
#include <DFRobot_QMC5883.h>
// LovyanGFX
// https://github.com/lovyan03/LovyanGFX
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
// LVGL
// https://github.com/lvgl/lvgl
#include <lvgl.h>
#include "ui.h"

static const int RXPin = 10, TXPin = 11, sclPin = 12, sdaPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
DFRobot_QMC5883 compass(&Wire, QMC5883_ADDRESS);

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 40000000;
      cfg.pin_wr = 47;
      cfg.pin_rd = -1;
      cfg.pin_rs = 0;
      cfg.pin_d0 = 9;
      cfg.pin_d1 = 46;
      cfg.pin_d2 = 3;
      cfg.pin_d3 = 8;
      cfg.pin_d4 = 18;
      cfg.pin_d5 = 17;
      cfg.pin_d6 = 16;
      cfg.pin_d7 = 15;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = -1;
      cfg.pin_rst = 4;
      cfg.pin_busy = -1;
      cfg.memory_width = 320;
      cfg.memory_height = 480;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 45;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.i2c_port = 1;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = 6;
      cfg.pin_scl = 5;
      cfg.freq = 400000;
      cfg.x_min = 0;
      cfg.x_max = 320;
      cfg.y_min = 0;
      cfg.y_max = 480;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

static LGFX tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 480;
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
  Serial.begin(115200);
  initDisplay();
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
  ui_reset();

  xTaskCreatePinnedToCore(bn_880q_task,
                          "bn_880q_task",
                          4000,
                          NULL,
                          0,
                          NULL,
                          1);
}

void loop() {
  lv_timer_handler();
  delay(5);
}

void ui_reset() {
  lv_slider_set_value(ui_SpeedSlider, 0, LV_ANIM_OFF);
  lv_obj_clear_flag(ui_SpeedSlider, LV_OBJ_FLAG_CLICKABLE);
  lv_label_set_text(ui_SpeedLabel, "0");
  lv_label_set_text(ui_SpeedLabel2, "0");
  lv_label_set_text(ui_SatellitesLabel, "0");
  lv_label_set_text(ui_CoordinatesLabel, "0\n0");
  lv_label_set_text(ui_AltitudeLabel, "0.00");
}

void bn_880q_task(void *pvParameters) {

  ss.begin(GPSBaud);
  initQMC5883();

  while (1) {

    while (ss.available() > 0) {
      if (gps.encode(ss.read())) {
        updateGPS();
      }
    }
    updateDegree();
    vTaskDelay(10);
  }
}

void initDisplay() {
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
}

void initQMC5883() {
  Wire.begin(sdaPin, sclPin);
  while (!compass.begin()) {
    Serial.println("Could not find a valid 5883 sensor, check wiring!");
    delay(500);
  }

  if (compass.isQMC()) {
    Serial.println("Initialize QMC5883");
    compass.setRange(QMC5883_RANGE_2GA);
    Serial.print("compass range is:");
    Serial.println(compass.getRange());

    compass.setMeasurementMode(QMC5883_CONTINOUS);
    Serial.print("compass measurement mode is:");
    Serial.println(compass.getMeasurementMode());

    compass.setDataRate(QMC5883_DATARATE_200HZ);
    Serial.print("compass data rate is:");
    Serial.println(compass.getDataRate());

    compass.setSamples(QMC5883_SAMPLES_8);
    Serial.print("compass samples is:");
    Serial.println(compass.getSamples());
  }
}

void updateGPS() {

  int satellites = 0;
  if (gps.satellites.isValid()) {
    satellites = gps.satellites.value();
    lv_label_set_text(ui_SatellitesLabel, String(satellites).c_str());
  }

  if (satellites == 0) {
    showGPSWaitingSpinner(true);
  } else {
    showGPSWaitingSpinner(false);
  }

  if (gps.speed.isValid()) {
    double mph = gps.speed.mph();
    lv_label_set_text(ui_SpeedLabel, String(mph, 0).c_str());
    lv_label_set_text(ui_SpeedLabel2, String(mph, 0).c_str());
    lv_slider_set_value(ui_SpeedSlider, int(mph), LV_ANIM_ON);
  }

  if (gps.location.isValid()) {
    String coordinates = String(gps.location.lat(), 6);
    coordinates += "\n";
    coordinates += String(gps.location.lng(), 6);
    lv_label_set_text(ui_CoordinatesLabel, coordinates.c_str());
  }

  if (gps.altitude.isValid()) {
    lv_label_set_text(ui_AltitudeLabel, String(gps.altitude.feet(), 2).c_str());
  }
}

void showGPSWaitingSpinner(bool isOn) {

  if (isOn) {
    if (lv_obj_has_flag(ui_GPSWaitingSpinner, LV_OBJ_FLAG_HIDDEN))
      lv_obj_clear_flag(ui_GPSWaitingSpinner, LV_OBJ_FLAG_HIDDEN);
  } else {
    if (!lv_obj_has_flag(ui_GPSWaitingSpinner, LV_OBJ_FLAG_HIDDEN)) {
      lv_obj_add_flag(ui_GPSWaitingSpinner, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void updateDegree() {
  sVector_t mag = compass.readRaw();
  compass.getHeadingDegrees();
  float headingDegree = mag.HeadingDegress;
  float textDegree = 360 - headingDegree;
  lv_label_set_text(ui_CompassLabel, String(textDegree, 0).c_str());
  lv_label_set_text(ui_DegreeLabel, String(textDegree, 0).c_str());
  lv_img_set_angle(ui_CompassImg, (headingDegree * 10));
}