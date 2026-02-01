/*
 *  See wifi_scan.ino for license and attribution.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "MACs.h"

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
#define SERIAL_BAUD 115200
///
///  Time in milliseconds to wait after Serial.begin() in 
///  the setup() function. If not defined, it will be set
///  to 5000 if running in the PlaformIO IDE to manually switch
///  to the serial monitor otherwise to 2000 if an native USB 
///  peripheral is used or 1000 if a USB-serial adpater is used.
///#define SERIAL_BEGIN_DELAY 10000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "XIAO_ESP32C5"
  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "ONBOARD CERAMIC"
  #endif
#else
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#endif  

void setup() {
  #if !defined(SERIAL_BEGIN_DELAY)
    #if defined(PLATFORMIO)
      #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
    #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
      #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
    #else
      #define SERIAL_BEGIN_DELAY 1000    // 1 second
    #endif
  #endif 

  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: wifi_scan");
  Serial.println("Purpose: List found Wi-Fi networks");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Antenna: %s\n\n", ANTENNA);
  
  Serial.println("Starting Wi-Fi in STA(tion) mode");
  WiFi.STA.begin();

  Serial.println("Setup done");
}

void printDashes(int n = 83) {
            //            10        20        30        40        50        60        70        80
            //    1234567890123456789012345678901234567890123456789012345678901234567890123456789012345
  #define DASHES "-------------------------------------------------------------------------------------"
  printf("%.*s\n", n, DASHES); 
}  


void ScanWiFi() {
  Serial.print("Scan start... ");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println("done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             |       BSSID       | RSSI |  CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d | ", i + 1);
      Serial.printf("%-32.32s | ", WiFi.SSID(i).c_str());
      Serial.printf("%-17.17s | ", WiFi.BSSIDstr(i).c_str());
      Serial.printf("%4ld | ", WiFi.RSSI(i));
      Serial.printf("%3ld | ", WiFi.channel(i));
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
  Serial.println();
}

void loop() {
  printDashes();
  Serial.println("Default wifi band mode scan:");
  WiFi.setBandMode(WIFI_BAND_MODE_AUTO); // introdudced in ESP-IDF 5.4.2, esp32 3.3.6 is based on ESP-IDF 5.5.2

  // First scan will contain network on both bands if 5G supporte, 2.4GHz band only otherwise
  ScanWiFi();

#if CONFIG_SOC_WIFI_SUPPORT_5G
  // both bands supported, so scan bands separately, 5 GHz band first
  // Wait a bit before scanning again.
  delay(1000);
  printDashes();
  Serial.println("2.4 Ghz wifi band mode scan:");
  WiFi.setBandMode(WIFI_BAND_MODE_2G_ONLY);
  ScanWiFi();

  // Wait a bit before scanning again, 2.4 GHz band second.
  delay(1000);
  printDashes();
  Serial.println("5 Ghz wifi band mode scan:");
  WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);
  ScanWiFi();
#endif
  // Wait a bit before scanning again.
  delay(15000);
}
