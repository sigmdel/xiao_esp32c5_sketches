/*
 *  See interrupt.ino for license and attribution.
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


struct Button {
  uint8_t PIN;  
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button = {D0, 0, false};
bool boot_button_pressed = false;

void ARDUINO_ISR_ATTR button_isr() {
  button.numberKeyPresses += 1;
  button.pressed = true;
}

void ARDUINO_ISR_ATTR boot_button_isr() {
  boot_button_pressed = true;
}


// XIAO ESP32C5 dev board pin layout
//
const int padcount = 11;
const char *padlabels[padcount] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6",  // left edge
                                    "D7", "D8", "D9", "D10"};       // right edge
const int iopins[padcount] = { D0, D1, D2, D3, D4, D5, D6,            // GPIO  1, 0, 25,  7, 23, 24, 11, // left edge
                               D7, D8, D9, D10};         // GPIO 12, 8,  9, 10,              // right edge                                  


// Select the current gpio pin based on the currentindex into the
// above array.

int currentindex = -1;  // initially, no gpio pin is active

void setindex(int i) {
  if ((i >= padcount) || (i < 0)) i = 0;
  if (currentindex == i) {
    Serial.printf("ERROR: I/O index %d unchanged; do nothing.\n", i);
    return;
  } 
  if (currentindex >= 0) {
    // turn current gpio pin off 
    detachInterrupt(iopins[currentindex]);
    digitalWrite(iopins[currentindex], LOW);
    pinMode(iopins[currentindex], INPUT);
    delay(2000);
  }
  currentindex = i;
  button.PIN = iopins[currentindex];
  Serial.printf("\n\nAn ISR has been attached to pin %s (gpio %d) for testing.\n", padlabels[i], button.PIN);
  pinMode(button.PIN, INPUT_PULLUP);
  button.numberKeyPresses = 0;
  attachInterrupt(button.PIN, button_isr, FALLING); 
}  

void printlabel(int pin) {
  for (int i=0; i < padcount; i++) {
    if (iopins[i] == pin) {
      Serial.print(padlabels[i]);
      return;
    }  
  }
  Serial.print("unknown");
}

//----------------------------------------------

void setup() {
  Serial.begin(); // 
  delay(SERIAL_BEGIN_DELAY);

  Serial.println("\n\nProject: check interrupts on all i/o pins");
  Serial.println("  Board: XIAO ESP32C5");

  Serial.println("\n\nCheck the interrupt capabilities of the GPIO pins of an XIAO ESP32C5.");
  Serial.println("An interrupt service routine (ISR) will be attached to each gpio pin in turn.");
  Serial.println("Probe the pin with a grounded wire to test the interrupt."); 
  Serial.println("Press the boot button to move on to the next pin.");
  
  delay(2000); // time to read above message

  // start process
  setindex(0);
  attachInterrupt(BOOT_PIN, boot_button_isr, FALLING);
}

unsigned long timer;

void loop() {
  if (button.pressed) {
    Serial.print("Interrupt on pin ");
    printlabel(button.PIN);
    Serial.printf(" (gpio %d) has been raised %lu times\n", button.PIN, button.numberKeyPresses);
    button.pressed = false;
    button.numberKeyPresses = 0;
    /*
    // debouncing sort of. Can't use delay() if
    // interrupts are disabled.
    noInterrupts();  // not a good idea
    timer = millis();
    while (millis() - timer < 100) {  };   // do nothing
    interrupts();
    */
  }
  if (boot_button_pressed) {
    setindex(currentindex + 1);
    boot_button_pressed = false;
  }
}




