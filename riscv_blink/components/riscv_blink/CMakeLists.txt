idf_component_register(
    SRCS "riscv_blink_runtime.c"
    INCLUDE_DIRS "."
    REQUIRES ulp esp_driver_gpio
)

set(ulp_app_name "ulp_main")
set(ulp_riscv_sources "ulp_main.c")
set(ulp_exp_dep_srcs "riscv_blink_runtime.c")

ulp_embed_binary(${ulp_app_name} "${ulp_riscv_sources}" "${ulp_exp_dep_srcs}" TYPE riscv)