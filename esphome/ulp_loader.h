#pragma once

#include <cstdint>

#include "esphome/core/component.h"

class ULPBlinkComponent : public esphome::Component {
 public:
  explicit ULPBlinkComponent(uint8_t gpio_num) : gpio_num_(gpio_num) {}

  void setup() override;
  void loop() override {}

  void set_pulse_width_us(uint32_t pulse_width_us);
  void set_wakeup_period_ms(uint32_t wakeup_period_ms);

 private:
  bool start_lp_core_();

  uint8_t gpio_num_;
  uint32_t pulse_width_us_{30000};
  uint32_t wakeup_period_us_{1000000};
  bool running_{false};
};

extern ULPBlinkComponent *g_ulp_blink_component;
