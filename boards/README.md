# Seeed Studio XIAO ESP3256 Board Definition in PlatformIO/pioarduino 

The latest stable (55.03.36) branch of the [pioarduino](https://github.com/pioarduino/platform-espressif32) platform does not contain a board definition for the XIAO ESP32C5. This directory contains a proposed definition `seeed_xiao_esp32c5.json`. <!-- which has been successfully tested with all the projects in the repository-->

To use the [boards/seeed_xiao_esp32c6.json](seeed_xiao_esp32c6.json) definition in a PlatformIO project, add the following line in the project configuration file (`platformio.ini`): `boards_dir = ../boards`. Here is a typical example.

```ini
[platformio]
boards_dir = ../boards

[env:seeed_xiao_esp32c5]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/51.03.07/platform-espressif32.zip
board = seeed_xiao_esp32c5
framework = arduino
...
```


## Credit

This board definition is a mash-up of two existing definitions: 

  `~/.platformio/platforms/espressif32/boards/seeed_xiao_esp32c6.json`

  `~/.platformio/platforms/espressif32/boards/esp32-c5-devkitc1-n8r4.json`
