#include <Arduino.h>
#include "esp_mac.h"

String getDefaultMacAddress() {

  String mac = "";

  unsigned char mac_base[8] = {0};
 
  if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
    char buffer[24];  // 8*2 characters for hex + 7 characters for colons + 1 character for null terminator
#ifdef CONFIG_SOC_IEEE802154_SUPPORTED
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5], mac_base[6], mac_base[7]);
#else 
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
#endif
    mac = buffer;
  }

  return mac;
}

String getInterfaceMacAddress(esp_mac_type_t interface) {

  String mac = "";
  unsigned char mac_base[8] = {0x5A};
  char buffer[24];  // 8*2 characters for hex + 7 characters for colons + 1 character for null terminator

  /*
  if (interface == ESP_MAC_EFUSE_CUSTOM) {
    esp_err_t err = esp_efuse_mac_get_custom(mac_base);
    Serial.printf("esp_efuse_mac_get_custom = 0x%2x\n", err);   
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;  
    return mac;  
  }
  */

  if (interface == ESP_MAC_EFUSE_CUSTOM) {
    if (esp_efuse_mac_get_custom(mac_base) == ESP_OK) {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
      mac = buffer;  
    }  
    return mac;  
  } 

#ifdef CONFIG_SOC_IEEE802154_SUPPORTED
  if (esp_read_mac(mac_base, interface) == ESP_OK) {
    if (interface == ESP_MAC_IEEE802154) {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5], mac_base[6], mac_base[7]);
    } else if(interface == ESP_MAC_EFUSE_EXT) {
      sprintf(buffer, "%02X:%02X", mac_base[0], mac_base[1]);
    } else {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    }
    mac = buffer;
  }  
#else // all 6 bytes
  if ((interface != ESP_MAC_IEEE802154) && (interface != ESP_MAC_EFUSE_EXT)) {
    if (esp_read_mac(mac_base, interface) == ESP_OK) {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
      mac = buffer;
    }  
  }
#endif

  return mac;
}
