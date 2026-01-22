/*
 *  See blink.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
/// Time in milliseconds the LED is on during the heartbeat
#define ON_TIME  80
///
/// Time in millisecods the LED is off during the heartbeat
#define OFF_TIME 840
///
/// Time in milliseconds during which the current io pin is active
#define ACTIVE_PERIOD 6000
///
/// Time of inactivity in milliseconds between pins
#define INACTIVE_PERIOD 2000
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

unsigned long beattime = 0;
int beatstate = 0;
int beatdelay = OFF_TIME;  // will alternate betwee ON_TIME and OFF_TIME
int ledOn = 1;

/* beatstate
  0 (even) = turn on  for short delay 
  1  (odd) = turn off for short delay 
  2 (even) = turn on  for short delay 
  3  (odd) = turn off for long delay  
*/

unsigned long activetimer = 0;  // used to time active period

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
    Serial.println("  --all--off--");
    delay(blankperiod);
  }
  currentindex = i;
  if (currentindex == padcount - 1) {
    Serial.printf("\nThe yellow user LED (gpio pin %d) is turned on and off.\n", iopins[currentindex]);
    ledOn = 0;
  } else { 
    Serial.printf("\nProbe board pad %s as gpio pin %d is turned on and off.\n", padlabels[i], iopins[currentindex]);
    ledOn = 1;
  }  
  beatstate = 0;
  beatdelay = ON_TIME;
  pinMode(iopins[currentindex], OUTPUT);
  digitalWrite(iopins[currentindex], ledOn);
  beattime = millis();
  activetimer = millis();
}  

void heartbeat(void) {
  if (millis() - beattime > beatdelay) {
    // take care of turning LED on or off
    if ((beatstate & 1) == 1) // if beatstate odd turn LED off 
       digitalWrite(iopins[currentindex], 1-ledOn);
    else                      // if beatstate even turn LED on
       digitalWrite(iopins[currentindex], ledOn);
    
    // take care of setting the on or off delay and 
    // update the beat state
    if (beatstate >= 3) {
      beatdelay = OFF_TIME;
      beatstate = 0;
    } else {
      beatdelay = ON_TIME;
      beatstate++;
    }
    beattime = millis();
  }
}

//----------------------------------------------

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  

  Serial.println("\n\nProject: blink all i/o pins");
  Serial.println("  Board: XIAO ESP32C5");

  Serial.println("\nCheck the digital write function of the GPIO pins of an XIAO ESP32C5.");
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
  heartbeat();
}
