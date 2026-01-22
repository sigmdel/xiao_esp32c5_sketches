/*
 *  See client.ino for license and attribution.
 */
 
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

#include "MACs.h"


//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
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
#elif defined(ARDUINO_XIAO_ESP32C3)
  #define TITLE "Seeed XIAO ESP32C3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ARDUINO_XIAO_ESP32S3)
  #define TITLE "Seeed XIAO ESP32S3"
  #define ANTENNA "V1.2 FPC"
#elif defined(ESP32)
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#else  
  #error "An ESP32 SoC required"
#endif        

// UUID of the Server (must be exactly the same as the Server)
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

// Scanning parameters
const int scanTime = 10; // Scanning duration (seconds)

// Global variables
static BLEAddress serverAddress;        // Address of the found Server
static bool deviceFound = false;        // Whether the target device is found
static BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;
static BLEClient* pClient = nullptr;

// Scan callback: called when a device is discovered
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Check if it contains the service UUID we need
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.print("Found target Server! Name: ");
      Serial.print(advertisedDevice.getName().c_str());
      Serial.print(", Address: ");
      Serial.println(advertisedDevice.getAddress().toString().c_str());

      serverAddress = advertisedDevice.getAddress();
      deviceFound = true;

      // Stop scanning (stop once found)
      BLEDevice::getScan()->stop();
    }
  }
};


// Client connection callback (optional, used to monitor connection status)
class MyClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("Connected to Server successfully!");
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("Disconnected from Server.");
    deviceFound = false;  // Rescan next time
  }
};


// Encapsulate scan + connect logic
void scanAndConnect() {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  // Active scan to get more information
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  Serial.println("Scanning for Server...");
  deviceFound = false;
  pBLEScan->start(scanTime, false);  // Scan for scanTime seconds

  if (!deviceFound) {
    Serial.println("Target Server not found, will retry later.");
    return;
  }

  // Create client and connect after finding the device
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());

  Serial.print("Connecting to ");
  Serial.println(serverAddress.toString().c_str());

  if (!pClient->connect(serverAddress)) {
    Serial.println("Connection failed!");
    return;
  }

  // Get remote service
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find service UUID");
    pClient->disconnect();
    return;
  }
  Serial.println("Service found.");

  // Get remote characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.println("Failed to find characteristic UUID");
    pClient->disconnect();
    return;
  }
  Serial.println("Characteristic found. Ready to communicate!");
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

  #if defined(USE_EXTERNAL_ANTENNA) && defined(ARDUINO_XIAO_ESP32C6)
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\nProject: Bluetooth LE Client");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("BLE MAC: %s\n", BT_MAC_STR);
  Serial.printf("Antenna: %s\n\n", ANTENNA);

  Serial.println("Starting BLE Client...");

  BLEDevice::init("XIAO_Client");  // Client device name

  // Start the first scan
  scanAndConnect();
}

void loop() {
  // If connected successfully, read/write data every 5 seconds
  if (pClient && pClient->isConnected() && pRemoteCharacteristic) {
    // Read characteristic value
    String value = pRemoteCharacteristic->readValue();
    Serial.print("Read value from Server: ");
    Serial.println(value.c_str());

    // Write new data (with response)
    String sendMsg = "Hello from Client @ " + String(millis() / 1000) + "s";
    pRemoteCharacteristic->writeValue(sendMsg.c_str(), true);  // true = require response
    Serial.println("Sent: " + sendMsg);

    delay(5000);
  } 
  else {
    // Not connected or disconnected → rescan
    if (!deviceFound || (pClient && !pClient->isConnected())) {
      Serial.println("Server not connected, rescanning...");
      scanAndConnect();
    }
    delay(1000);
  }
}
