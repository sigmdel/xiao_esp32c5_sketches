/*
 *  See adc.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
/// Number of reading done before calculating the average voltage
#define AVG_RUN 5
///
/// Time in milliseconds between readings
#define WAIT_PERIOD 2000
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

//----------------------------------------------

void setup() {
  Serial.begin(); // 
  delay(SERIAL_BEGIN_DELAY);
  Serial.println("\n\nChecking the analogReadMilliVolt() with pin A0 of the XIAO ESP32C5.");
  Serial.printf("Data is read every %d milliseconds and displayed\n", WAIT_PERIOD); 
  Serial.printf("The data is averaged over %d obvervations and that data is displayed\n", AVG_RUN);
  
  // Set the resolution to 12 bits (0-4095)
  analogReadResolution(12);
  
  // start process
  pinMode(A0, INPUT);
  Serial.println("setup() complete, compare measured volts with dvm measure");
}

int sumv = 0;
int minv = 50000;
int maxv = 0;
int count = AVG_RUN-1;
float divisor = 1000.0*AVG_RUN;

void loop() {
  int inmv = analogReadMilliVolts(A0);
  sumv = sumv + inmv;
  if (minv > inmv) minv = inmv;
  if (maxv < inmv) maxv = inmv;
  Serial.printf("  [%d] volts:%.3f\n", AVG_RUN-count, inmv/1000.0);
  if (count) {
    count--;
  } else {
    Serial.printf("\nAverage: volts: %.3f, min: %.3f, max: %.3f\n",  sumv/divisor, minv/1000.0, maxv/1000.0);
    count = AVG_RUN-1;
    sumv = 0;
    minv = 50000;
    maxv = 0;
    Serial.println();
    delay(WAIT_PERIOD);
  }  
  delay(WAIT_PERIOD); 
}	

