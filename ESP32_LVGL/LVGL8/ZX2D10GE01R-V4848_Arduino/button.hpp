#pragma once

#include "stdint.h"

typedef struct {
  uint32_t pin_io;
  uint8_t pressed_value;
  uint8_t pressed;
  uint8_t released;
  uint32_t last_press_time;
  uint32_t last_release_time;
  uint16_t filter_time;
} button_t;

uint8_t button_isPressed(button_t* button);
uint8_t button_isRelease(button_t* button);
uint8_t button_wasPressed(button_t* button);
uint8_t button_wasRelease(button_t* button);
uint8_t button_wasPressFor(button_t* button, uint32_t press_time);
button_t* button_attch(uint16_t gpio_pin, uint8_t press_value, uint16_t filter_time);
