/*
 * wifi_scan.ino
 * This sketch demonstrates how to scan WiFi networks. For chips that 
 * support 5GHz band, separate scans are done for all bands. The API is 
 * based on the Arduino WiFi Shield library, but has significant 
 * changes as newer WiFi functions are supported. E.g. the return value 
 * of `encryptionType()` different because more modern encryption is 
 * supported.
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
 * January 11, 2026
 *  
 *  Source:  arduino-esp32/librairies/WiFi/examples/WiFiScan/WiFiScan.ino
 *
 * -------------------------------------------
 * ERROR
 * -------------------------------------------
 *
 * The code generates a run-time error: 
 *   E (5094) wifi:can not get wifi protocol under WiFi band mode WIFI_BAND_MODE_AUTO, please use esp_wifi_get_protocols instead
 * when setting the Wi-Fi in STA mode with arduino-esp32 (V3.3.4 and V3.3.5).
 *
 * However the scan will proceed correctly.
 *   
 * See Issue 12200: ESP32-C5 Runtime error on WiFi.mode() 
 * @https://github.com/espressif/arduino-esp32/issues/12200  
 * A fix already exits 
 *  @https://github.com/espressif/arduino-esp32/pull/12202 so the error 
 * should not occur in future releases of the Arduino core for the ESP32.
 */    
    

