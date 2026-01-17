#include <Arduino.h>

#include "NetworkManager.h"

// convert an hex representation of an array of octets as a String 
String fromArray(uint8_t* bytes, int len = 6) {
  String result = "";
  for (int i=0; i<len; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", bytes[i]);
    result += buffer;
    if (i < len-1) result += ":";
  }
  return result;
}  

/*
 * The example sketch GetMacAddress.ino in the ESP32 Arduino core  (.../libraries/ESP32/examples/MacAddress/GetMacAddress/GetMacAddress.ino)
 * in version  3.3.6-RC1 released Jan 16, 2026
 *
 *  The getDefaultMacAddress() is hard coded to return a 6 byte address, it is 8 bytes long if CONFIG_SOC_IEEE802154_SUPPORTED is defined 
 *  which it is for the ESP32-C5.
 */

String defaultMAC(void) {
  uint64_t factmac;
  #ifdef CONFIG_SOC_IEEE802154_SUPPORTED
	int n = sizeof(uint64_t);   // 8
  #else
  int n = 6;
  #endif
  factmac = ESP.getEfuseMac();
  return fromArray((uint8_t*)&factmac, n);
}


String getmac(int i) {
  uint8_t mac[6]; 
  Network.macAddress(&mac[0]);
  mac[5] += i;
  return fromArray(&mac[0], 6);
}  


String baseMAC(void) {
  return getmac(0);
}  

String staMAC(void) {
  return getmac(0);
}

String softapMAC(void) {
  return getmac(1);
}

String btMAC(void) {
  return getmac(2);
}

String etherMAC(void) {
  return getmac(3);
}
