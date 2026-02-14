/*
 * ble_switch.ino
 * Create a Bluetooth server that will send notification of a change in an on/off button
 * setting to a connected Bluetooth client
 *
 * This is a stub to satisfy the Arduino IDE. The source code is 
 * in the file main.cpp in the same directory.
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.3.6 or newer with the Boards Manager
 *  3- Select the XIAO_ESP32C5 board
 *
 * Michel Deslierres
 * February 10, 2026
 *
 * Based on esp32/3.3.6/libraries/BLE/examples/Notify/Notify.ino
 * Attibution in that file:
 * 
 *     Video: https://www.youtube.com/watch?v=oCMOYS71NIU
 *     Based on Neil Kolban example for IDF: 
 *        @ https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
 *     Ported to Arduino ESP32 by Evandro Copercini
 *     updated by chegewara
 * 
 *     Create a BLE server that, once we receive a connection, will send periodic notifications.
 */
