# Development repo for modifying low power blink routines 

### This repo has ESP-IDF projects for ESP32C6 low power core and ESP32S2/S3 risc v low power core blink programs.  It also contains an ESPHome staging directory for the modified binary files if the lp core programs are modified.  Using this repo should ONLY BE NEEDED if you want to change the actual program that runs in the low power cores.  To change the ESPHome code in the components you can just clone and alter the component code from the repo below.

This dev repo was used to create the esphome components found in the repo:

https://github.com/standsi/esphome-external-components

### Prerequisites
* [Install ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v6.0.1/esp32/)
* Clone this repo and open the workspace file.
* ESP IDF should start, choose the specific component directory to develop (lp_blink or riscv_blink) and follow the instructions in the getting started section of the ESP IDF documentation.
* NOTE that you may need to delete the .vscode/settings.json file to let the ESP-IDF set the proper paths.

### Development process to change the lp core programs
The lp/riscv core programs are written in c in the `lp_blink/components/ulp_blink/ulp_main.c` and `riscv_blink/components/riscv_blink/ulp_main.c` files.  They are very simple: setup the gpio pin number passed in through shared memory as an output, set the pin to the active state (high or low based on the invert parameter passed in), wait for time based on the pulse width parameter passed in, then set the pin to the inactive state.  The lp core then goes dormant, waking back up for the next pulse when the config value of the wakeup period used in the component when the core was configured.

Changing the lp core program requires:
* Modifying the file, setting the chip build target, building the program, and flashing the chip to test with a simple stub program `main/main.c`.
* Once the new lp core program is running, the binary artifact from the build needs to be copied into the esphome components under `esphome/my_components/lp_blink` or `esphome/my_components/riscv_blink`. If you added shared memory values those relative offsets will also need to be added.  Instructions for doing this step are in `esphome_lp_section.md` and `esphome_riscv_section.md` (which also includes the use of a powershell script to simplify the process because of the two potential targets.  *A Bash script equivalent would be welcome!*).
* Once this is completed the modified folder(s) under `my_components` can be used in place of the same components from the `esphome-external-components` repo.

The `esphome` directory in this repo is just a staging directory to provide a location for building the modified components.  It is not setup to run directly under the ESPHome runtime; thus the .yaml files are for illustration only.  See the README-dev.md file in the component folder of the main `esphome-external-components` repo for tips on esphome component development.

