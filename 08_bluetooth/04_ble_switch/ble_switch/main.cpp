/*
 *  See ble_switch.ino for license and attribution.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "MACs.h"
#include "BLE_UUIDs.h"


////// User configuration //////
///
///  Advertised name of Bluetooth server
#define BLUETOOTH_NAME  "BLE_SWITCH_SERVER"
///
///  Define indicating that the SoC does not support 
//   BLE version 5.0 or better.
///  Define here or in platformio.ini
///#ifdef BLE_V4
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  GPIO pin used by a push button that is connected to GROUND.
///  Define here to overide the automatic use of the BOOT button.
///#define BUTTON_PIN BOOT_PIN
///
///  Rate of USB to Serial chip if used on the development board.
///  This is ignored when the native USB peripheral of the 
///  ESP SoC is used.
#define SERIAL_BAUD 115200
///
///  Time in milliseconds to wait after Serial.begin() in 
///  the setup() function. If not defined, it will be set
///  to 5000 if running in the PlaformIO IDE to manually switch
///  to the serial monitor otherwise to 2000 if a native USB 
///  peripheral is used or 1000 if a USB-serial adpater is used.
///#define SERIAL_BEGIN_DELAY 10000
///
//////////////////////////////////

#if !defined(ESP32)
  #error An ESP32 based board is required
#endif  

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif


//---- sanity checks -----------------

#if !defined(BUTTON_PIN)
  #define BUTTON_PIN BOOT_PIN
#endif

#if !(ARDUINO_USB_CDC_ON_BOOT > 0) && !defined(SERIAL_BAUD)
  #error SERIAL_BAUD not defined
#endif

#if defined(ARDUINO_XIAO_ESP32C6)
  #define TITLE "Seeed XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA
    #define ANTENNA "External antenna"
  #else
    #define ANTENNA "Onboard ceramic"
  #endif
#elif defined(ARDUINO_BOARD)
  #define TITLE ARDUINO_BOARD
#else
  #define TITLE "Unknown ESP32 board"
#endif    

#ifdef BLE_V4
#include <BLE2902.h>
#include <BLE2901.h>
BLE2901 *descriptor_2901 = NULL;
#endif
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

static BLEUUID serviceUUID(SERVICE_UUID);
static BLEUUID onoffUUID(ON_OFF_UUID);

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

const uint8_t buttonPin = BUTTON_PIN;   

void setup() {
  #if !defined(SERIAL_BEGIN_DELAY)
    #if defined(PLATFORMIO)
      #define SERIAL_BEGIN_DELAY 5000    // 5 seconds
    #elif (ARDUINO_USB_CDC_ON_BOOT > 0)
      #define SERIAL_BEGIN_DELAY 2000    // 2 seconds
    #else
      #define SERIAL_BEGIN_DELAY 1000    // 1 second
    #endif
  #endif 

  #if (ARDUINO_USB_CDC_ON_BOOT > 0)
  Serial.begin();
  delay(SERIAL_BEGIN_DELAY);
  #else 
  Serial.begin(SERIAL_BAUD);
  delay(SERIAL_BEGIN_DELAY);
  Serial.println();
  #endif  

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT); //done in .../variants/XIAO_ESP32C6/variant.cpp
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: ble_switch");
  Serial.println("Purpose: Run a remote switch as a Bluetooth server");
  Serial.printf("  Board: %s\n", TITLE);
  #ifdef ANTENNA
  Serial.printf("Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("BLE MAC: %s\n\n", BT_MAC_STR);
   
  // Initialize the push button GPIO pin that will act as a toggle switch
  pinMode(buttonPin, INPUT_PULLUP);


  // 1. Initialize BLE
  BLEDevice::init(BLUETOOTH_NAME);
  
  // 2. Create Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Create Service
  BLEService *pService = pServer->createService(serviceUUID);

  // 4. Create Characteristic (set read/write permissions)
  pCharacteristic = pService->createCharacteristic(onoffUUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
#ifdef BLE_V4
  // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD)
  // Descriptor 2902 is not required when using NimBLE as it is automatically added based on the characteristic properties
  pCharacteristic->addDescriptor(new BLE2902());
  // Adds also the Characteristic User Description - 0x2901 descriptor
  descriptor_2901 = new BLE2901();
  descriptor_2901->setDescription("On Off characteristic description.");
  descriptor_2901->setAccessPermissions(ESP_GATT_PERM_READ);  // enforce read only - default is Read|Write
  pCharacteristic->addDescriptor(descriptor_2901);
#endif

  pCharacteristic->setValue("off"); // Set initial value

  // 5. Start Service
  pService->start();

  // 6. Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);

  // this is from notify.ino
  //    pAdvertising->setScanResponse(false);
  //    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  // which works with ESP32C5/C6 with BLE v5 server but not with ESP32 with BLE v4
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);  // helps with connection with iPhone client (have seen 0x06 also)

  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection to notify on button press...");  
}

// Button control
uint32_t button_time_stamp = 0;                // debouncing control
bool button_state = false;                     // false = released | true = pressed
const uint32_t debouceTime = 250;              // button debouncing time (ms)
bool on_off_state = false;                     // emulate an on off switch, push button toggles between two states

void loop() {
  // The button is used to control a remote on/off device
  // Check if the button has been pressed
  if (digitalRead(buttonPin) == LOW && !button_state) {
    // deals with button debouncing
    button_time_stamp = millis();  // record the time while the button is pressed.
    button_state = true;           // pressed.
    // Serial.printf(".(%s) ", (on_off_state) ? "true" : "false");
  }

  // Onboard User Button is used as a Light toggle switch 
  uint32_t time_diff = millis() - button_time_stamp;
  if (button_state && time_diff > debouceTime && digitalRead(buttonPin) == HIGH) {
    button_state = false;  // released
    // Toggle button is released - toggle the light state
    Serial.println("\nUser button released. Toggling on_off_state");
    on_off_state = !on_off_state;
    //Serial.printf("on_off_state now: %s\n", (on_off_state) ? "true/on" : "false/off");

    pCharacteristic->setValue((on_off_state) ? "on" : "off"); 
    Serial.printf("onoffUUID characteristic: now: %s\n", pCharacteristic->getValue().c_str());
    
    if (deviceConnected) {
      pCharacteristic->notify();
      Serial.println("Notification sent out");
    } else {
      Serial.println("Notification not sent; no client connection");
    }
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }  
}
