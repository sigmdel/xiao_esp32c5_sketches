/*
 * client.ino
 * Create a Bluetooth LE client device that connects to a BLE server and then retrieves 
 * the current characteristic value. It will then periodically update the value of the 
 * characteristic on the remote server with the current time since boot.
 *
 * This is a stub to satisfy the Arduino IDE. The source code is 
 * in the file main.cpp in the same directory.
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.3.4 or newer with the Boards Manager
 *  3- Select the XIAO ESP32C5 board
 *
 * Michel Deslierres
 * January 12, 2026
 *
 * Based on BLE Client Program in Getting Started with Seeed Stuido XIAO ESP32-C3
 * @https://wiki.seeedstudio.com/xiao_esp32c5_bluetooth_usage/
 * 
 * Based on the ../libraries/BLE/examples/Scan/Scan.ino example 
 * included in the arduino-esp32 GitHub repository
 * @https://github.com/espressif/arduino-esp32 
 * 
 * Attribution in that file
 * 
 *   A BLE client example that is rich in capabilities.
 *   There is a lot new capabilities implemented.
 *   author unknown
 *   updated by chegewara
 */


 
