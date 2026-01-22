/*
 *  See poll.ino for license and attribution.
 */

#include <Arduino.h>

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

#if !defined(ARDUINO_XIAO_ESP32C5)
  #error "This program is meant to run on the XIAO ESP32C5"
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


// XIAO ESP32C5 dev board edge pads layout.
//
const int padcount = 11;
const char *padlabels[padcount] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6",  // left edge
                                   "D7", "D8", "D9", "D10",};                 // right edge
const int iopins[padcount] = {D0, D1, D2, D3, D4, D5, D6,        // GPIO  1, 0, 25,  7, 23, 24, 11, // left edge
                              D7, D8, D9, D10};                  // GPIO 12, 8,  9, 10,             // right edge                                  

int levels[padcount];

int INPUT_MODE = -1;
int currentindex = 0;
bool ALL_LOW = true;

void setPinMode(int mode) {
  Serial.println();
  if (mode == INPUT_PULLUP)
    Serial.println("The mode of all I/O pins set to INPUT_PULLUP, probe pins with a connection to GND.");
  else if (mode == INPUT_PULLDOWN)
    Serial.println("The mode of all I/O pins set to INPUT_PULLDOWN, probe pins with a connection to VCC (3V3).");
  else if (mode == OUTPUT) {
    char conn[4];
    if (ALL_LOW)
      strlcpy(conn, "VCC", sizeof(conn));
    else
      strlcpy(conn, "GND", sizeof(conn));
    Serial.printf("The mode of all I/O pins set to OUTPUT, probe pins with a connection to %s.\n", conn);
  }  
  else {
    // should never get here!
    Serial.printf("setPinMode(%d) is not allowed.\n", mode);
    Serial.printf("Only INPUT_PULLDOWN (%d), INPUT_PULLUP (%d) and OUTPUT (%d) are accetped.\n", INPUT_PULLDOWN, INPUT_PULLUP, OUTPUT);
    return;
  }  
  delay(50);
  for (int i=0; i < padcount; i++) 
    pinMode(iopins[i], mode);  
  INPUT_MODE = mode;
  currentindex = 0;
  delay(50);
  if (mode == OUTPUT) {
    Serial0.end();  // Trying to avoid problem with D6, UARTX0 in output mode, not working!
    for (int i=0; i<padcount; i++) {
      digitalWrite(iopins[i], !ALL_LOW);
      delay(3);
      levels[i] = digitalRead(iopins[i]);
    }
    ALL_LOW = !ALL_LOW;  
  } else {
    int lv = (mode == INPUT_PULLUP); // lv = HIGH, lv = LOW if INPUT_PULLDOWN
    for (int i=0; i<padcount; i++)
      levels[i] = lv;
  }
}

void setup() {
  Serial.begin(); // 
  delay(SERIAL_BEGIN_DELAY);

  Serial.println("\n\nProject: poll all i/o pins");
  Serial.println("  Board: XIAO ESP32C5");

  Serial.println("\nChecking digitalRead() function of the GPIO pins of an XIAO ESP32C5.");
  Serial.println("All the pins are polled sequentially for a change in their value.");
  Serial.println("There are three modes of operation correponding to the mode of all the");
  Serial.println("the pins which can be in INPUT_PULLUP, INPUT_PULLDOWN, or OUTPUT mode.");
  Serial.println("Switch the mode by pressing the BOOT button.");
  delay(4000); // time to read above message

  setPinMode(INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BOOT_PIN) == LOW) {
    delay(200);
    if (digitalRead(BOOT_PIN) == LOW) {
      switch(INPUT_MODE) {
        case INPUT_PULLUP: setPinMode(INPUT_PULLDOWN); break;
        case INPUT_PULLDOWN: setPinMode(OUTPUT); break;
        case OUTPUT: setPinMode(INPUT_PULLUP); break;
      }
      while (digitalRead(BOOT_PIN) == LOW)
        delay(50);
    }  
  }  

  int level = digitalRead(iopins[currentindex]);
  // check if the level has changed
  if (level != levels[currentindex])  {
    Serial.printf("Pin %s (gpio %d) is %s\n", padlabels[currentindex], iopins[currentindex], (level) ? "HIGH" : "LOW");
    levels[currentindex] = level;
    delay(100);
  }
  currentindex = (++currentindex)%padcount;
}




