/////////////////////////////////////////////////////////////////
/*
  Turntable MP3 Player!
  For More Information: https://youtu.be/QDrJKkUJmFE
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

/*
- Device
ESP32-S3 Parallel IPS with Touch 4.0" 480*480 ST7701
https://www.makerfabs.com/esp32-s3-parallel-tft-with-touch-4-inch.html

- Library
Arduino ESP32: 2.0.7
LVGL: 8.3.6
GFX Library for Arduino: 1.3.4
ESP32 AudioI2S: 3.0.0
*/

#include <Arduino_GFX_Library.h> // https://github.com/moononournation/Arduino_GFX
#include <SD.h>      
#include "Audio.h"  // https://github.com/schreibfaul1/ESP32-audioI2S
#include "ui.h"

#define I2S_DOUT 19
#define I2S_BCLK 20
#define I2S_LRCK 46

#define SD_SCK 12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS 10

Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED /* DC */, 1 /* CS */,
  12 /* SCK */, 11 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  45 /* DE */, 4 /* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
  39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
  0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
  6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */,
  1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
  1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  bus, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));

#include "touch.h"

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

Audio audio;
TaskHandle_t audioInstanceTaskHandle = NULL;
bool isPlaying = false;

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

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200);
  // Init touch device
  touch_init();

  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);

  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();
#ifdef ESP32
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 32, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#else
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 10);
#endif
  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 10);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    action_init();

    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS)) {
      Serial.println(F("ERROR: File System Mount Failed!"));
    }

    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    audio.setVolume(2);  // 0...21
    audio.connecttoFS(SD, "/FineDining.mp3");
    audio.pauseResume();
    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 20480, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(Task_Audio, "Task_Audio", 10240, NULL, 3, NULL, 1);
  }
}

void Task_TFT(void *pvParameters) {
  while (1) {
    lv_timer_handler();
  }
}

void Task_Audio(void *pvParameters) {
  while (1) {
    audio.loop();
    if (isPlaying) {
      if (audio.getAudioFileDuration() < audio.getAudioCurrentTime()) {
        audio_done();
      }
    }
  }
}

void Task_Audio_Command(void *pvParameters) {
  vTaskDelay(2500);
  audio.pauseResume();
  audioInstanceTaskHandle = NULL;
  vTaskDelete(NULL);
}

void loop() {}

void audio_done() {
  isPlaying = !isPlaying;
  animation_play(isPlaying);
  // TODO: Need to restart process or play next song
}

void animation_play(bool isOn) {
  yield();
  if (isOn) {
    RecordAnim_Animation(ui_Record, 0);
    HandAnim_Animation(ui_Hand, 1000);
    RecordAnim2_Animation(ui_Record, 2000);
    if (audioInstanceTaskHandle == NULL)
      xTaskCreate(Task_Audio_Command, "Task_Audio_Command", 1024, NULL, 1, &audioInstanceTaskHandle);
  } else {
    lv_anim_del_all();
    HandAnimOff_Animation(ui_Hand, 0);
    RecordAnimOff_Animation(ui_Record, 1000);
    audio.pauseResume();
  }
}

static void value_changed_event_cb(lv_event_t *e) {
  lv_obj_t *arc = lv_event_get_target(e);
  int16_t _val = lv_arc_get_value(arc);
  int vol_angle = map(_val, 0, 100, 0, 2700);
  lv_img_set_angle(ui_Volume, vol_angle);
  int vol_audio = map(_val, 0, 100, 0, 21);
  audio.setVolume(vol_audio);
}

static void btn_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    isPlaying = !isPlaying;
    animation_play(isPlaying);
  }
}

void action_init() {
  lv_obj_add_event_cb(ui_PowerButton, btn_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_add_event_cb(ui_VolumeArc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_arc_set_value(ui_VolumeArc, 0);
  lv_event_send(ui_VolumeArc, LV_EVENT_VALUE_CHANGED, NULL);
}
