/////////////////////////////////////////////////////////////////
/*
  LVGL 9, [Ep. 07] My Air Quality Sensor, How to update the screen with data obtained from sensors.
  For More Information: https://youtu.be/g9j8FfF2GJ8
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
// Version Info
// ESP32                v3.0.1
// LVGL                 v9.1.0
// LovyanGFX            v1.1.16
// Sensirion I2C Sen5x  v0.3.0 

#include "LGFX_WT32-SC01-Plus.h"
#include <lvgl.h>
#include <SensirionI2CSen5x.h>
#include <Wire.h>

#define TFT_HOR_RES 320
#define TFT_VER_RES 480
#define I2C_SDA 10
#define I2C_SCL 11

// The used commands use up to 48 bytes. On some Arduino's the default buffer
// space is not large enough
#define MAXBUF_REQUIREMENT 48

#if (defined(I2C_BUFFER_LENGTH) && (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

enum PMStatus {
  Green,
  Yellow,
  Red
};

PMStatus _pmStatus;

// Define Queue handle
QueueHandle_t QueueHandle;
const int QueueElementSize = 10;
typedef struct {
  float massConcentrationPm1p0;
  float massConcentrationPm2p5;
  float massConcentrationPm4p0;
  float massConcentrationPm10p0;
  float ambientHumidity;
  float ambientTemperature;
  float vocIndex;
  float noxIndex;
} message_t;

static LGFX tft;

SensirionI2CSen5x sen5x;

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
void *draw_buf_1;

SemaphoreHandle_t gui_mutex;

lv_obj_t *screen;
lv_obj_t *ui_pm_label;
lv_obj_t *ui_temp_label;
lv_obj_t *ui_humidity_label;
lv_obj_t *ui_pm_cursor;
lv_obj_t *ui_tvoc_arrow;

int _arrow_angle = 0;
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

  // Create the queue which will have <QueueElementSize> number of elements, each of size `message_t` and pass the address to <QueueHandle>.
  QueueHandle = xQueueCreate(QueueElementSize, sizeof(message_t));

  // Check if the queue was successfully created
  if (QueueHandle == NULL) {
    Serial.println("Queue could not be created. Halt.");
    while (1) delay(1000);  // Halt at this point as is not possible to continue
  }

  gui_mutex = xSemaphoreCreateMutex();
  if (gui_mutex == NULL) {
    // Handle semaphore creation failure
    Serial.println("semaphore creation failure");
    return;
  }

  xTaskCreatePinnedToCore(Task_LVGL,    // Pointer to the task entry function.
                          "Task_LVGL",  // A descriptive name for the task.
                          1024 * 10,    // The size of the task stack specified as the number of bytes
                          NULL,         // Pointer that will be used as the parameter for the task being created.
                          3,            // The priority at which the task should run.
                          NULL,         // Used to pass back a handle by which the created task can be referenced.
                          0);           // The core to which the task is pinned to, or tskNO_AFFINITY if the task has no core affinity.

  xTaskCreatePinnedToCore(Task_Screen_Update,
                          "Task_Screen_Update",
                          1024 * 3,
                          NULL,
                          2,
                          NULL,
                          1);

  // This task is only available when a sensor is connected.
  // If you do not have the Environmental sensor, use the task below.
  xTaskCreatePinnedToCore(Task_Sensor,
                          "Task_Sensor",
                          1024 * 3,
                          NULL,
                          1,
                          NULL,
                          1);

  // xTaskCreatePinnedToCore(Task_Dummy_Sensor,
  //                         "Task_Dummy_Sensor",
  //                         1024 * 2,
  //                         NULL,
  //                         1,
  //                         NULL,
  //                         1);
}

void loop() {}

void Task_Dummy_Sensor(void *pvParameters) {
  message_t message;
  while (1) {
    message.ambientTemperature = random(30, 33);
    message.ambientHumidity = random(30, 35);
    message.vocIndex = random(0, 150);
    message.massConcentrationPm2p5 = random(1, 50);

    int ret = xQueueSend(QueueHandle, (void *)&message, 0);
    if (ret == pdTRUE) {
      // The message was successfully sent.
      Serial.println("The message was successfully sent.");
    } else if (ret == errQUEUE_FULL) {
      // Since we are checking uxQueueSpacesAvailable this should not occur, however if more than one task should
      //   write into the same queue it can fill-up between the test and actual send attempt
      Serial.println("Task Dummy Sensor was unable to send data into the Queue");
    }  // Queue send check

    vTaskDelay(1000);
  }
}

void Task_Sensor(void *pvParameters) {

  Wire.begin(I2C_SDA, I2C_SCL);
  sen5x.begin(Wire);

  uint16_t error;
  char errorMessage[256];
  error = sen5x.deviceReset();
  if (error) {
    Serial.print("Error trying to execute deviceReset(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  float tempOffset = 0.0;
  error = sen5x.setTemperatureOffsetSimple(tempOffset);
  if (error) {
    Serial.print("Error trying to execute setTemperatureOffsetSimple(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.print("Temperature Offset set to ");
    Serial.print(tempOffset);
    Serial.println(" deg. Celsius (SEN54/SEN55 only");
  }

  // Start Measurement
  error = sen5x.startMeasurement();
  while (error) {
    Serial.print("Error trying to execute startMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    vTaskDelay(1000);
  }

  message_t message;

  while (1) {
    Serial.printf("\n[Task_Sensor] running on core: %d, Free stack space: %d\n", xPortGetCoreID(), uxTaskGetStackHighWaterMark(NULL));
    error = sen5x.readMeasuredValues(
      message.massConcentrationPm1p0, message.massConcentrationPm2p5, message.massConcentrationPm4p0,
      message.massConcentrationPm10p0, message.ambientHumidity, message.ambientTemperature, message.vocIndex,
      message.noxIndex);

    if (error) {
      Serial.print("[Task_Sensor] Error trying to execute readMeasuredValues(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
    } else {
      int ret = xQueueSend(QueueHandle, (void *)&message, 0);
      if (ret == pdTRUE) {
        // The message was successfully sent.
        Serial.println("[Task_Sensor] The message was successfully sent.");
      } else if (ret == errQUEUE_FULL) {
        // Since we are checking uxQueueSpacesAvailable this should not occur, however if more than one task should
        //   write into the same queue it can fill-up between the test and actual send attempt
        Serial.println("[Task_Sensor] Task Sensor was unable to send data into the Queue");
      }  // Queue send check
    }
    vTaskDelay(1000);
  }
}

void Task_LVGL(void *pvParameters) {
  // Initialize LVGL
  display_init();

  lv_init();

  lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf_1, NULL, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();

  // Main LVGL loop
  while (1) {
    unsigned int tickPeriod = millis() - lastTickMillis;
    lv_tick_inc(tickPeriod);
    lastTickMillis = millis();

    // Take the semaphore to access LVGL resources
    if (xSemaphoreTake(gui_mutex, portMAX_DELAY) == pdTRUE) {
      // Call LVGL's main task handler
      lv_timer_handler();

      // Release the semaphore after LVGL operations
      xSemaphoreGive(gui_mutex);
    }

    // Delay to control LVGL's refresh rate
    vTaskDelay(pdMS_TO_TICKS(1));  // Adjust as needed
  }
}

void Task_Screen_Update(void *pvParameters) {

  message_t message;
  char label_char[100];

  while (1) {
    Serial.printf("\n[Task_Screen_Update] running on core: %d, Free stack space: %d\n", xPortGetCoreID(), uxTaskGetStackHighWaterMark(NULL));
    if (QueueHandle != NULL) {  // Sanity check just to make sure the queue actually exists
      if (xQueueReceive(QueueHandle, &message, portMAX_DELAY)) {
        Serial.println("[Task_Screen_Update] The message was successfully received.");

        if (xSemaphoreTake(gui_mutex, portMAX_DELAY) == pdTRUE) {
          if (!isnan(message.ambientTemperature)) {
            if (ui_temp_label != NULL) lv_label_set_text_fmt(ui_temp_label, "%d", convertToF(message.ambientTemperature));
          }

          if (!isnan(message.ambientHumidity)) {
            if (ui_humidity_label != NULL) lv_label_set_text_fmt(ui_humidity_label, "%d", (int)message.ambientHumidity);
          }

          if (!isnan(message.massConcentrationPm2p5)) {
            if (ui_pm_label != NULL) lv_label_set_text_fmt(ui_pm_label, "%d", (int)message.massConcentrationPm2p5);
            set_pm_cursor((int)message.massConcentrationPm2p5);
          }

          if (!isnan(message.vocIndex)) {
            set_voc_arrow((int)message.vocIndex);
          }

          // Release the semaphore after LVGL operations
          xSemaphoreGive(gui_mutex);
        }
      } else {
        Serial.println("[Task_Screen_Update] It was unable to receive data from the Queue.");
      }
    }  // Sanity check
  }
}

void display_init() {
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
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
  screen = lv_obj_create(lv_screen_active());         // Get the active screen of the default display
  lv_obj_set_size(screen, TFT_HOR_RES, TFT_VER_RES);  // Set the size of lv object
  lv_obj_center(screen);                              // Set make it center
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
  ui_pm_label = lv_label_create(ui_pm_panel);
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
  ui_pm_cursor = lv_image_create(ui_pm_panel);
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
  ui_temp_label = lv_label_create(ui_temp_panel);
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

  ui_humidity_label = lv_label_create(ui_temp_panel);
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
  ui_tvoc_arrow = lv_image_create(ui_tvoc_panel);
  lv_image_set_src(ui_tvoc_arrow, &tvoc_arrow);
  lv_obj_align_to(ui_tvoc_arrow, ui_tvoc_label, LV_ALIGN_OUT_RIGHT_MID, 25, 0);

  Serial.println("UI DRAW Done");
}

void set_voc_arrow(int voc) {
  if (ui_tvoc_arrow == NULL) return;

  if (voc > 150) voc = 150;  // Set Maximum VOC

  int arrow_angle = 3600 - map(voc, 0, 150, 0, 1800);
  lv_anim_t anim_rotate_img;
  lv_anim_init(&anim_rotate_img);
  lv_anim_set_var(&anim_rotate_img, ui_tvoc_arrow);
  lv_anim_set_exec_cb(&anim_rotate_img, anim_rotate_cb);
  lv_anim_set_duration(&anim_rotate_img, 200);
  lv_anim_set_values(&anim_rotate_img, _arrow_angle, arrow_angle);
  lv_anim_start(&anim_rotate_img);
  _arrow_angle = arrow_angle;
}

void set_pm_cursor(int pm25) {
  if (ui_pm_cursor == NULL) return;

  if (pm25 < 12) {
    anim_pm_cursor(ui_pm_cursor, Green);
  } else if (pm25 >= 13 && pm25 <= 35) {
    anim_pm_cursor(ui_pm_cursor, Yellow);
  } else {
    anim_pm_cursor(ui_pm_cursor, Red);
  }
}

static void anim_rotate_cb(void *var, int32_t v) {
  lv_image_set_rotation((lv_obj_t *)var, v);
}

static void anim_y_cb(void *var, int32_t v) {
  lv_obj_set_y((lv_obj_t *)var, v);
}

static void anim_pm_cursor(lv_obj_t *target, PMStatus pmStatus) {

  if (target == NULL || pmStatus == _pmStatus) {
    return;
  }
  _pmStatus = pmStatus;

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, target);
  lv_anim_set_duration(&a, 500);
  lv_anim_set_path_cb(&a, lv_anim_path_overshoot);

  switch (pmStatus) {
    case Green:
      lv_anim_set_values(&a, lv_obj_get_y(target), 104);
      break;
    case Yellow:
      lv_anim_set_values(&a, lv_obj_get_y(target), 68);
      break;
    case Red:
      lv_anim_set_values(&a, lv_obj_get_y(target), 32);
      break;
  }

  lv_anim_set_exec_cb(&a, anim_y_cb);
  lv_anim_start(&a);
}

int convertToF(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}