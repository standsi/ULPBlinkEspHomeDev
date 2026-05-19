#pragma once

#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"

esp_err_t riscv_blink_start(gpio_num_t gpio_num, uint32_t pulse_width_us, uint32_t period_us, bool io_inverted);
void riscv_blink_stop(void);
uint32_t riscv_blink_get_run_count(void);