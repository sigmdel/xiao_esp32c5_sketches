/*
 *  See macs.ino for license and attribution.
 */

#include <Arduino.h>

#include "MACs.h"

//////// User configuration //////
///
///  Define if a custom MAC address was burned in ESP32 fuse memory
///#define HAS_CUSTOM_MAC
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
///*define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 4))    
  #error ESP32 Arduino core version 3.3.4 or newer needed
#endif 

#if !defined(SERIAL_BEGIN_DELAY)
  #if defined(PLATFORMIO)
    #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
  #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
    #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
  #else
    #define SERIAL_BEGIN_DELAY 1000    // 1 second
  #endif
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
  Serial.println("\n\nMAC Adresses-----------------------------------------------------------------------------");
  Serial.printf("      Default %-23s  uses getDefaultMacAddress()\n",     getDefaultMacAddress().c_str()); // can be 8 or 6 bytes 
  Serial.printf("         BASE %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_BASE).c_str(), "ESP_MAC_BASE");     
  Serial.printf("    Wi-Fi STA %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_WIFI_STA).c_str(), "ESP_MAC_WIFI_STA");        // administered address, 6 bytes
  
  Serial.printf(" Wi-Fi softAP %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP).c_str(), "ESP_MAC_WIFI_SOFTAP");  // administered address, 6 bytes
  Serial.printf("    Bluetooth %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_BT).c_str(), "ESP_MAC_BT");                    // administered address, 6 bytes
  Serial.printf("     Ethernet %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_ETH).c_str(), "ESP_MAC_ETH");                  // administered address, 6 bytes

  Serial.printf("   IEEE802154 %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_IEEE802154).c_str(), "IEEE802154");    
  Serial.printf("      EXT MAC %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_EFUSE_EXT).c_str(), "ESP_MAC_EFUSE_EXT");  

  Serial.printf("      Factory %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY).c_str(), "ESP_MAC_EFUSE_FACTORY");
  #ifdef HAS_CUSTOM_MAC
  Serial.printf("       Custom %-23s  uses getInterfaceMacAddress(%s)\n", getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM).c_str(), "ESP_MAC_EFUSE_CUSTOM"); 
  #else
  Serial.printf("       Custom %-23s  no custom MAC in eFuse bits\n", " ");
  #endif
  Serial.println(    "-----------------------------------------------------------------------------------------");

  Serial.println("\nMacros-----------------------------------------------------------------------------------");
  Serial.printf("    Wi-Fi STA %-23s  uses STAT_MAC.c_str()\n", STA_MAC.c_str());
  Serial.printf(" Wi-Fi softAP %-23s  uses SOFTAP_MAC.c_str(()\n", SOFTAP_MAC.c_str());
  Serial.printf("    Bluetooth %-23s  uses BT_MAC.c_str()\n", BT_MAC.c_str());
  Serial.printf("     Ethernet %-23s  uses ETH_MAC.c_str()\n", ETH_MAC.c_str());
  Serial.printf("   IEEE802154 %-23s  uses ZIGBEE_MAC.c_str()\n", ZIGBEE_MAC.c_str());
  Serial.println(  "-----------------------------------------------------------------------------------------");

  Serial.println("\nMacros-----------------------------------------------------------------------------------");
  Serial.printf("    Wi-Fi STA %-23s  uses STAT_MAC_STR\n", STA_MAC_STR);
  Serial.printf(" Wi-Fi softAP %-23s  uses SOFTAP_MAC_STR\n", SOFTAP_MAC_STR);
  Serial.printf("    Bluetooth %-23s  uses BT_MAC_STR\n", BT_MAC_STR);
  Serial.printf("     Ethernet %-23s  uses ETH_MAC_STR\n", ETH_MAC_STR);
  Serial.printf("   IEEE802154 %-23s  uses ZIGBEE_MAC_STR\n", ZIGBEE_MAC_STR);
  Serial.println(  "-----------------------------------------------------------------------------------------");
}

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
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
  //Serial.printf("Custom MAC address: \"%s\"\n\n", getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM).c_str()); 
  delay(10000);
}
                                                
