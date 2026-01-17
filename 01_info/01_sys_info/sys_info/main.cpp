/*
 *  See sys_info.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
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

void printFactoryMac(void) {
  union {
    uint64_t factmac;
    char bytes[sizeof(factmac)];
   } mac;

  Serial.print("  MAC: ");
  #ifdef CONFIG_SOC_IEEE802154_SUPPORTED
	int n = sizeof(uint64_t);  
  #else
  int n = 6;
  #endif
  mac.factmac = ESP.getEfuseMac();
  for (int i=0; i<n; i++) {
    Serial.printf("%02x", mac.bytes[i]);
    if (i<n-1) Serial.print(":"); else Serial.println();
  }
}


void printFlashChipMode(FlashMode_t mode) {
  switch (mode) {
    case FM_QIO:  Serial.print("FM_QIO"); break;
    case FM_QOUT: Serial.print("FMQOUT"); break;
    case FM_DIO:  Serial.print("FM_DIO"); break;
    case FM_DOUT: Serial.print("FM_DOUT"); break;
    case FM_FAST_READ: Serial.print("FM_FAST_READ"); break;
    case FM_SLOW_READ: Serial.print("FM_SLOW_READ"); break;
	default: Serial.print("FM_UNKNOWN");
  }
}

void getInfo(void) {
  Serial.println("\n\nESP32 Chip Information\n");
	Serial.printf("Chip model: %s, Revision: %d\n", ESP.getChipModel(), ESP.getChipRevision());
	Serial.printf("  Core count: %d \n", ESP.getChipCores());
	Serial.printf("  CPU frequency: %lu MHz\n", ESP.getCpuFreqMHz());
	Serial.printf("  Cycle count: %lu\n", ESP.getCycleCount());
  printFactoryMac();
	Serial.printf("  SDK version: %s\n", ESP.getSdkVersion());

	Serial.println("\nFlash Memory");
	Serial.printf("  Flash size: %lu\n", ESP.getFlashChipSize());
	Serial.printf("  Flash speed: %lu\n", ESP.getFlashChipSpeed());
	Serial.print("  Flash mode: ");
	printFlashChipMode(ESP.getFlashChipMode());
	Serial.printf(" (%d)\n", ESP.getFlashChipMode());

	Serial.println("\nSPI RAM Memory (PSRAM)");
	Serial.printf("  PSRAM size: %lu\n", ESP.getPsramSize());
	Serial.printf("  Free PSRAM: %lu\n", ESP.getFreePsram());
	Serial.printf("  Min free PSRAM: %lu\n", ESP.getMinFreePsram());
	Serial.printf("  Max PSRAM alloc size: %lu\n", ESP.getMaxAllocPsram());


  Serial.println("\nSketch");
  Serial.printf("  Size: %lu\n", ESP.getSketchSize());
	Serial.printf("  Free space: %lu\n", ESP.getFreeSketchSpace());

	Serial.println("\nHeap");
	Serial.printf("  Size: %lu\n", ESP.getHeapSize()); //total heap size
	Serial.printf("  Free: %lu\n", ESP.getFreeHeap()); //available heap
	Serial.printf("  Mininum free since boot: %lu\n", ESP.getMinFreeHeap()); //lowest level of free heap since boot
	Serial.printf("  Maximum allocation size: %lu\n", ESP.getMaxAllocHeap()); //largest block of heap that can be allocated


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
}

void loop() {
  getInfo();
	delay(30000);
}
