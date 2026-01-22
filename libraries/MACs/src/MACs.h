#pragma once

#include "esp_mac.h"

/*
 * Written by: Daniel Nebert
 *
 * The first printed MAC address is obtained by calling 'esp_efuse_mac_get_default'
 * (https://docs.espressif.com/projects/esp-idf/en/release-v5.1/esp32/api-reference/system/misc_system_api.html#_CPPv425esp_efuse_mac_get_defaultP7uint8_t)
 * which returns base MAC address which is factory-programmed by Espressif in EFUSE.
 *
 * The remaining printed MAC addresses is obtained by calling 'esp_read_mac'
 * (https://docs.espressif.com/projects/esp-idf/en/release-v5.1/esp32/api-reference/system/misc_system_api.html#_CPPv412esp_read_macP7uint8_t14esp_mac_type_t)
 * and passing in the 'esp_mac_type_t' type.
 * (https://docs.espressif.com/projects/esp-idf/en/release-v5.1/esp32/api-reference/system/misc_system_api.html#esp__mac_8h_1a1b19aca597277a2179682869c140477a)
 *

esp_mac_type_t values:

  ESP_MAC_WIFI_STA - MAC for WiFi Station (6 bytes)
  ESP_MAC_WIFI_SOFTAP - MAC for WiFi Soft-AP (6 bytes)
  ESP_MAC_BT - MAC for Bluetooth (6 bytes)
  ESP_MAC_ETH - MAC for Ethernet (6 bytes)
  ESP_MAC_IEEE802154 - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes)
  ESP_MAC_BASE - Base MAC for that used for other MAC types (6 bytes)
  ESP_MAC_EFUSE_FACTORY - MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes)
  ESP_MAC_EFUSE_CUSTOM - MAC_CUSTOM eFuse which was can be burned by customer (6 bytes)
  ESP_MAC_EFUSE_EXT - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes)
*/

extern String getDefaultMacAddress();
extern String getInterfaceMacAddress(esp_mac_type_t interface);


// Macro definitions for most common interfaces, returns String

#define STA_MAC getInterfaceMacAddress(ESP_MAC_WIFI_STA)
#define SOFTAP_MAC getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP)
#define BT_MAC getInterfaceMacAddress(ESP_MAC_BT)
#define ETH_MAC getInterfaceMacAddress(ESP_MAC_ETH)
#define ZIGBEE_MAC getInterfaceMacAddress(ESP_MAC_IEEE802154)    // no way I would remember IEEE802154_MAC


// Macro definitions for most common interfaces, returns C string

#define STA_MAC_STR getInterfaceMacAddress(ESP_MAC_WIFI_STA).c_str()
#define SOFTAP_MAC_STR getInterfaceMacAddress(ESP_MAC_WIFI_SOFTAP).c_str()
#define BT_MAC_STR getInterfaceMacAddress(ESP_MAC_BT).c_str()
#define ETH_MAC_STR getInterfaceMacAddress(ESP_MAC_ETH).c_str()
#define ZIGBEE_MAC_STR getInterfaceMacAddress(ESP_MAC_IEEE802154).c_str() 

