#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/pcnt.h"

#define PCNT_H_LIM_VAL INT16_MAX
#define PCNT_L_LIM_VAL INT16_MIN

static pcnt_unit_t unit = PCNT_UNIT_0;

void mt8901_init(int16_t sig_pin, int16_t dir_pin) {
  /* Prepare configuration for the PCNT unit */
  pcnt_config_t pcnt_config;
  pcnt_config.pulse_gpio_num = sig_pin;
  pcnt_config.ctrl_gpio_num = dir_pin;
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.unit = unit;
  pcnt_config.pos_mode = PCNT_COUNT_DEC;
  pcnt_config.neg_mode = PCNT_COUNT_INC;
  pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;
  pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;

  /* Initialize PCNT unit */
  pcnt_unit_config(&pcnt_config);

  /* Configure and enable the input filter */
  pcnt_set_filter_value(unit, 1000);
  pcnt_filter_enable(unit);

  /* Initialize PCNT's counter */
  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);

  /* Everything is set up, now go to counting */
  pcnt_counter_resume(unit);
}

int16_t mt8901_get_count() {
  int16_t count = 0;
  pcnt_get_counter_value(unit, &count);
  return count;
}
