/*
 * wifi_scan.ino
 * Scan Wi-Fi networks combining and segregating 2.4GHz and 5GHz bands.
 *
 * This is a stub to satisfy the Arduino IDE. The source code is 
 * in the file main.cpp in the same directory.
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.3.6 or newer with the Boards Manager
 *  3- Select the XIAO ESP32C5 board
 *
 * Michel Deslierres
 * January 31, 2026
 *
 * From the original source:
 *
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 *  Source: https://github.com/espressif/arduino-esp32/blob/3.3.6/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
 *
 * Modifications:
 *  - Added Wi-Fi BSSID in the list of networks
 
 *  Licence
 *    The original example appears to have been released under the
 *    GNU LESSER GENERAL PUBLIC LICENSE 2.1
 *    see: https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md
 */

