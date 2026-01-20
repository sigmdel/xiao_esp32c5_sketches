// Main module of deep_sleep_tmr.ino
// Copyright: see notice in deep_sleep_tmr.ino

/**************************************************************
OPERATION
  Each time the board wakes up from deep sleep, it executes the
  setup() function. Near the end of the setup() function, the
  board is put into deep sleep. Consequently the loop() function
  is never executed and anything in the setup() function after 
  when deep sleep is enabled will also not be executed.

LED 
  In correct operation, the LED does two things.
  First, as the board wakes from deep sleep, it slowly 
  flashes out the number of times the setup() function
  has been started since the SoC was powered up.
  The the LED quickly flashes for 5 times announcing
  that the SoC is going into sleep mode.
  
NOTE
  The sketch can report the state of the machine to the 
  serial monitor. However, the serial peripheral is shut
  down  when the SoC goes into sleep mode. That means 
  that the serial port has to be reopened when comming
  out of sleep mode. The Arduino IDE automatically 
  reconnects to the serial port quickly enough that 
  the messages from the sketch will be seen. In 
  platformIO it is necessary to manually reconnect 
  to the SoC built-in USB of the SoC so that it will
  not be possible to see the messages. If the board
  uses a USB-serial adapter then this may not be 
  a factor.
*************************************************************/

#include <Arduino.h>

//////// User configuration //////
///
///  Send message to the serial monitor
///  Works well in Arduino IDE, but not in PlaformIO to the 
///  best of knowledge because it is necessary to reconnect 
///  manually to the board.
#define USE_SERIAL
///
/// 15 seccond sleep period in microseconds
#define SLEEP_PERIOD 15000000
///
///  Settle time in milliseconds after esp_deep_sleep_enable_gpio_wakeup(
#define SETTLE_TIME 250
///
///  By default LED_PIN is set to BUILTIN_LED when the latter is defined 
///  For XIAO ESP32C3, this must be define; it does not have a builtin LED
///#define LED_PIN D0
///
///  Define the signal needed to turn on the LED
#define LED_ON LOW 
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

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "Seeed XIAO ESP32C5"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
#elif defined(ARDUINO_LOLIN32_LITE) 
  #define TITLE "WEMOS LOLIN32 Lite"
#else
  #define TITLE "Unknown ESP32 board"
#endif        

#if !defined(LED_PIN) && defined(BUILTIN_LED)
#define LED_PIN BUILTIN_LED
#endif

const int ledPin = LED_PIN;
const int ledOn = LED_ON;

// Counter for restarts stored in non-volatile memory
RTC_DATA_ATTR int bootCount = 0; 


// Flash LED for count times with ms on and off times
void blink(int count=1, int ms=50) {
  for (int i=0; i<count; i++) {
    digitalWrite(ledPin, ledOn);
    delay(ms);
    digitalWrite(ledPin, 1-ledOn);
    delay(ms);
  }
}  


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_GPIO : Serial.println("Wakeup caused by GPIO"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
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

  if (!bootCount) {
    delay(1000);
    Serial.println("\n\nProject: Test waking the SoC from deep sleep with a timer");
    Serial.printf("  Board: %s\n\n", TITLE);
  }

  // Increment boot count
  bootCount++;

  #ifdef USE_SERIAL
  Serial.println("\n\nBoot number: " + String(bootCount));
  print_wakeup_reason();
  #endif

  // Set builtin led pin as an output
  pinMode(ledPin, OUTPUT); 

  // Display bootcount with slow LED blinks
  blink(bootCount, 750); 

  if (esp_sleep_enable_timer_wakeup(SLEEP_PERIOD) == ESP_OK) {
    delay(SETTLE_TIME);
    #ifdef USE_SERIAL
    Serial.printf("Going into sleep mode for %.1f seconds\n", SLEEP_PERIOD/(1000*1000.0));
    #endif
    // Quick LED blinks announcing start of deep sleep
    blink(5);
    esp_deep_sleep_start();
  } else {
    // delay 10 seconds to give time for manual reconnection
    delay(10000);
    Serial.println("Could not enable timer wakeup");
    Serial.println("Will reboot in 10 seconds");
    unsigned long errortimer = millis();
    while (millis() - errortimer < 10000) {
      blink(1);
      delay(100);
    }  
    ESP.restart();
  }  

  Serial.println("This message in setup() should never be printed");
  while (true) {
    blink(5);
    delay(250);
  }  

}

void loop() {
  Serial.println("This is the loop talking. Should never happen.");
  // This odd flashing pattern should never be seen because the 
  // device is put in deep sleep in setup() before loop() starts.
  blink(4, 50);
  delay(125);
  blink(2, 100);
  delay(5000);
}
