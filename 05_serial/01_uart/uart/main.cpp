/*
 *  See uart.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
///
/// Time between transmission of messages in milliseconds
#define WRITE_INTERVAL  2000  
///
/// Baud used for loopback test. 
/// "UART Controller features: programmable baud rates up to 5 MBaud"
///     ESP32-C5 Series Datasheet v1.0, p.50
/// Select one of the following
#define SERIAL1_BAUD  5242880  
///#define SERIAL1_BAUD   921600 
///#define SERIAL1_BAUD   576000 
///#define SERIAL1_BAUD   460800 
///#define SERIAL1_BAUD   230400 
///#define SERIAL1_BAUD   115200 
///#define SERIAL1_BAUD    76800 
///#define SERIAL1_BAUD    57600 
///#define SERIAL1_BAUD    28800 
///#define SERIAL1_BAUD     9600
///
/// Rate of USB to Serial chip if used on the development board.
/// This is ignored when the native USB peripheral of the 
/// ESP SoC is used.
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

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "Seeed XIAO ESP32C5"
#elif defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "Seeed XIAO ESP32S3"
#elif defined(ESP32)
  #define TITLE "Unknown ESP32 board"
#else  
  #error "An ESP32 SoC required"
#endif        


unsigned long writetimer;

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  


  Serial.println("\n\nProject: UART Loop Back Test");
  Serial.printf("  Board: %s\n\n", TITLE);

  Serial.println("Connect the UART TX (D6) output and the RX (D7) input together.");

  Serial1.begin(SERIAL1_BAUD, SERIAL_8N1, RX, TX);
  while (!Serial1) delay(10);
  Serial.println();
  Serial.printf("Hardware serial port (Serial1) initialized with its baud set to %d.\n", SERIAL1_BAUD);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn yellow LED off
  Serial.println("setup() completed, starting loop().");
  writetimer = millis();
}

int count = 0;

void loop() {
  while (Serial1.available()) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.write(Serial1.read());
    delay(1); // otherwise the flashng LED is hardly visible
    digitalWrite(LED_BUILTIN, HIGH);
  }

  if (millis() - writetimer > WRITE_INTERVAL) {
    count++;
    Serial.printf("\nTransmitting \"Message %d\"\nRx: ", count);
    Serial1.printf("Message %d\n", count);
    writetimer = millis();
  }
}
