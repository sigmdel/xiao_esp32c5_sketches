/*
 *  See pin_names.ino for license and attribution.
 */

#include <Arduino.h>

//////// User configuration //////
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
  #error "This program is meant to run on the XIAO ESP32C5 only"
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
void iopins(void) {
  Serial.println("\n\nXIAO ESP32C5 I/O Pin Names and Numbers"); 
  Serial.println("BOOT_PIN defined in esp32-hal.h all others in pins_arduino.h");

  Serial.println("\nThe symbolic name and corresponding I/O number of the 11 digital pins");
  Serial.printf("   D0 = %2d\n", D0);
  Serial.printf("   D1 = %2d\n", D1);
  Serial.printf("   D2 = %2d\n", D2);
  Serial.printf("   D3 = %2d\n", D3);
  Serial.printf("   D4 = %2d\n", D4);
  Serial.printf("   D5 = %2d\n", D5);
  Serial.printf("   D6 = %2d\n", D6);
  Serial.printf("   D7 = %2d\n", D7);
  Serial.printf("   D8 = %2d\n", D8);
  Serial.printf("   D9 = %2d\n", D9);
  Serial.printf("  D10 = %2d\n", D10);

  Serial.println("\nThe symbolic name and corresponding I/O number of the 5 analogue pins (4 are JTAG pads on the back side)");
  Serial.printf("  A0 = %d\n", A0);
  Serial.printf("  A1 = %d [JTAG MTMS]\n", A1);
  Serial.printf("  A2 = %d [JTAG MTDI]\n", A2);
  Serial.printf("  A3 = %d [JTAG MTCK]\n", A3);
  Serial.printf("  A4 = %d [JTAG MTDO]\n", A4);
  
  Serial.println("\nThe symbolic name and corresponding I/O number of the 8 serial pins");
  Serial.printf("    TX = %2d [UART] (=D6)\n", TX);
  Serial.printf("    RX = %2d [UART] (=D7)\n", RX);
  Serial.printf("   SDA = %2d [I2C]  (=D4)\n", SDA);
  Serial.printf("   SCL = %2d [I2C]  (=D5)\n", SCL);
  Serial.printf("    SS = %2d [SPI]  (=D3)\n", SS);
  Serial.printf("  MOSI = %2d [SPI]  (=D10)\n", MOSI);
  Serial.printf("  MISO = %2d [SPI]  (=D9)\n", MISO);
  Serial.printf("   SCK = %2d [SPI]  (=D8)\n", SCK);

  Serial.println("\nBattery"); 
  Serial.printf("  BAT_VOLT_PIN = %d\n", BAT_VOLT_PIN);
  Serial.printf("  BAT_VOLT_PIN_EN = %d\n", BAT_VOLT_PIN_EN);

  Serial.println("\nUser yellow LED");
  Serial.printf("  LED_BUILTIN = %d\n", LED_BUILTIN);
  Serial.printf("  BUILTIN_LED = %d // backward compatibility\n", BUILTIN_LED);

  Serial.println("\nBoot Button ");
  #ifdef BOOT_PIN
  Serial.printf("  BOOT_PIN = %2d (defined in esp32-hal.h)\n", BOOT_PIN);
  #else
  Serial.println(" BOOT_PIN not defined")
  #endif

  Serial.println("\nOther macros"); 
  Serial.printf("  USB_VID = 0x%04x\n", USB_VID);
  Serial.printf("  USB_PID = 0x%04x\n", USB_PID);
  
  Serial.println("\nPackage macro");
  #ifdef ESP32
  Serial.println("  ESP32 defined");
  #else
  Serial.println("  ESP32 not defined"); // should not occur
  #endif

  Serial.println("\nBoard macro");
  Serial.println("  ARDUINO_XIAO_ESP32C5 defined"); // of course, see top of file
  
  Serial.println("\nNative USB support");
  #ifdef ARDUINO_USB_CDC_ON_BOOT
  Serial.printf("  ARDUINO_USB_CDC_ON_BOOT is defined and equal to %d\n", ARDUINO_USB_CDC_ON_BOOT);
  #else
  Serial.println("  ARDUINO_USB_CDC_ON_BOOT is not defined");
  #endif

  Serial.println("\nReverse listing");
  Serial.println("  GPIO  C  Names");
  Serial.println("    0   e   D1");
  Serial.println("    1   e   D0, A0");
  Serial.println("    2   p   A1");
  Serial.println("    3   p   A2");
  Serial.println("    4   p   A3");
  Serial.println("    5   p   A4");
  Serial.println("    6   -   BAT_VOLT_PIN");
  Serial.println("    7   e   D3,  SS   [SPI]");
  Serial.println("    8   e   D8,  SCK  [SPI]");
  Serial.println("    9   e   D9,  MISO [SPI]");
  Serial.println("   10   e   D10, MOSI [SPI]");
  Serial.println("   11   e   D6,  TX   [UART]");
  Serial.println("   12   e   D7,  RX   [UART]");
  Serial.println("   23   e   D4,  SDA  [I2C]");
  Serial.println("   24   e   D5,  SCL  [I2C]");
  Serial.println("   25   e   D2");
  Serial.println("   26   -   BAT_VOLT_PIN_EN");
  Serial.println("   27   -   BUILTIN_LED, LED_BUILTIN");
  Serial.println("   28   -   BOOT_PIN");
  Serial.println("\nC(onnection): e = edge and through-hole, p = pad on reverse side");
}

void setup() {
  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  
}

void loop() {
  iopins();
  delay(10000); // wait 10 seconds
}
