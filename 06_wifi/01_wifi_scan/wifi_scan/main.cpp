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
//////////////////////////////////


#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 4))    
  #error ESP32 Arduino core version 3.3.4 or newer needed
#endif 

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "Seeed XIAO ESP32C5"
  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "INTERNAL CERAMIC"
  #endif
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "Seeed XIAO ESP32S3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ESP32)
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#else  
  #error "An ESP32 SoC required"
#endif        

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(2000); 
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.println();
  #endif  

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: Wi-Fi Scan");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.print("STA MAC: ");
  Serial.println(staMAC());
  Serial.printf("Antenna: %s\n\n", ANTENNA);
  
  #if defined(ARDUINO_XIAO_ESP32C5) && (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))  // assuming this is fixed in 3.3.6
    // Set Debug Core Level to Error or higher in Tool menu  to see run time error 
    // This print statement will make it easier to spot the error 
    Serial.println("\n>>>>>>>>>> Starting Wi-Fi STA <<<<<<<<<<<<<");
    WiFi.STA.begin();
    Serial.println("Wi-Fi STA started");
  #else
    WiFi.STA.begin();
  #endif    

  Serial.println("Setup done");
}

void ScanWiFi() {
  Serial.println("Scan start");
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
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
  Serial.println("-------------------------------------");
}

void loop() {
  Serial.println("-------------------------------------");
  Serial.println("Default wifi band mode scan:");
  Serial.println("-------------------------------------");
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 2)
  WiFi.setBandMode(WIFI_BAND_MODE_AUTO);
#endif
  ScanWiFi();
#if CONFIG_SOC_WIFI_SUPPORT_5G
  // Wait a bit before scanning again.
  delay(1000);
  Serial.println("-------------------------------------");
  Serial.println("2.4 Ghz wifi band mode scan:");
  Serial.println("-------------------------------------");
  WiFi.setBandMode(WIFI_BAND_MODE_2G_ONLY);
  ScanWiFi();
  // Wait a bit before scanning again.
  delay(1000);
  Serial.println("-------------------------------------");
  Serial.println("5 Ghz wifi band mode scan:");
  Serial.println("-------------------------------------");
  WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);
  ScanWiFi();
#endif
  // Wait a bit before scanning again.
  delay(15000);
}
