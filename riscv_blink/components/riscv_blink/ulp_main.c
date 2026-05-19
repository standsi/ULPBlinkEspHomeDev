#include <stdint.h>

#include "ulp_riscv.h"
#include "ulp_riscv_gpio.h"
#include "ulp_riscv_utils.h"

volatile uint32_t flash_lp_io;
volatile uint32_t pulse_width_us;
volatile uint32_t flash_lp_io_inverted;
volatile uint32_t run_count;

static void delay_us_(uint32_t delay_us)
{
    ulp_riscv_delay_cycles(delay_us * ULP_RISCV_CYCLES_PER_US);
}

int main(void)
{
    const uint8_t active_level = flash_lp_io_inverted ? 0U : 1U;
    const uint8_t inactive_level = flash_lp_io_inverted ? 1U : 0U;

    run_count++;

    ulp_riscv_gpio_init((gpio_num_t) flash_lp_io);
    ulp_riscv_gpio_output_enable((gpio_num_t) flash_lp_io);
    ulp_riscv_gpio_input_disable((gpio_num_t) flash_lp_io);
    ulp_riscv_gpio_output_level((gpio_num_t) flash_lp_io, active_level);
    delay_us_(pulse_width_us);
    ulp_riscv_gpio_output_level((gpio_num_t) flash_lp_io, inactive_level);

    return 0;
}