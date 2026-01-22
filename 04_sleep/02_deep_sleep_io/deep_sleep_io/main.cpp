/*
 *  See deep_sleep_io.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
///  Settle time in milliseconds after esp_deep_sleep_enable_gpio_wakeup(
#define SETTLE_TIME 250
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

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 4))    
  #error ESP32 Arduino core version 3.3.4 or newer needed
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


//---- Identify the ESP32-Cx based board -----

#if defined(ARDUINO_NOLOGO_ESP32C3_SUPER_MINI)
  #define TITLE "Nologo ESP32C3 Super Mini"
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "Seeed XIAO ESP32C5"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
#else
  // Can't seem to be able to find a RISC-V macro so...
  #error Add other boards based on ESP32-C family SoC
#endif        


//---- RTC ram data ------------------------------
/*
 * Three interger variables that must store in the reat time
 * clock ram which does not lose its content during deep sleep 
 * and during the wake up process from deep sleep
 * 
 * from esp32/tools/esp32c5-libs/3.3.6-RC1/include/esp_hw_support/include/esp_sleep.h
 *
 * #if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
 * typedef enum {
 *     ESP_GPIO_WAKEUP_GPIO_LOW = 0,
 *     ESP_GPIO_WAKEUP_GPIO_HIGH = 1
 * } esp_deepsleep_gpio_wake_up_mode_t;
 * #endif
 */

// Wake mode, start with ESP_GPIO_WAKEUP_GPIO_HIGH = 1
RTC_DATA_ATTR int wakemode = 0;     

// Counter for restarts stored in non-volatile memory
RTC_DATA_ATTR int bootCount = 0;

// Current index of wake pin
RTC_DATA_ATTR int currentindex = -1; //initially, no gpio pin is active

//---- WAKE PIN DEFINITIONS --------------------------------------
//
// padcount = number of pin/pads that can be used to wake the SoC
// padlables[] array with board labels of the wake pins
// iopins[] corresponding gpio pin number of wake pins
//
//
#if defined(ARDUINO_NOLOGO_ESP32C3_SUPER_MINI)
// only the pins (GPIO0 ~ GPIO5) in VDD3P3_RTC domain can be used to wake up the chip from Deep-sleep mode.
// ESP32-C3 Technical Reference Manual Verson 1.3 2025-05-08 section 5.10.1 Power Supplies of GPIO Pins, p167
const int padcount = 6;
const char *padlabels[padcount] = {"0", "1", "2", "3", "4", "5"};
const int iopins[padcount] = {0, 1, 2, 3, 4, 5};
#endif
//
//
#if defined(ARDUINO_XIAO_ESP32C3)
const int padcount = 4;
const char *padlabels[padcount] = {"D0", "D1", "D2", "D3"};
const int iopins[padcount] = { D0, D1, D2, D3};
#endif
//
//
#if defined(ARDUINO_XIAO_ESP32C5)
const int padcount = 6;
const char *padlabels[padcount] = {"LP_GPIO1 [D0]",  
                                   "LP_GPIO0 [D1]",
                                   "LP_GPIO2 [MTMS])", 
                                   "LP_GPIO3 [MTDI])",
                                   "LP_GPIO4 [MTCK])", 
                                   "LP_GPIO5 [MTDO])"};
const int iopins[padcount] = { D0, D1, A1, A2, A3, A4};
#endif
//
//
#if defined(ARDUINO_XIAO_ESP32C6)
const int padcount = 7;
const char *padlabels[padcount] = {"LP_GPIO0 [D0]",  
                                   "LP_GPIO1 [D1]",
                                   "LP_GPIO2 [D2])",  
                                   "LP_GPIO4 [MTMS]",
                                   "LP_GPIO5 [MTDI]",    
                                   "LP_GPIO6 [MTCK]", 
                                   "LP_GPIO7 [MTDO]"};
const int iopins[padcount] = {A0, A1, A2, 4, 5, 6, 7};
#endif


void nextindex(void) {
  currentindex++;
  if (currentindex >= padcount) 
    currentindex = 0;
 
  if (!currentindex) {
    wakemode = 1 - wakemode;
    Serial.printf("\n*** For this round, wake the SoC from sleep mode by setting the wake pin %s ***\n", (wakemode) ? "HIGH" : "LOW");
  }  

  int wkpin = iopins[currentindex];
  Serial.printf("\nWake the SoC by seting pad %s (%d) %s.\n", padlabels[currentindex], wkpin, (wakemode) ? "HIGH" : "LOW");
  
  pinMode(wkpin, (wakemode) ? INPUT_PULLUP : INPUT_PULLDOWN);
  uint64_t mask = 1ULL << wkpin;
  esp_deep_sleep_enable_gpio_wakeup(mask, (esp_deepsleep_gpio_wake_up_mode_t)wakemode); // ESP_GPIO_WAKEUP_GPIO_HIGH) or _LOW;
  delay(SETTLE_TIME);
  Serial.println("Going to sleep now");
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
  
  if (!bootCount) {
    delay(1000);
    Serial.println("\n\nProject: Test waking the SoC from deep sleep with I/O pin");
    Serial.printf("  Board: %s\n\n", TITLE);
    Serial.println("Instructions:");
    Serial.println("  Set each wake pin either HIGH or LOW as instructed to wake");
    Serial.println("  the board from deep sleep.");
  }

  ++bootCount;
  Serial.println("\n\nBoot number: " + String(bootCount));
  print_wakeup_reason();

  nextindex();
  esp_deep_sleep_start();
  delay(50);
  Serial.println("This will never be printed");
}

void loop() { };
