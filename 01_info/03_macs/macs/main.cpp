/*
 *  See macs.ino for license and attribution.
 */

 /**
Project: Print MACs
  Board: Seeed XIAO ESP32C5
Antenna: A-01 FPC

      Default D0:CF:13:FE:FF:E2:22:8C	 getDefaultMacAddress()
         BASE D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_BASE)
    Wi-Fi STA D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_WIFI_STA)
 Wi-Fi softAP D0:CF:13:E2:22:8D		 getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
    Bluetooth D0:CF:13:E2:22:8E		 getInterfaceMacAddress(ESP_MAC_BT)
     Ethernet D0:CF:13:E2:22:8F		 getInterfaceMacAddress(ESP_MAC_ETH)
   IEEE802154 D0:CF:13:FE:FF:E2:22:8C	 getInterfaceMacAddress(IEEE802154)
      EXT MAC FE:FF			 getInterfaceMacAddress(ESP_MAC_EFUSE_EXT)
      Factory D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY)
E (4153) system_api: eFuse MAC_CUSTOM is empty
       Custom 00:00:00:00:00:00		 getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM)


      Default D0:CF:13:FE:FF:E2:22:8C	 getDefaultMacAddress()
         BASE D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_BASE)
    Wi-Fi STA D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_WIFI_STA)
 Wi-Fi softAP D0:CF:13:E2:22:8D		 getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
    Bluetooth D0:CF:13:E2:22:8E		 getInterfaceMacAddress(ESP_MAC_BT)
     Ethernet D0:CF:13:E2:22:8F		 getInterfaceMacAddress(ESP_MAC_ETH)
   IEEE802154 D0:CF:13:FE:FF:E2:22:8C	 getInterfaceMacAddress(IEEE802154)
      EXT MAC FE:FF			 getInterfaceMacAddress(ESP_MAC_EFUSE_EXT)
      Factory D0:CF:13:E2:22:8C		 getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY)
       Custom 00:00:00:00:00:00		 getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM)

********************************************************************************



Project: Print MACs
  Board: Seeed XIAO ESP32C6
Antenna: INTERNAL CERAMIC

      Default 54:32:04:FF:FE:11:F4:90	 getDefaultMacAddress()
         BASE 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_BASE)
    Wi-Fi STA 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_WIFI_STA)
 Wi-Fi softAP 54:32:04:11:F4:91		 getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
    Bluetooth 54:32:04:11:F4:92		 getInterfaceMacAddress(ESP_MAC_BT)
     Ethernet 54:32:04:11:F4:93		 getInterfaceMacAddress(ESP_MAC_ETH)
   IEEE802154 54:32:04:FF:FE:11:F4:90	 getInterfaceMacAddress(IEEE802154)
      EXT MAC FF:FE			 getInterfaceMacAddress(ESP_MAC_EFUSE_EXT)
      Factory 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY)
E (2139) system_api: eFuse MAC_CUSTOM is empty
       Custom 00:00:00:00:00:00		 getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM)


      Default 54:32:04:FF:FE:11:F4:90	 getDefaultMacAddress()
         BASE 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_BASE)
    Wi-Fi STA 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_WIFI_STA)
 Wi-Fi softAP 54:32:04:11:F4:91		 getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
    Bluetooth 54:32:04:11:F4:92		 getInterfaceMacAddress(ESP_MAC_BT)
     Ethernet 54:32:04:11:F4:93		 getInterfaceMacAddress(ESP_MAC_ETH)
   IEEE802154 54:32:04:FF:FE:11:F4:90	 getInterfaceMacAddress(IEEE802154)
      EXT MAC FF:FE			 getInterfaceMacAddress(ESP_MAC_EFUSE_EXT)
      Factory 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY)
E (12145) system_api: eFuse MAC_CUSTOM is empty
       Custom 00:00:00:00:00:00		 getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM)


      Default 54:32:04:FF:FE:11:F4:90	 getDefaultMacAddress()
         BASE 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_BASE)
    Wi-Fi STA 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_WIFI_STA)
 Wi-Fi softAP 54:32:04:11:F4:91		 getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
    Bluetooth 54:32:04:11:F4:92		 getInterfaceMacAddress(ESP_MAC_BT)
     Ethernet 54:32:04:11:F4:93		 getInterfaceMacAddress(ESP_MAC_ETH)
   IEEE802154 54:32:04:FF:FE:11:F4:90	 getInterfaceMacAddress(IEEE802154)
      EXT MAC FF:FE			 getInterfaceMacAddress(ESP_MAC_EFUSE_EXT)
      Factory 54:32:04:11:F4:90		 getInterfaceMacAddress(ESP_MAC_EFUSE_FACTORY)
E (22151) system_api: eFuse MAC_CUSTOM is empty
       Custom 00:00:00:00:00:00		 getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM)


 */

#include <Arduino.h>

#include "MACs.h"

#include <exception>

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
  //Serial.printf("Custom MAC address: \"%s\"\n\n", getInterfaceMacAddress(ESP_MAC_EFUSE_CUSTOM).c_str()); 
  delay(10000);
}
                                                
