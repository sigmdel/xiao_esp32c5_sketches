# pioarduino Board Definition for the Seeed Studio XIAO ESP32C5

*February 19, 2026*

**Table of Content**
<!-- TOC -->

- [1. Board Definition](#1-board-definition)
  - [1.1. Stable release platform with local board definition](#11-stable-release-platform-with-local-board-definition)
  - [1.2. Development release](#12-development-release)
- [2. PSRAM](#2-psram)
- [3. Enabling Core Debugging Messages](#3-enabling-core-debugging-messages)

<!-- /TOC -->
---

## 1. Board Definition
While the latest stable release (55.03.37) of the [pioarduino espressif32 platform](https://github.com/pioarduino/platform-espressif32) does not contain a board definition for the XIAO ESP32C5, there is one in the development branch  named [seeed_xiao_esp32c5.json](https://github.com/pioarduino/platform-espressif32/tree/develop/boards). This board definition will be added to a future stable release of `platform-espressif32` from pioarduino.

In the meantime, there are two possible ways to work with the board in the pioarduinoIDE. 

> In principle, there could be a third way using the Seeed-Studio platform (`platform-seeedboards`) instead of the pioarduino `platform-espressif32` in either the pioarduinoIDE or PlatformioIDE extensions. However it was somewhat difficult, but not impossible, to install `platform-seeedboards` in the pioarduinoIDE extension in VSCodium. Be aware that there are some problems with the `seeed-xiao-esp32-c5.json` manifest in the Seeed platform. These will probably have no consequences for the sketches in this repository. Also, the Seeed platform contains only a subset of the ESP32 boards supported in the pioarduino platform.

### 1.1. Stable release platform with local board definition 

The approach adopted in this repository is to use the stable release of `platform-espressif32` along with a local copy of the board definition. To use a local board definition in a pio project, add the following line in the `[platformio]` section of the project configuration file: 

```ini
[platformio]
boards_dir = <relative_path_of_directory_with_boards.json_manifests>
...
```

Relative here is with respect to the directory containing the `platformio.ini` configuration file. Here is a typical example taken from a sketch in the repository.

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = blink             ;; .../xiao_esp32c5_sketches/02_digital_io/01_blink/platformio.ini - path to this file
boards_dir = ../../boards   ;; .../xiao_esp32c5_sketches/boards/ 
lib_dir = ../../libraries   ;; .../xiao_esp32c5_sketches/libraries/                     
                            
[env:seeed_xiao_esp32c5]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = seeed_xiao_esp32c5
framework = arduino
monitor_speed = 460800   
```

### 1.2. Development release 

As explained in the [platform-espressif32 README](https://github.com/pioarduino/platform-espressif32#development-arduino)
use the `develop` branch of the platform.

```ini
[env:seeed_xiao_esp32c5_dev]
platform = https://github.com/pioarduino/platform-espressif32.git#develop
board = seeed_xiao_esp32c5
framework = arduino
monitor_speed = 460800   
...
```

This is certainly simpler if one doesn't mind living on the edge. There's probably not much risk when working with the simple projects in this repository.

## 2. PSRAM

In the Arduino IDE **Tools** menu, PSRAM is disabled by default. To disable PSRAM in the `platformio.ini` configuration file of a project, add a [-U build flag](https://docs.platformio.org/en/latest/projectconf/sections/env/options/build/build_flags.html) as shown here. 

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = blink
boards_dir = ../../boards
lib_dir = ../../libraries
                            
[env:seeed_xiao_esp32c5]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = seeed_xiao_esp32c5
build_flags = -UBOARD_HAS_PSRAM
framework = arduino
monitor_speed = 460800   
```

## 3. Core Debugging Messages

While on this topic, another build flag can be used to set the core debug level as done in the **Tools** menu of the Arduino IDE.

```ini
[platformio]
; Make the Arduino IDE happy (.INO file must be in a directory of the same name)
src_dir = blink
boards_dir = ../../boards
lib_dir = ../../libraries

[env:seeed_xiao_esp32c5]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = seeed_xiao_esp32c5
build_flags =
  -UBOARD_HAS_PSRAM
  -DCORE_DEBUG_LEVEL=4 ; options: 5 verbose, 4 debug, 3 info, 2 error, 1 critical, 0 none (default)
framework = arduino
monitor_speed = 460800   
```
