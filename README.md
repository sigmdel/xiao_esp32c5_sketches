# XIAO ESP32C5 Sketches

*January 22, 2026*

**Arduino source code that accompanies [First Look at the Seeed Studio XIAO ESP32C5](https://sigmdel.ca/michel/ha/xiao/xiao_esp32c5_intro_en.html)**. 

---

**Table of Content**
<!-- TOC -->

- [1. Introduction](#1-introduction)
- [2. PlatformIO Notes](#2-platformio-notes)
- [3. Arduino IDE Notes](#3-arduino-ide-notes)
- [4. Sketches/Projects](#4-sketchesprojects)
  - [4.1. ./01_info](#41-01_info)
  - [4.2. ./02_digital_io](#42-02_digital_io)
  - [4.3. ./03_adc_in](#43-03_adc_in)
  - [4.4. ./04_sleep](#44-04_sleep)
  - [4.5. ./05_serial](#45-05_serial)
  - [4.6. ./06_wifi](#46-06_wifi)
  - [4.7. ./07_zibgee](#47-07_zibgee)
  - [4.8. ./08_bluetooth](#48-08_bluetooth)
- [5. Change Log](#5-change-log)
- [6. License](#6-license)

<!-- /TOC -->

## 1. Introduction

The [XIAO ESP32C5](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C5-p-6609.html) is a recent addition (Jan 16, 2026) to the [Seeed Studio XIAO series](https://www.seeedstudio.com/xiao-series-page) of diminutive development boards. As with the other boards in the XIAO ESP32Cx series, it is based on a RISC-V SoC.

[![](images/xiao_esp32c5_pinout.jpg)](images/xiao_esp32c5_pinout_legend.jpg)

## 2. PlatformIO/pioarduino Notes

All of the sketches in the repository have been successfully compiled with the [pioarduino-espressif32](https://github.com/pioarduino/platform-espressif32). The January 21, 2026 version of [pioarduino/platform-espressif Release 55.03.36 Arduino Release v3.3.6 based on ESP-IDF v5.5.2.260116](https://github.com/pioarduino/platform-espressif32/releases/tag/55.03.36) was used. 

The platform does not contain a board definition for the XIAO ESP32C5, nor could one be found elsewhere. Consequently, one was created and a private copy was added in this repository: [boards/seeed_xiao_esp32c5.json](boards/seeed_xiao_esp32c5.json). All the configuration files were updated to reflect the URL of the platform and the private boards directory to use. 

The latest version of the [piorduino IDE](https://github.com/pioarduino/pioarduino-vscode-ide), which is a fork of the PlatformIO IDE, can be installed as an extension in Visual Studio Code [VS&nbsp;Code](https://code.visualstudio.com/), [VSCODIUM](https://vscodium.com/), and perhaps other development environments. Since the extension is available in the respective Marketplace of both VS&nbsp;CODE and VSCODIUM, the installation only requires a few mouse clicks.

## 3. Arduino IDE Notes

The latest version of the Arduino IDE can be obtained for Windows, Linux and macOS in the [Downloads](https://www.arduino.cc/en/software) page from Arduino. 

Install the latest Espressif ESP32 Arduino core. 

 1. Add https://espressif.github.io/arduino-esp32/package_esp32_index.json in the Additional Boards Manager URLS in the Preferences window in the IDE.
  
 1. Install platform `esp32` by Espressif, version 3.3.5 or newer, with the Boards Manager. The only error in the esp32-arduino core that occurred in all the sketches has been fixed in version 3.3.6-RC1. However the sketches have not been run against this release candidate. There may be regression errors. Since the single error encountered in 3.3.5 does not impact the functioning of the sketches, it may be better to wait for the stable release of version 3.3.6.
 
 1. Select the `XIAO_ESP32C5` board in the `Tools` menu of the IDE when compiling a project.

Arduino sketches must have an `.ino` file name extension and must be contained in a directory that has the same name as the Arduino sketch (excluding the extension). Consequently the `01_pin_names` project contains a directory named `pin_names` that in turn contains the Arduino sketch `pin_names.ino`. That sketch is basically empty as it is a long comment only. This is not a problem because the Arduino IDE will import all source files found in the sketch directory. The actual code is in `main.cpp` which is the default name of a PlatformIO project.

## 4. Sketches/Projects

The sketches are grouped into categories. It should be easier to add or delete individual sketches with reduced impact on the numbering system.

### 4.1. ./01_info

Contains 3 sketches that print information to the serial monitor.

 | Project | Description |
 | :--- |  :--- |
 | *sys_info* | Display some information about the ESP32 system.|
 | *pin_names* | Display the mapping of XIAO ESP32C5 Arduino pin names to gpio numbers and some useful macros. |
 | *macs* | Prints out MAC addresses.  |

### 4.2. ./02_digital_io

Contains 4 sketches that verify the digital input and output capabilities of all the pins on the board. 

 | Project | Description |
 | :--- |  :--- |
 | *blink* | Blinks every pin around a XIAO ESP32C5 board in a heartbeat pattern. |
 | *pwm* | Pulses every pin around a XIAO ESP32C5 board using pulse width modulation.|
 | *poll* | Test polling of all the i/o pins of the XIAO ESP32C5.|
 | *interrupt* | Test the interrupt capability of all the i/o pins of the XIAO ESP32C5.|

### 4.3. ./03_adc_in

Contains 2 sketches that look at the analogue input capabilities of all the pins on the board.

 | Project | Description |
 | :--- |  :--- |
 | *adc* | Testing the analog input pins of the XIAO ESP32C5.|
 | *adc-dvm* | Measuring the A0 analog input of the XIAO ESP32C5.|

### 4.4. ./04_sleep

Contains 2 sketches that test 2 methods to wake the ESP32-C5 from a deep sleep state. 

 | Project | Description |
 | :--- |  :--- |
 | *deep_sleep_tmr* |  Wake XIAO ESP32C5 from deep sleep mode with timer.|
 | *deep_sleep_io* | Wake the XIAO ESP32C5 from deep sleep mode with a high or low signal to i/o pins.| 


### 4.5. ./05_serial

Contains 2 sketches with loopback tests of the UART and SPI peripherals.

 | Project | Description |
 | :--- |  :--- |
 | *uart* | UART loopback test.|
 | *spi* | SPI loopback test. |

### 4.6. ./06_wifi

Contains 3 sketches highlighting the 5 GH band capability of the ESP32-C5.

 | Project | Description |
 | :--- |  :--- |
 | *scan* | Scan Wi-Fi networks combining and segregating 2.4 GHz and 5 GHz bands.|
 | *wifi_connect* | Connect to 2.4 GHz or 5 GHz band Wi-Fi network with XIAO ESP32C5 using BSSID. |
 | *throughput* | Measure the Wi-Fi download speed on 2.4 GHz and 5 GHz bands of a XIAO ESP32C5. |
 
### 4.7. ./07_zibgee

Contains 2 slightly modified examples from the ESP32 Arduino Zigbee library.

 | Project | Description |
 | :--- |  :--- |
 | *Zigbee_On_Off_Switch* | Library example with adjustments for the XIAO ESP32C5 and XIAO ESP32C6. |
 | *Zigbee_On_Off_Light* | Library example with adjustments for the XIAO ESP32C5 and XIAO ESP32C6. |
 
### 4.8. ./08_bluetooth

Contains 3 slightly modified examples from the ESP32 Arduino BLE library.

 | Project | Description |
 | :--- |  :--- |
| *ble_scan* | Library example (Scan.ino) with adjustments for the XIAO ESP32C5 and XIAO ESP32C6. |
| *server* | Library example (Server.ino) with adjustments for the XIAO ESP32C5 and XIAO ESP32C6. |
| *client* |  Library example (Client.ino) with adjustments for the XIAO ESP32C5 and XIAO ESP32C6. |


## 5. Change Log

| Date | Change |
| :---  |  :--- | 
| 2026-01-22 | PlatformIO/pioarduino support and MACs changed into a private library |
| 2026-01-21 | Corrections and README update |
| 2026-01-20 | Sleep sketches |
| 2026-01-19 | Link to post and corrections to readmes|
| 2026-01-17 | Initial version |


## 6. License

Copyright 2026 Michel Deslierres. No rights reserved. 

While the copyright pertaining to included libraries must be respected, all the code by Michel Deslierres in this repository is in the public domain. In those jurisdictions where this may be a problem, the [BSD Zero Clause License](https://spdx.org/licenses/0BSD.html) applies.
