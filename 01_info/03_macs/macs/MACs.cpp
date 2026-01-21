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

#define VBnot

String getInterfaceMacAddress(esp_mac_type_t interface) {

  String mac = "";

  unsigned char mac_base[8] = {8};
  char buffer[24];  // 8*2 characters for hex + 7 characters for colons + 1 character for null terminator
  
  if (interface == ESP_MAC_EFUSE_CUSTOM) {
    #ifdef VB
      Serial.print("esp_mac_addr_len_get(ESP_MAC_EFUSE_CUSTOM) ");
    int len = esp_mac_addr_len_get(ESP_MAC_EFUSE_CUSTOM);
    Serial.printf("returns %d\n", len);

    Serial.print("esp_efuse_mac_get_custom(mac_base) "); 
    #endif
    esp_err_t err = esp_efuse_mac_get_custom(mac_base);
    #ifdef VB
    Serial.printf("returns %d\n", err);
    #endif
    if ((err == ESP_OK) || (err == ESP_ERR_INVALID_MAC)) {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
      mac = buffer; 
    } else {
      #ifdef VB
       if (err == ESP_ERR_INVALID_ARG) Serial.println("mac is NULL");
       if (err == ESP_ERR_INVALID_MAC) Serial.println("CUSTOM_MAC address has not been set, all zeros (for esp32-xx)");
       if (err == ESP_ERR_INVALID_VERSION) Serial.println("An invalid MAC version field was read from BLK3 of EFUSE (for esp32)");
       if (err == ESP_ERR_INVALID_CRC) Serial.println("An invalid MAC CRC was read from BLK3 of EFUSE (for esp32)");
      #endif  
    }   

    return mac;  
  }  
  
    
  if (esp_read_mac(mac_base, interface) == ESP_OK) {
    //char buffer[24];  // 8*2 characters for hex + 7 characters for colons + 1 character for null terminator
#ifdef CONFIG_SOC_IEEE802154_SUPPORTED
    if (interface == ESP_MAC_IEEE802154) {
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5], mac_base[6], mac_base[7]);
    } else if(interface == ESP_MAC_EFUSE_EXT){
      sprintf(buffer, "%02X:%02X", mac_base[0], mac_base[1]);
    } else {
#endif
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
#ifdef CONFIG_SOC_IEEE802154_SUPPORTED
    }
#endif
    mac = buffer;
  }

  return mac;
}

