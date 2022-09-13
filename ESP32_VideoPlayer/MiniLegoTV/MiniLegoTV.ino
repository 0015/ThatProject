/////////////////////////////////////////////////////////////////
/*
  Mini Lego TV based on ESP32
  For More Information: https://youtu.be/2TOVohmUqOE
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
// This project is based on a Mini Retro TV.
// https://www.instructables.com/Mini-Retro-TV/ 
/////////////////////////////////////////////////////////////////

/***
 * Required libraries:
 * https://github.com/moononournation/Arduino_GFX.git
 * https://github.com/pschatzmann/arduino-libhelix.git
 * https://github.com/bitbank2/JPEGDEC.git
 */

/***
* Added libraries:
* https://github.com/vshymanskyy/Preferences
* https://github.com/fbiego/CST816S
*/

#define FPS 30
#define MJPEG_BUFFER_SIZE (288 * 240 * 2 / 8)
#define AUDIOASSIGNCORE 1
#define DECODEASSIGNCORE 0
#define DRAWASSIGNCORE 1

#include "config.h"
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <Preferences.h>

Preferences preferences;
#define APP_NAME "video_player"
#define K_VIDEO_INDEX "video_index"
#define BASE_PATH "/Videos/"
#define AAC_FILENAME "/44100.aac"
#define MJPEG_FILENAME "/288_30fps.mjpeg"
#define VIDEO_COUNT 20

/* Arduino_GFX */
#include <Arduino_GFX_Library.h>
Arduino_DataBus *bus = new Arduino_ESP32SPI(DC, CS, SCK, MOSI, MISO, VSPI);
Arduino_GFX *gfx = new Arduino_ST7789(bus, RST, 1, true, ST7789_TFTWIDTH, ST7789_TFTHEIGHT, 0, 20, 0, 20);

/* Audio */
#include "esp32_audio_task.h"

/* MJPEG Video */
#include "mjpeg_decode_draw_task.h"

/* Touch */
#include <CST816S.h>
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ);

/* Variables */
static int next_frame = 0;
static int skipped_frames = 0;
static unsigned long start_ms, curr_ms, next_frame_ms;
static unsigned int video_idx = 1;

// pixel drawing callback
static int drawMCU(JPEGDRAW *pDraw) {
  unsigned long s = millis();
  gfx->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  total_show_video_ms += millis() - s;
  return 1;
} /* drawMCU() */

void setup() {
  disableCore0WDT();

  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);

  // Init Display
  gfx->begin(80000000);
  gfx->fillScreen(BLACK);
  pinMode(BLK, OUTPUT);
  digitalWrite(BLK, HIGH);

  xTaskCreate(
    touchTask,
    "touchTask",
    2000,
    NULL,
    1,
    NULL);

  Serial.println("Init I2S");

  esp_err_t ret_val = i2s_init(I2S_NUM_0, 44100, I2S_MCLK, I2S_SCLK, I2S_LRCLK, I2S_DOUT, I2S_DIN);
  if (ret_val != ESP_OK) {
    Serial.printf("i2s_init failed: %d\n", ret_val);
    gfx->println("i2s_init failed");
    return;
  }
  i2s_zero_dma_buffer(I2S_NUM_0);

  Serial.println("Init FS");

  SPIClass spi = SPIClass(HSPI);
  spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, spi, 80000000)) {
    Serial.println("ERROR: File system mount failed!");
    gfx->println("ERROR: File system mount failed!");
    return;
  }

  preferences.begin(APP_NAME, false);
  video_idx = preferences.getUInt(K_VIDEO_INDEX, 1);
  Serial.printf("videoIndex: %d\n", video_idx);

  gfx->setCursor(20, 20);
  gfx->setTextColor(GREEN);
  gfx->setTextSize(6, 6, 0);
  gfx->printf("CH %d", video_idx);
  delay(1000);

  playVideoWithAudio(video_idx);
}

