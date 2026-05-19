#include <stdint.h>

#include "driver/rtc_io.h"
#include "esp_check.h"
#include "ulp_main.h"
#include "ulp_riscv.h"

#include "riscv_blink_runtime.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");

static const char *const TAG = "riscv_blink";

static void prepare_rtc_gpio_(gpio_num_t gpio_num)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_gpio_deinit(gpio_num));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_gpio_init(gpio_num));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_gpio_pulldown_dis(gpio_num));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_gpio_pullup_dis(gpio_num));
}

esp_err_t riscv_blink_start(gpio_num_t gpio_num, uint32_t pulse_width_us, uint32_t period_us, bool io_inverted)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, TAG,
                        "GPIO %d is not RTC capable", gpio_num);

    prepare_rtc_gpio_(gpio_num);

    ulp_riscv_timer_stop();
    ulp_riscv_halt();
    ulp_riscv_reset();

    ESP_RETURN_ON_ERROR(
        ulp_riscv_load_binary(ulp_main_bin_start, ulp_main_bin_end - ulp_main_bin_start),
        TAG,
        "failed to load ULP RISC-V binary");

    ulp_flash_lp_io = (uint32_t) gpio_num;
    ulp_pulse_width_us = pulse_width_us;
    ulp_flash_lp_io_inverted = io_inverted ? 1U : 0U;
    ulp_run_count = 0;

    ESP_RETURN_ON_ERROR(ulp_set_wakeup_period(0, period_us), TAG, "failed to set wakeup period");

    return ulp_riscv_run();
}

void riscv_blink_stop(void)
{
    ulp_riscv_timer_stop();
    ulp_riscv_halt();
    ulp_riscv_reset();
}

uint32_t riscv_blink_get_run_count(void)
{
    return ulp_run_count;
}