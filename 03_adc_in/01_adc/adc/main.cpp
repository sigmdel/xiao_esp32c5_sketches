/*
 *  See adc.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
/// Define this macro to displays the raw (integer) ADC data obtained by analogRead(). 
//  If the macro define then raw data is internally processed and obtained using 
//  analogReadMilliVolts(). The millivolts are then converted to volts for display.
///#define RAW 
///
/// Define this macro if the data is to be plotted in the Serial Plotter of the Arduino IDE
/// This inhibits printing text to the serial monitor which confuses the plotter.
#define PLOTTER 
///
/// Time in milliseconds during which the current io pin is active
#define ACTIVE_PERIOD 12000
///
/// Time of inactivity in milliseconds between pins
#define INACTIVE_PERIOD 2000
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
#define SERIAL_BAUD 115200
///
//////////////////////////////////




// timing constants all in milliseconds

const int activeperiod  = ACTIVE_PERIOD;   // time during which the current io pin is read
const int blankperiod   = INACTIVE_PERIOD; // time during which no io pin is active

unsigned long activetimer = 0;  // used to time active period

#if !defined(ARDUINO_XIAO_ESP32C5)
  #error "This program is meant to run on the XIAO ESP32C5"
#endif

// Checking XIAO ESP32C5 div board with USB connector at top, antenna connector at bottom.
// Probe each pad in anti-clockwise fashion starting at top left pad labeled D0 (on the underside)
const int padcount = 5;
const char *padlabels[padcount] = {"A0", "MTMS (A1)", "MTDI (A2)", "MTCK (A3)", "MTDO (A4)"};   
const int iopins[padcount] = { A0, A1, A2, A3, A4};   

int currentindex = -1;  // initially, no gpio pin is active

void setindex(int i) {
  if ((i >= padcount) || (i < 0)) i = 0;
  if (currentindex == i) {
    Serial.printf("ERROR: I/O index %d unchanged; do nothing.\n", i);
    return;
  } 
  if (currentindex >= 0) {
    delay(blankperiod);
  }
  currentindex = i;
  #ifndef PLOTTER
  Serial.printf("\nConnect board pad labeled %s to an analog signal.\n", padlabels[currentindex]);
  #endif

}  


//----------------------------------------------

void setup() {
  Serial.begin(); // 
  delay(2000); // 2 second delay should be sufficient for USB-CDC

  Serial.println("\n\nCheck the adc function of the analogue GPIO pins of an XIAO ESP32C5.\n");
  Serial.println("There's only one analogue pin on the top side of the XIAO: A0 near the charge LED.");
  Serial.println("The other A1 to A4 connections, labeled MTMS, MTDI, MTCK and MTDO, are");
  Serial.println("pads on the reverve side of the board.");

  Serial.println("Connect an analogue signal source to each GPIO pin as prompted on the serial monitor.");
  Serial.println("Be careful, the signal should be in the range of 0 to 3,3 votls.");
  Serial.println("The measured voltage on the analogue pin (or its raw integer value if the");
  Serial.println("RAW macro is defined) will be displayed in the serial monitor or it can be");
  Serial.println("viewed in the serial ploter.");

  #ifdef RAW
  Serial.println("The raw input ADC data will be displayed");
  #else
  Serial.println("The input data converted to volts will be displayed");
  #endif
  
  delay(2000); // time to read above message

  // Set the resolution to 12 bits (0-4095)
  analogReadResolution(12);
  
  // start process
  setindex(0);
  activetimer = millis();
}

void loop() {
  if (millis() - activetimer > activeperiod) {
    setindex(currentindex+1);
    activetimer = millis();
  }   
  #ifdef RAW
  int invalue = analogRead(iopins[currentindex]);
    #ifdef PLOTTER
    Serial.printf("IN:%d\tMaxDAC:%d\tGND:0\n", invalue, 4095);
    #else
    Serial.printf("IN:%d\tMaxDAC:%d\tGND:0\n", invalue, 4095);
    #endif
  #else
  int inmv = analogReadMilliVolts(iopins[currentindex]);
  float volts = inmv/1000.0;   
    #ifdef PLOTTER
    Serial.printf("Volts:%.3f\tMax:3.3\tGND:0\n", volts);
    #else
    Serial.printf("Volts(%s):%.3f\tMax:3.3\tGND:0\n", padlabels[currentindex], volts);
    #endif
  #endif
  delay(100);      
}	

