/*
 *  See Zigbee_Dimmable_Light.ino for license and attribution.
 */

#include <Arduino.h>
#include "Zigbee.h"

#include "MACs.h"

#ifndef ZIGBEE_MODE_ED
  #error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  GPIO pin used to control LED. Define here to override the
///  automatic definition done below
///#define LED_PIN xx
///
///  Signal level (HIGH or LOW) needed to turn on the LED. 
///  Define here to override the automatic definition done below.
///#define LED_ON xxx 
///
///  GPIO pin used by a push button that is connected to GROUND.
///  Define here to overide the automatic use of the BOOT button.
///#define BOOT_PIN xxxx
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
///
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


#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 4))    
  #error ESP32 Arduino core version 3.3.4 or newer needed
#endif 

#if !defined(CONFIG_SOC_IEEE802154_SUPPORTED)
  #error The SoC must support IEEE 802.15.4 (Zigbee)
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

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "Seeed XIAO ESP32C5"
  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "INTERNAL CERAMIC"
  #endif
#elif defined(ESP32)
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#else  
  #error "An ESP32 SoC required"
#endif        


// Use correct builtin LED
#if defined(RGB_BUILTIN)       // not defined in XIAO_ESP32Cx pins_arduino.h
  #if !defined(LED_PIN)
    #define LED_PIN RGB_BUILTIN
  #endif
  #if !defined(LED_ON)
    #define LED_ON HIGH          // default for other boards in original code
  #endif
#elif defined(LED_BUILTIN)
  #if !defined(LED_PIN)
    #define LED_PIN LED_BUILTIN
  #endif
  #if !defined(LED_ON)
    #define LED_ON LOW          // because I/O pin must be grounded to turn on LED
  #endif
#endif


//---- sanity checks -----------------

#if !defined(LED_PIN)
  #error LED_PIN not defined
#endif

#if !defined(LED_ON)
  #error LED_ON not defined
#endif

#if !defined(BOOT_PIN)
  #error BOOT_PIN not defined
#endif

#if !(ARDUINO_USB_CDC_ON_BOOT > 0) && !defined(SERIAL_BAUD)
  #error SERIAL_BAUD not defined
#endif

//</Configuration> ---------------------

uint8_t led = LED_PIN;
uint8_t button = BOOT_PIN;

/* Zigbee light bulb configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10

ZigbeeDimmableLight zbDimmableLight = ZigbeeDimmableLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLight(bool state, uint8_t level) {
  log_d("setLigth(state=%d, level=%d)", state, level);
  if (!state) {
    pinMode(led, OUTPUT); // back to digital mode to turn off
    digitalWrite(LED_PIN, 1 - LED_ON);
    return;
  }
  // Reverse level if LOW turns the LED on
  analogWrite(LED_PIN, (LED_ON) ? level : 255 - level);
}

// Create a task on identify call to handle the identify function
void identify(uint16_t time) {
  static uint8_t blink = 1;
  log_d("Identify called for %d seconds", time);
  if (time == 0) {
    // If identify time is 0, stop blinking and restore light as it was used for identify
    zbDimmableLight.restoreLight();
    return;
  }
  //rgbLedWrite(led, 255 * blink, 255 * blink, 255 * blink);
  setLight(blink, 255*blink);
  blink = !blink;
}

/********************* Arduino functions **************************/
void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\n     Project: Zigbee Dimmable Light");
  Serial.printf("       Board: %s\n", TITLE);
  Serial.printf("     Antenna: %s\n", ANTENNA);
  Serial.printf("IEEE Address: %s\n\n", ZIGBEE_MAC_STR);

  // Init RMT and leave light OFF
  //rgbLedWrite(led, 0, 0, 0);
  
  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pinMode(led, OUTPUT);
  setLight(LOW, 0);

  // Init button for factory reset
  pinMode(button, INPUT_PULLUP);

  // Set callback function for light change
  zbDimmableLight.onLightChange(setLight);

  // Optional: Set callback function for device identify
  zbDimmableLight.onIdentify(identify);

  // Optional: Set Zigbee device name and model
  zbDimmableLight.setManufacturerAndModel("Espressif", "ZBDimLightBulb");

  // Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeDimmableLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbDimmableLight);

  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nsetup() completed.");
  Serial.println("Modify the onboard LED brightness with short boot button presses.");
  Serial.println("Perform a Zigbee factory reset and SoC reset with a");
  Serial.println("long boot button press that is longer than 3 seconds.");
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    // Increase blightness by 50 every time the button is pressed
    zbDimmableLight.setLightLevel(zbDimmableLight.getLightLevel() + 50);
  }
  delay(100);
}
