#pragma once

#include "stdint.h"

void mt8901_init(int16_t sig_pin, int16_t dir_pin);

int16_t mt8901_get_count();
