// Pulls ulp_main.bin into the firmware image so the loader can access it.
extern "C" {
asm(
  ".section .rodata\n"
  ".global _binary_ulp_main_bin_start\n"
  ".global _binary_ulp_main_bin_end\n"
  "_binary_ulp_main_bin_start:\n"
  ".incbin \"ulp_assets/ulp_main.bin\"\n"
  "_binary_ulp_main_bin_end:\n"
);
}
