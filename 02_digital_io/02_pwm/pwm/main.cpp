/*
 *  See pwm.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
/// Time in milliseconds during which duty cycle value is used
#define LEVEL_PERIOD 50 
//.
/// The increment or decrement in the duty cycle between levels.
#define DELTA 10
///
/// The initial duty cycle above 0
#define INIT_DELTA 5
///
/// Time in milliseconds during which the current io pin is active
#define ACTIVE_PERIOD 8000
///
/// Time of inactivity in milliseconds between pins
#define INACTIVE_PERIOD 3000
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

// Checking XIAO ESP32C5 div board with USB connector at top, antenna connector at bottom.
// Probe each pad in anti-clockwise fashion starting at top left pad labeled D0 (on the underside)
const int padcount = 12;
const char *padlabels[padcount] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6",  // left edge
                                   "D7", "D8", "D9", "D10", "User LED"};      // right edge
const int iopins[padcount] = {D0, D1, D2, D3, D4, D5, D6,        // GPIO  1, 0, 25,  7, 23, 24, 11, // left edge
                              D7, D8, D9, D10, LED_BUILTIN};     // GPIO 12, 8,  9, 10, 27          // right edge                                  

// timing constants all in milliseconds
const int lightperiod  = ACTIVE_PERIOD;   // time during which the current io pin is active
const int blankperiod  = INACTIVE_PERIOD; // time during which no io pin is active

unsigned long activetimer = 0;  // used to time active period

unsigned long delaytime = 0;
int delta = 0;   // pwm increment or decrement value - starts at 5 to hit 255 when incrementing by DELTA = 10 
int fade = 0;    // actual pwm value 

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
    digitalWrite(iopins[currentindex], LOW);
    pinMode(iopins[currentindex], INPUT);
    delay(blankperiod);
  }
  currentindex = i;
  if (currentindex == padcount - 1) 
    Serial.printf("The yellow user LED (gpio pin %d) is being pulsed (pwm).\n", iopins[currentindex]);
  else 
    Serial.printf("Probe board pad %s as gpio pin %d is being pulsed (pwm).\n", padlabels[i], iopins[currentindex]);
  pinMode(iopins[currentindex], OUTPUT);
  analogWrite(iopins[currentindex], 0);
  activetimer = millis();
  fade = 0;
  delta = INIT_DELTA;
  delaytime = 0;
}  

//////////////////////////////////////////
// pulse current gpio pin
// test of pwm - analogWrite
//

void pulse(void) {
  if (millis() - delaytime > LEVEL_PERIOD) {
    fade += delta;
    if (fade <= 0) {
      fade = INIT_DELTA;
      delta = DELTA;
    } else if (fade >= 255) {
      fade = 255;
      delta = - DELTA;
    }
    analogWrite(iopins[currentindex], fade);
    delaytime = millis();
  }
}

//----------------------------------------------

void setup() {
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY); 

  Serial.println("\n\nProject: pulse all i/o pins");
  Serial.println("  Board: XIAO ESP32C5");

  Serial.println("\n\nCheck the pwm function of the GPIO pins of an XIAO ESP32C5.");
  Serial.println("Hold the board, component side up, with the USB connector towards the top.");
  Serial.println("Probe each pad in a anti-clockwise fashion starting with the top left pad labelled D0.");
  
  delay(2000); // time to read above message

  // start process
  setindex(0);
}

void loop() {
  if (millis() - activetimer > lightperiod) {
    setindex(currentindex+1);
  }    
  pulse();
}




