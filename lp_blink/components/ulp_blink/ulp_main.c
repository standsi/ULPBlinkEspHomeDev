// LP-core program for esp32c6: generate one pulse, then return.
// Target: ESP-IDF v5.5.3.

#include <stdint.h>

#include "ulp_lp_core.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"

// Shared variable in LP memory for:
//  flash LP IO pin number.
//  pulse width in microseconds.
//  flash LP IO inversion flag.
//  run count (for debugging).
volatile uint32_t flash_lp_io;
volatile uint32_t pulse_width_us;
volatile bool flash_lp_io_inverted;
volatile uint32_t run_count;

int main(void)
{
    run_count++;

    ulp_lp_core_gpio_init(flash_lp_io);
    ulp_lp_core_gpio_output_enable(flash_lp_io);
    ulp_lp_core_gpio_input_disable(flash_lp_io);

    ulp_lp_core_gpio_set_level(flash_lp_io, flash_lp_io_inverted ? 0 : 1);
    ulp_lp_core_delay_us(pulse_width_us);
    ulp_lp_core_gpio_set_level(flash_lp_io, flash_lp_io_inverted ? 1 : 0);

    // The LP core halts automatically when main returns.
    return 0;
}

