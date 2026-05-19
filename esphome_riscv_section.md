# ESPHome configuration for esp32s2 and s3 riscv core

This document describes the current ESPHome-side integration for the working
`riscv_blink` ulp binary. The setup now uses a local external component,
which is compatible with current ESPHome releases and avoids the removed
`custom_component` path.

This configuration has been verified to compile, upload, and run on-device with
the YAML defaults of a 30 ms pulse every 1000 ms.

## Build and refresh the risc v binary for both platforms

1. Build the LP project in `riscv_blink/` for s2:

```bash
idf.py set-target esp32s2
idf.py build
```
2. Run the following powershell script to copy the generated risc-v binary to the s2 asset file in the esphome component, including the shared RTC memory addresses (with the main repo dir as cur dir):
```
pwsh ./esphome/update_riscv_ulp_artifacts.ps1 -Target esp32s2
```
(Note, the raw .bin file will just be processed here, not copied like with lp_blink)

3. Just for tracking, copy the assembly .S file over even though it is not used.
```powershell
Copy-Item riscv_blink/build/ulp_main.bin.S esphome\my_components\riscv_blink\ulp_main_esp32s2.bin.S -Force
```

4. Build the LP project in `riscv_blink/` for s3:

```bash
idf.py set-target esp32s3
idf.py build
```
2. Run the following powershell script to copy the generated risc-v binary to the s3 asset file in the esphome component, including the shared RTC memory addresses (with the main repo dir as cur dir):
```
pwsh ./esphome/update_riscv_ulp_artifacts.ps1 -Target esp32s3
```
(Note, the raw .bin file will just be processed here, not copied like with lp_blink)

3. Just for tracking, copy the assembly .S file over even though it is not used.
```powershell
Copy-Item riscv_blink/build/ulp_main.bin.S esphome\my_components\riscv_blink\ulp_main_esp32s3.bin.S -Force
```


## ESPHome file layout

TBD

The current working build does not rely on `ulp_main.bin.S`. That file can be
kept as a reference artifact from the LP build, but the external component now
loads the binary from the asset files, `esphome\my_components\riscv_blink\ulp_artifacts_esp32s2.h` and `esphome\my_components\riscv_blink\ulp_artifacts_esp32s3.h`.

## Build notes

TBD

## YAML template

Adjust `ssid`, `password`, and `board` for your device. Same for S3 substituted for S2.

```yaml
substitutions:
  riscv_blink_pin: "4"

external_components:
  - source:
      type: local
      path: my_components

esphome:
  name: esp32s2_riscv_blink

esp32:
  variant: esp32s2
  board: esp32-s2-saola-1
  framework:
    type: esp-idf

logger:
  level: DEBUG

wifi:
  ssid: "your_ssid"
  password: "your_password"

riscv_blink:
  id: riscv_blink_component
  gpio_num: ${riscv_blink_pin}
  init_state: last
  pulse_width_us: 30000
  wake_period_ms: 1000

number:
  - platform: template
    name: "ULP Pulse Width (us)"
    id: ulp_pulse_width_us
    min_value: 1000
    max_value: 100000
    step: 1000
    optimistic: true
    initial_value: 30000
    set_action:
      lambda: |-
        id(riscv_blink_component).set_pulse_width_us((uint32_t) x);

  - platform: template
    name: "ULP Wake Period (ms)"
    id: ulp_wake_period_ms
    min_value: 100
    max_value: 5000
    step: 100
    optimistic: true
    initial_value: 1000
    set_action:
      lambda: |-
        id(riscv_blink_component).set_wakeup_period_ms((uint32_t) x);

switch:
  - platform: template
    name: "ULP Blink Enabled"
    lambda: |-
      return id(riscv_blink_component).is_running();
    turn_on_action:
      lambda: |-
        id(riscv_blink_component).start_blink();
    turn_off_action:
      lambda: |-
        id(riscv_blink_component).stop_blink();
```

## Component behavior

TBD

If `init_state: last` is selected and no valid saved RTC state exists yet, the component falls back to `stopped`.

## Notes

TBD