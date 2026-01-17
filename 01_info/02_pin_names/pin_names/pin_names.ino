/*
 * pin_names.ino
 * Display the mapping of XIAO ESP32C5 Arduino pin names to gpio numbers and some useful macros 
 *
 * The XIAO ESP32C5 pin definitions are found in 
 *   https://github.com/espressif/arduino-esp32/blob/master/variants/XIAO_ESP32C5/pins_arduino.h
 *
 * The BOOT_PIN definition for all ESP32s found in
 *  https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal.h
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.0.1 or newer with the Boards Manager
 *  3- Select the XIAO_ESP32C5 board
 *
 * Michel Deslierres
 * Jan 10, 2026
 *
 * Copyright 2026, Michel Deslierres. No rights reserved, this code is in the public domain.
 * In those jurisdictions where this may be a problem, the BSD Zero Clause License applies.
 * <https://spdx.org/licenses/0BSD.html>
 */
// SPDX-License-Identifier: 0BSD



