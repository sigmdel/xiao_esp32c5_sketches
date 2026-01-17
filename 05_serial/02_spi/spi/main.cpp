/*
 *  See spi.ino for license and attribution.
 */

#include <Arduino.h>
#include <SPI.h>

//////// User configuration //////
///
/// If defined the SPI clock is divided by 8
///#define CHANGE_CLOCK_DIVIDER
///
/// Rate of USB to Serial chip if used on the development board.
/// This is ignored when the native USB peripheral of the 
/// ESP SoC is used.
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
  delay(2000);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.println();
  #endif  

  Serial.println("\n\nProject: SPI Loopback Test");
  Serial.printf("  Board: %s\n\n", TITLE);

  Serial.println("Initializing SPI port");
  Serial.printf("  MOSI (output pin): %d - connected to MISO (D9)\n", MOSI);
  Serial.printf("  MISO (input pin):  %d - connected to MOSI (D10) initially\n", MISO);
  Serial.printf("  SCK  (clock):      %d - not connected\n", SCK);
  Serial.printf("  SS   (select):     %d - not connected\n", SS);

  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);                // disable chip select
  SPI.begin();
  Serial.printf("Default SPI clock divider: %lu\n", (unsigned long)SPI.getClockDivider());
  #ifdef CHANGE_CLOCK_DIVIDER
  SPI.setClockDivider(SPI_CLOCK_DIV8);   //divide the clock by 8
  Serial.printf("Set SPI clock divider to %lu (SPI_CLOCK_DIV8)\n", (unsigned long)SPI_CLOCK_DIV8);
  #endif
  Serial.println("\nIt is necessary to reset the board after the firmware is uploaded.");
  Serial.println(" Otherwise, it will seem as if the test fails.");
  Serial.println("\nsetup() completed, starting loop().");
 }

#define BUFFER_SIZE  32

char outBuffer[BUFFER_SIZE] = {0};
char inBuffer[BUFFER_SIZE] = {0};
int count = 0;

void loop (void) {
  digitalWrite(SS, LOW); // enable Slave Select
  count++;
  int n = snprintf(outBuffer, BUFFER_SIZE, "Message %d", count);
  Serial.printf("\nTransmitting \"%s\"\n", outBuffer);

  for (int i = 0; i < n; i++) {
    inBuffer[i] = SPI.transfer(outBuffer[i]);
  }
  digitalWrite(SS, HIGH); // disable Slave Select

  Serial.print("Received: \"");
  for (int i=0; i < n; i++) {
    char c = inBuffer[i];
    if ((31 < c) && (c < 127))
      Serial.print(c);
    else
      Serial.printf(" 0x%.2x ", c);
  }
  Serial.println("\"");

  memset(inBuffer, 0xA5, sizeof(inBuffer));
  delay(1000);
}
