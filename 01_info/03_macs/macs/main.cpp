/*
 *  See macs.ino for license and attribution.
 */

#include <Arduino.h>

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

void printMACS(void) {
  Serial.println("\n");
  Serial.printf("      Default MAC %s\n", defaultMAC().c_str()); // can be 8 or 6 bytes 
  Serial.printf("         Base MAC %s\n", baseMAC().c_str());    // 6 bytes 
  Serial.printf("    Wi-Fi STA MAC %s\n", staMAC().c_str());     // administered address, 6 bytes
  Serial.printf("Wi-Fi soft AP MAC %s\n", softapMAC().c_str());  // administered address, 6 bytes
  Serial.printf("Bluetooth BD_ADDR %s\n", btMAC().c_str());      // administered address, 6 bytes
  Serial.printf("     Ethernet MAC %s\n", etherMAC().c_str());   // administered address, 6 bytes
}  

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
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: Print MACs");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("Antenna: %s\n\n", ANTENNA);
}

void loop() {
  printMACS();
  delay(10000);
}
                                                
