/*
 *  See ble_light.ino for license and attribution.
 */

#include <Arduino.h>
#include <BLEDevice.h>

#include "MACs.h"
#include "BLE_UUIDs.h"


////// User configuration //////
///
///  Advertised name of Bluetooth client
#define BLUETOOTH_NAME  "BLE_LIGHT_CLIENT"
///
///  Define to debug scanning BLE devices for wanted service.
///  Was useful when using ESP32 (BLE v4) for ble_switch
///#define DEBUG_FIND_SERVICE
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
#define USE_EXTERNAL_ANTENNA 
///
///  By default LED_PIN is set to BUILTIN_LED when the latter is defined 
///  Some boards such as the XIAO ESP32C3 do not have a builtin LED and the LED_PIN macro 
///  must specify the GPIO pin to which an external LED is connected.
///#define LED_PIN D0
///
///  Define the signal needed to turn the LED on.
///    #define LED_ON LOW  
///  assumes that the diode's anode (+) is connected to HIGH and the cathode (-) is connected to the GPIO pin.
///    #define LED_ON HIGHT
///  assumes the diode's cathode (-) is connected to GND and the anode (+) is connected to the GPIO pin then 
///  Do not forget to insert a current limiting resistor (around 240 ohms depending on the LED and desired
///  brightness) in the circuit.
#define LED_ON LOW
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

#if !defined(LED_PIN) && defined(BUILTIN_LED)
#define LED_PIN BUILTIN_LED
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

const int ledPin = LED_PIN;
const int ledOn = LED_ON;

  // Turn LED off if value = LOW or false or 0   
  // and LED on if value = HIGH or true or any value != 0
void setLed(int value) {
  digitalWrite(ledPin, (value) ? ledOn : 1-ledOn);
  Serial.printf("LED now %s.\n", (value) ? "on" : "off");
}

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;
static BLEUUID serviceUUID(SERVICE_UUID);
static BLEUUID onoffUUID(ON_OFF_UUID);

  // Callback function to handle notifications
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  String payload(pData, length);
  if (payload.equals("on"))
    setLed(HIGH);
  else if (payload.equals("off")) 
    setLed(LOW);
  else  
    Serial.printf("notifyCallback with invalid payload \"%s\"\n", payload.c_str());
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {
    Serial.println("onConnect");   
  }

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.printf("Forming a connection to %s\n", myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  // pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise) *** 23 fine in our case with 4 byte messages max ()"on", "off")

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.printf("Failed to find our service UUID: %s\n", SERVICE_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(onoffUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.printf("Failed to find our characteristic UUID: %s\n", ON_OFF_UUID);
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    Serial.printf("The characteristic value was: %s", value.c_str());
    boolean init = false;
    if (value.equals("on"))
      init = true;
    else if (!value.equals("off"))
      Serial.print(" an invalid value");
    Serial.println();  
    setLed(init);
  }

  if (pRemoteCharacteristic->canNotify()) {
    // Register/Subscribe for notifications
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
      #ifdef DEBUG_FIND_SERVICE
      bool isAdvertising = false;
      bool hasServiceUUID = advertisedDevice.haveServiceUUID();
      Serial.printf("advertisedDevice.haveServiceUUID()=%s\n", (hasServiceUUID) ? "true" : "false");
      if (hasServiceUUID) {
        isAdvertising = advertisedDevice.isAdvertisingService(serviceUUID);
        Serial.printf("advertisedDevice.isAdvertisingService(%s)=%s\n", SERVICE_UUID, (isAdvertising) ? "true" : "false");
      }  
      if (hasServiceUUID && isAdvertising) {     
      #else
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {   
      #endif
      Serial.println("Stopping scan and defining myDevice");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }  // Found our server
  }  // onResult
};  // MyAdvertisedDeviceCallbacks


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

  Serial.println("\n\nProject: ble_light");
  Serial.println("Purpose: Run a remote light (LED) as a Bluetooth client");
  Serial.printf("  Board: %s\n", TITLE);
  #ifdef VARIANT
  Serial.printf("Variant: %s\n", VARIANT);
  #endif
  #ifdef ANTENNA
  Serial.printf("Antenna: %s\n", ANTENNA);
  #endif
  Serial.printf("BLE MAC: %s\n\n", BT_MAC_STR);

  // Set the digital pin connected to the LED as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1-ledOn); // quietly turn it off

  BLEDevice::init(BLUETOOTH_NAME);

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}  // End of setup.

// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (!connected && doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  delay(1000);  // Delay a second between loops.
}  // End of loop