void loop() {
}

void touchTask(void *parameter) {
  touch.begin();
  while (1) {
    if (touch.available()) {
      Serial.printf("x: %d, y: %d\n", touch.data.x, touch.data.y);
      switch (touch.data.gestureID) {
        case SWIPE_LEFT:
          Serial.println("SWIPE_LEFT");
          break;
        case SWIPE_RIGHT:
          Serial.println("SWIPE_RIGHT");
          break;
        case SWIPE_UP:
          Serial.println("SWIPE_UP (RIGHT)");
          videoController(-1);
          break;
        case SWIPE_DOWN:
          Serial.println("SWIPE_DOWN (LEFT)");
          videoController(1);
          break;
        case SINGLE_CLICK:
          Serial.println("SINGLE_CLICK");
          break;
        case DOUBLE_CLICK:
          Serial.println("DOUBLE_CLICK");
          break;
        case LONG_PRESS:
          Serial.println("LONG_PRESS");
          break;
      }
    }
    vTaskDelay(1000);
  }
}

void playVideoWithAudio(int channel) {

  char aFilePath[40];
  sprintf(aFilePath, "%s%d%s", BASE_PATH, channel, AAC_FILENAME);

  File aFile = SD.open(aFilePath);
  if (!aFile || aFile.isDirectory()) {
    Serial.printf("ERROR: Failed to open %s file for reading\n", aFilePath);
    gfx->printf("ERROR: Failed to open %s file for reading\n", aFilePath);
    return;
  }

  char vFilePath[40];
  sprintf(vFilePath, "%s%d%s", BASE_PATH, channel, MJPEG_FILENAME);

  File vFile = SD.open(vFilePath);
  if (!vFile || vFile.isDirectory()) {
    Serial.printf("ERROR: Failed to open %s file for reading\n", vFilePath);
    gfx->printf("ERROR: Failed to open %s file for reading\n", vFilePath);
    return;
  }

  Serial.println("Init video");

  mjpeg_setup(&vFile, MJPEG_BUFFER_SIZE, drawMCU, false, DECODEASSIGNCORE, DRAWASSIGNCORE);

  Serial.println("Start play audio task");

  BaseType_t ret = aac_player_task_start(&aFile, AUDIOASSIGNCORE);

  if (ret != pdPASS) {
    Serial.printf("Audio player task start failed: %d\n", ret);
    gfx->printf("Audio player task start failed: %d\n", ret);
  }

  Serial.println("Start play video");

  start_ms = millis();
  curr_ms = millis();
  next_frame_ms = start_ms + (++next_frame * 1000 / FPS / 2);
  while (vFile.available() && mjpeg_read_frame())  // Read video
  {
    total_read_video_ms += millis() - curr_ms;
    curr_ms = millis();

    if (millis() < next_frame_ms)  // check show frame or skip frame
    {
      // Play video
      mjpeg_draw_frame();
      total_decode_video_ms += millis() - curr_ms;
      curr_ms = millis();
    } else {
      ++skipped_frames;
      Serial.println("Skip frame");
    }

    while (millis() < next_frame_ms) {
      vTaskDelay(pdMS_TO_TICKS(1));
    }

    curr_ms = millis();
    next_frame_ms = start_ms + (++next_frame * 1000 / FPS);
  }
  int time_used = millis() - start_ms;
  int total_frames = next_frame - 1;
  Serial.println("AV end");
  vFile.close();
  aFile.close();

  videoController(1);
}

void videoController(int next) {

  video_idx += next;
  if (video_idx <= 0) {
    video_idx = VIDEO_COUNT;
  } else if (video_idx > VIDEO_COUNT) {
    video_idx = 1;
  }
  Serial.printf("video_idx : %d\n", video_idx);
  preferences.putUInt(K_VIDEO_INDEX, video_idx);
  preferences.end();
  esp_restart();
}