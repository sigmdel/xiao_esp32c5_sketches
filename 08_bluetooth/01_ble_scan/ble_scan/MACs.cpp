#include <Arduino.h>
#include "NetworkManager.h"

String getmac(int i) {
  uint8_t mac[6];
  
  Network.macAddress(&mac[0]);
  mac[5] += i;
  String result = "";
  for (int i=0; i<6; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", mac[i]);
    result += buffer;
    if (i<5) result += ":";
  }
  return result;
}

String baseMAC(void) {
  return getmac(0);
}  

String staMAC(void) {
  return getmac(0);
}

String softAPMac(void) {
  return getmac(1);
}

String bleMAC(void) {
  return getmac(2);
}