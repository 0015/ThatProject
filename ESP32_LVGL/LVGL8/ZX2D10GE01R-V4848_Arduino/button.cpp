#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#ifndef CONFIG_IDF_TARGET_ESP32C3
#include "freertos/xtensa_api.h"
#endif
#include "freertos/portmacro.h"
#include "rom/gpio.h"
#include "driver/gpio.h"
#include "button.hpp"
#include "esp_log.h"

static QueueHandle_t btn_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  xQueueSendFromISR(btn_queue, &arg, NULL);
}

static void button_update_task(void* arg) {
  button_t* btn = NULL;
  for (;;) {
    xQueueReceive(btn_queue, &btn, portMAX_DELAY);
    uint32_t time = xTaskGetTickCount();
    if (time - btn->last_press_time < btn->filter_time) {
      continue;
    }
    if (time - btn->last_release_time < btn->filter_time) {
      continue;
    }
    vTaskDelay(pdMS_TO_TICKS(btn->filter_time));
    if (gpio_get_level(gpio_num_t(btn->pin_io)) == btn->pressed_value) {
      btn->pressed = 1;
      btn->last_press_time = time;
    } else {
      btn->released = 1;
      btn->last_release_time = time;
    }
  }
}

uint8_t button_isPressed(button_t* button) {
  return gpio_get_level(gpio_num_t(button->pin_io)) == button->pressed_value;
}

uint8_t button_isRelease(button_t* button) {
  return !(gpio_get_level(gpio_num_t(button->pin_io)) == button->pressed_value);
}

uint8_t button_wasPressed(button_t* button) {
  uint32_t time = xTaskGetTickCount();
  uint8_t pressed = button->pressed;
  button->pressed = 0;
  return pressed && ((time - button->last_press_time) < 300);
}

uint8_t button_wasPressFor(button_t* button, uint32_t press_time) {
  uint32_t time_now = xTaskGetTickCount();
  if (button->pressed && ((time_now - button->last_press_time) > press_time) && button->last_press_time > button->last_release_time) {
    button->pressed = 0;
    return 1;
  }
  return 0;
}

uint8_t button_wasRelease(button_t* button) {
  uint32_t time = xTaskGetTickCount();
  uint8_t released = button->released;
  button->released = 0;
  return released && ((time - button->last_release_time) < 300);
}

button_t* button_attch(uint16_t gpio_pin, uint8_t press_value, uint16_t filter_time) {
  if (btn_queue == NULL) {
    btn_queue = xQueueCreate(10, sizeof(button_t*));
    xTaskCreatePinnedToCore(button_update_task, "btn", 2 * 1024, NULL, 4, NULL, 1);
    gpio_install_isr_service(0);
  }

  button_t* button = (button_t*)calloc(1, sizeof(button_t));
  button->pin_io = gpio_pin;
  button->pressed_value = press_value;
  button->filter_time = filter_time;

  gpio_pad_select_gpio(gpio_num_t(button->pin_io));
  gpio_set_direction(gpio_num_t(button->pin_io), GPIO_MODE_INPUT);

  gpio_set_intr_type(gpio_num_t(button->pin_io), GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add(gpio_num_t(button->pin_io), gpio_isr_handler, button);

  return button;
}
