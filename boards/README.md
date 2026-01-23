# Proposed PlatformIO/pioarduino Board Definition for the Seeed Studio XIAO ESP32C5

The latest stable (55.03.36) branch of the [pioarduino](https://github.com/pioarduino/platform-espressif32) platform does not contain a board definition for the XIAO ESP32C5. This directory contains a proposed definition, [seeed_xiao_esp32c5.json](seeed_xiao_esp32c5.json), which has been successfully tested with all the projects in the repository.

To use this board definition in a PlatformIO project, add the following line in the `[platformio]` section of the project configuration file: 
```
[platformio]
boards_dir = <relative_path_of_boards_dir>
...
```
Relative here is with respect to the directory containing the `platformio.ini` configuration file. Here is a typical example taken from a sketch in the repository.

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = blink
boards_dir = ../../boards   ;; .../xiao_esp32c5_sketches/boards/
lib_dir = ../../libraries   ;; .../xiao_esp32c5_sketches/libraries/                     
                            ;; .../xiao_esp32c5_sketches/02_digital_io/01_blink/platformio.ini
[env:seeed_xiao_esp32c5]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = seeed_xiao_esp32c5
framework = arduino
monitor_speed = 460800   
```

## Sources

The proposed board definition is a blend of two existing definitions: 

  1.  [seeed-xiao-esp32-c5.json](https://github.com/Seeed-Studio/platform-seeedboards/blob/main/boards/seeed-xiao-esp32-c5.json) from Seeed Studio.

  2.  [esp32-c5-devkitc1-n8r4.json](https://github.com/pioarduino/platform-espressif32/blob/main/boards/esp32-c5-devkitc1-n8r4.json) presumably from Espressif.

These two definitions are also in the directory, to make it easy to compare them. That comparison revealed discrepancies where I would think that the Espressif definition is correct. These differences are about the `flash_mode` and the `maximum_size`. The hardware ids (`hwids`), which are the USB vendor:product values, do not correspond to the observed value. 

Hopefully, the proposed [seeed_xiao_esp32c5.json](seeed_xiao_esp32c5.json) is correct.
