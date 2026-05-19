#include <stdint.h>

#include "esp_check.h"
#include "driver/rtc_io.h"
#include "ulp_lp_core.h"
#include "ulp_main.h"

#include "lp_blink_runtime.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");

esp_err_t lp_blink_start(gpio_num_t gpio_num, uint32_t pulse_width_us, uint32_t period_us, bool io_inverted)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, "lp_blink",
                        "GPIO %d is not LP/RTC capable", gpio_num);

    ESP_RETURN_ON_ERROR(
        ulp_lp_core_load_binary(ulp_main_bin_start, ulp_main_bin_end - ulp_main_bin_start),
        "lp_blink",
        "failed to load LP-core binary");

    ulp_flash_lp_io = (uint32_t)rtc_io_number_get(gpio_num);
    ulp_pulse_width_us = pulse_width_us;
    ulp_flash_lp_io_inverted = io_inverted;
    ulp_run_count = 0;

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = period_us,
    };

    return ulp_lp_core_run(&cfg);
}

uint32_t lp_blink_get_run_count(void)
{
    return ulp_run_count;
}