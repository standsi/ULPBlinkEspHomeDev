idf_component_register(
    SRCS "lp_blink_runtime.c"
    INCLUDE_DIRS "."
    REQUIRES ulp esp_driver_uart
)

set(ulp_app_name "ulp_main")
set(ulp_sources "ulp_main.c")
set(ulp_exp_dep_srcs "lp_blink_runtime.c")

ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
