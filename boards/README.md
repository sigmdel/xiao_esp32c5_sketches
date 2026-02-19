# pioarduino Board Definition for the Seeed Studio XIAO ESP32C5

The latest stable (55.03.36) branch of the [pioarduino](https://github.com/pioarduino/platform-espressif32) platform does not contain a board definition for the XIAO ESP32C5. This directory contains the current (2026-02-02) board definition file from the Seeed Studio [platform-seeedboards](https://github.com/Seeed-Studio/platform-seeedboards) repository named [seeed-xiao-esp32-c5.json](https://github.com/Seeed-Studio/platform-seeedboards/blob/main/boards/seeed-xiao-esp32-c5.json). Many thanks for the speedy [confirmation from StarSphere-1024](https://github.com/Seeed-Studio/platform-seeedboards/issues/32#issuecomment-3839244558).

The [seeed_xiao_esp32c5.json](seeed_xiao_esp32c5.json) definition in this directory is renamed to match the naming convention of the other Seeed Studio XIAO boards in [platform-espressif32/boards] at [pioarduino](https://github.com/pioarduino/platform-espressif32/tree/main/boards) and [platformio](https://github.com/platformio/platform-espressif32/tree/develop/boards). It also contains some [modifications as suggested by the piarduino team](https://github.com/pioarduino/platform-espressif32/pull/393) notably to ensure compatibility with the ESP-IDF framework.

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

In the Arduino IDE Tools menu, PSRAM is disabled by default. To disable PSRAM in a PlatformIO project, add a [-U flag](https://docs.platformio.org/en/latest/projectconf/sections/env/options/build/build_flags.html) as shown here. 

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
build_flags = -UBOARD_HAS_PSRAM
framework = arduino
monitor_speed = 460800   
```

While on this topic, another build flag can be used to set the core debug level as done in the Tools menu of the Arduino IDE.

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
build_flags =
  -UBOARD_HAS_PSRAM
  -DCORE_DEBUG_LEVEL=4 ; options: 5 verbose, 4 debug, 3 info, 2 error, 1 critical, 0 none (default)
framework = arduino
monitor_speed = 460800   
```
