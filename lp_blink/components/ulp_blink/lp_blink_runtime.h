#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "driver/gpio.h"

esp_err_t lp_blink_start(gpio_num_t gpio_num, uint32_t pulse_width_us, uint32_t period_us, bool io_inverted);
uint32_t lp_blink_get_run_count(void);