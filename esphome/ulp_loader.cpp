#include "ulp_loader.h"

#include "esphome/core/log.h"

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp_lp_core.h"

#include "ulp_main_shared.h"

static const char *const TAG = "ulp_blink";

ULPBlinkComponent *g_ulp_blink_component = nullptr;

// Symbols from embed_ulp.cpp (binary data)
extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

bool ULPBlinkComponent::start_lp_core_() {
  gpio_num_t gpio_num = static_cast<gpio_num_t>(gpio_num_);

  if (!rtc_gpio_is_valid_gpio(gpio_num)) {
    ESP_LOGE(TAG, "GPIO %u is not LP/RTC capable", gpio_num_);
    return false;
  }

  if (running_) {
    ulp_lp_core_stop();
    running_ = false;
  }

  size_t ulp_size = (size_t)(ulp_main_bin_end - ulp_main_bin_start);

  esp_err_t err = ulp_lp_core_load_binary(ulp_main_bin_start, ulp_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to load LP binary: %d", err);
    return false;
  }

  ulp_main_shared::flash_lp_io() = static_cast<uint32_t>(rtc_io_number_get(gpio_num));
  ulp_main_shared::pulse_width_us() = pulse_width_us_;
  ulp_main_shared::run_count() = 0;

  ulp_lp_core_cfg_t cfg = {
      .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
      .lp_timer_sleep_duration_us = wakeup_period_us_,
  };

  ESP_LOGI(TAG, "Starting LP core: gpio=%u pulse=%u us period=%u us", gpio_num_, pulse_width_us_, wakeup_period_us_);
  err = ulp_lp_core_run(&cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start LP core: %d", err);
    return false;
  }

  running_ = true;
  ESP_LOGI(TAG, "LP core running (pulse+halt per wake)");
  return true;
}

void ULPBlinkComponent::setup() {
  g_ulp_blink_component = this;
  start_lp_core_();
}

void ULPBlinkComponent::set_pulse_width_us(uint32_t pulse_width_us_value) {
  pulse_width_us_ = pulse_width_us_value;

  if (running_) {
    ulp_main_shared::pulse_width_us() = pulse_width_us_;
  }

  ESP_LOGI(TAG, "Updated LP pulse width to %u us", pulse_width_us_);
}

void ULPBlinkComponent::set_wakeup_period_ms(uint32_t wakeup_period_ms) {
  wakeup_period_us_ = wakeup_period_ms * 1000U;
  ESP_LOGI(TAG, "Updating LP wake period to %u ms", wakeup_period_ms);

  start_lp_core_();
}
