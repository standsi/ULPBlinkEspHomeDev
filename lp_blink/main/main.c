#include <stdint.h>

#include "driver/gpio.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lp_blink_runtime.h"

static const char *TAG = "lp_blink";

void app_main(void)
{
    const gpio_num_t blink_gpio = GPIO_NUM_5;
    const uint32_t pulse_width_us = 8000;
    const uint32_t period_us = 800000;
    const bool io_inverted = false;

    ESP_LOGI(TAG, "Starting LP core on GPIO %d", blink_gpio);
    ESP_ERROR_CHECK(lp_blink_start(blink_gpio, pulse_width_us, period_us, io_inverted));

    ESP_LOGI(TAG, "Entering deep sleep; LP core will keep pulsing GPIO %d", blink_gpio);
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_deep_sleep_start();
}

