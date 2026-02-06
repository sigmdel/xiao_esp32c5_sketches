/*
 *  See heap.ino for license and attribution.
 */

#include <Arduino.h>

#include "esp_heap_caps.h"

//////// User configuration //////
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
///#define SERIAL_BEGIN_DELAY 8000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif 

#if !defined(BOARD_HAS_PSRAM)
  #error The board must containt PSRAM 
#endif  

#if defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
  #if defined (ARDUINO_VARIANT)
    #define VARIANT ARDUINO_VARIANT  
  #endif
#elif defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "XIAO_ESP32C5"
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "XIAO ESP32S3"
#elif 
  #define TITLE "Unknown ESP32 board"
#endif  

void logHeaps(void) {
  Serial.println("\n\nHeap in Internal Memory");
	Serial.printf("  Size: %lu\n", ESP.getHeapSize()); //total heap size
	Serial.printf("  Free: %lu\n", ESP.getFreeHeap()); //available heap
	Serial.printf("  Mininum free since boot: %lu\n", ESP.getMinFreeHeap()); //lowest level of free heap since boot
	Serial.printf("  Maximum allocation size: %lu\n", ESP.getMaxAllocHeap()); //largest block of heap that can be allocated 

  Serial.println("\nHeap in Pseudo Static RAM Memory (PSRAM)");
  Serial.printf("  size: %lu\n", ESP.getPsramSize());
  Serial.printf("  Free: %lu\n", ESP.getFreePsram());
  Serial.printf("  Minimum free since boot: %lu\n", ESP.getMinFreePsram());
  Serial.printf("  Maximum allocation size: %lu\n", ESP.getMaxAllocPsram());       
}


/*
  thePtr must be a pointer to an allocated block of a size >= bsize.
  Writes bsize byte values to the block and reads them back checking their value.
  Prints the number of errors and the time to write and verify in milliseconds
  Returns time in milliseconds.
*/
unsigned long testAllocatedRam(byte* thePtr, int bsize) {
  
  #define TEST_LOOP_COUNT 500   // required for about 30 ms time on ESP32C5

  unsigned long timer = millis();
  int errcnt = 0;

  byte* aptr;

  for (int j = 0; j < TEST_LOOP_COUNT; j++) {
    aptr = thePtr;
    //Serial.print(".");
    // write 0, 1, 2... 255 over and over again
    for (int i = 0; i < bsize; i++) {
      *aptr = (byte) i;
      aptr++;
    }

    // reads and verifies the bytes written 
    aptr = thePtr;
    for (int i = 0; i < bsize; i++) {
      if (*aptr != (byte) i) { 
        //Serial.printf("Error at %p read %d should have been %d\n", aptr, *aptr, i);
        errcnt++;
      }
      aptr++;
    }
  }

  timer = millis() - timer;

  switch (errcnt) {
    case 0: Serial.print("No errors"); break;
    case 1: Serial.print("One error"); break;
    default: Serial.printf("%d errors", errcnt); break;
  }
  Serial.printf(" reading and writing to allocated ram at %p\n", thePtr); 
  return timer;
}

void setup() {
  // put your setup code here, to run once:
  #if !defined(SERIAL_BEGIN_DELAY)
    #if defined(PLATFORMIO)
      #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
    #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
      #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
    #else
      #define SERIAL_BEGIN_DELAY 1000    // 1 second
    #endif
  #endif 

  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif                          

  Serial.println("\n\nProject: psram");
  Serial.println("Purpose: Investigate PSRAM and the heap");
  Serial.printf("  Board: %s\n", TITLE);
  #ifdef VARIANT
  Serial.printf("Variant: %s\n", VARIANT);
  #endif

  if (!psramFound()) {
    // sanity check
    Serial.println("\n\n*** Error: PSRAM not found ***");
    Serial.println("Restarting the board 10 seconds.");
    delay(10000);
    ESP.restart();
  } 

  logHeaps();

  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_DEFAULT) // Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call");  
  heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_SPIRAM) //  Memory must be in SPI RAM (PSRAM))");  
  heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_INTERNAL) // Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off");  
  heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_8BIT) // Memory must allow for 8/16/...-bit data accesses");
  heap_caps_print_heap_info(MALLOC_CAP_8BIT);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_32BIT) //  Memory must allow for aligned 32-bit data accesses");
  heap_caps_print_heap_info(MALLOC_CAP_32BIT);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_DMA) //  Memory must be able to accessed by DMA");
  heap_caps_print_heap_info(MALLOC_CAP_DMA);
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_RTCRAM) // Memory must be in RTC fast memory");  
  heap_caps_print_heap_info(MALLOC_CAP_RTCRAM);
  
  int testBlockSize = 1000;

  byte* heapblock = (byte*) heap_caps_malloc(testBlockSize, MALLOC_CAP_INTERNAL);
  byte* psramblock = (byte*) heap_caps_malloc(testBlockSize, MALLOC_CAP_SPIRAM);

  Serial.printf("\n\nAllocated two %d byte blocks\n", testBlockSize);
  Serial.printf("   heapblock at %p in internal heap (MALLOC_CAP_INTERNAL)\n", heapblock);
  Serial.printf("  psramblock at %p in PSRAM heap (MALLOC_CAP_SPIRAML)\n", psramblock);

  logHeaps();

  Serial.println();

  delay(100);
  unsigned long heaptime = testAllocatedRam(heapblock, testBlockSize);
  unsigned long psramtime = testAllocatedRam(psramblock, testBlockSize);
  Serial.printf("\nTime to test internal heap block: %lu ms\n", heaptime);
  Serial.printf("   Time to test psram heap block: %lu ms\n", psramtime);  

  /*
  // again, switching the order
  Serial.println("\nRun time tests again, but in reverse order");
  delay(100);
  psramtime = testAllocatedRam(psramblock, testBlockSize);
  heaptime = testAllocatedRam(heapblock, testBlockSize);
  Serial.printf("\n   Time to test psram heap block: %lu ms\n", psramtime);  
  Serial.printf("Time to test internal heap block: %lu ms\n", heaptime);
  */

  heap_caps_free(psramblock);
  heap_caps_free(heapblock);

  Serial.print("\nBoth blocks freed");
  logHeaps(); 
  
  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_INTERNAL) // Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off");  
  heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
  //Serial.println("</heap_caps_print_heap_info(MALLOC_CAP_INTERNAL)>");  

  Serial.println("\nheap_caps_print_heap_info(MALLOC_CAP_SPIRAM) //  Memory must be in SPI RAM (PSRAM))");  
  heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
  //Serial.println("</heap_caps_print_heap_info(MALLOC_CAP_SPIRAM)>");  
 
}

void loop(void) {};
