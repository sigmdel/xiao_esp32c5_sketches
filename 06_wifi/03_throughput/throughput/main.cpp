/*
 *  See throughput.ino for license and attribution.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "MACs.h"
#include "secrets.h"

/////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  WiFi band (2.4 or 5 GHz) in which to find the specified 
///  Specify one at most - only meaningful with ESP32-C5
///  Not necessary if a BSSID is specified in the secrets.h file. However
///  if the BSSID is specified and the USE_BAND_MODE is set to xxxx_ONLY
///  they must be compatible.
///#define USE_BAND_MODE  WIFI_BAND_MODE_2G_ONLY
#define USE_BAND_MODE  WIFI_BAND_MODE_5G_ONLY
///#define USE_BAND_MODE  WIFI_BAND_MODE_AUTO

///  Set this macro to the host name or ip address of the HTML server
///  from which a large file will be downloaded in speed test
#define SERVER_IP   "192.168.1.17"
///
///  Specify the TCP port used with the HTTP server
#define SERVER_PORT 9099
///
///  Relative path of the large file to download. Here / represents
///  the directory in which the HTML server is started
#define FILEPATH   "/data"
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

#if (ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 3, 6))    
  #error ESP32 Arduino core version 3.3.6 or newer needed
#endif

//---- Identify the ESP32 board and antenna ----

#if defined(ARDUINO_XIAO_ESP32C5)
  #define TITLE "XIAO ESP32C5"
  #define ANTENNA "A-01 FPC"
#elif defined(ARDUINO_XIAO_ESP32C6)
  // The onboard ceramic antenna is used by default.
  #define TITLE "XIAO ESP32C6"
  #ifdef USE_EXTERNAL_ANTENNA 
    #define ANTENNA "EXTERNAL"
  #else
    #define ANTENNA "INTERNAL CERAMIC"
  #endif
#else
  #define TITLE "Unknown ESP32 board"
  #define ANTENNA "Unknown"
#endif  

/*
  Convert a BSSID string such as "12:34:56:78:9A:BC" into 6 byte array .
  Returns true if *arr is filled with a valid MAC address such as {0x12, 0x34, 0x56, 0x78, 0x9A, BC}.
  Return false if bssid is not valid or if it scans to {0, 0, 0, 0, 0, 0}.

  There is no checking that macarr points to a 6 byte array of unsigned char (uint8_t).

  Indeed a type does not seem to be defined 6 byte MAC addresses. From 
   .../framework-arduinoespressif32-libs/esp32c5/include/esp_wifi/include/esp_wifi_types_generic.h
  typedef struct {
    uint8_t bssid[6];                     //**< MAC address of AP *
    uint8_t ssid[33];                     //**< SSID of AP *
    ...
  } wifi_ap_record_t;
*/

boolean bssidStr2bytes(String bssid, uint8_t *macarr) {
  bssid.replace(" ", "");
  if (sscanf(bssid.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &macarr[0], &macarr[1], &macarr[2], &macarr[3], &macarr[4], &macarr[5]) != 6)
    memset(macarr, 0, 6); 

  for (int i = 0; i < 6; i++) {
    if (macarr[i])
      return true; // at least one non zero byte
  }  
  return false; // all zeroes 
}

void printMAC(uint8_t *macarr) {
  for (int i=0; i < 6; i++) {
    Serial.printf("%02X", macarr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

boolean hasBSSID = false;
uint8_t  APmac[6];


// Prints current Wi-Fi band mode 
void printWiFiBandMode(char msg[], wifi_band_mode_t mode ) {
  if (msg) Serial.printf("%s ", msg);
  switch (mode) {
    case WIFI_BAND_MODE_2G_ONLY: Serial.println("2.4 GHz only"); break;
    case WIFI_BAND_MODE_5G_ONLY: Serial.println("5GHz only"); break;
    default: Serial.println("Automatic (2.4 or 5GHz)"); break;    
  }
}


// Global client
NetworkClient client;

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
    //pinMode(WIFI_ANT_CONFIG, OUTPUT);
    digitalWrite(WIFI_ANT_CONFIG, HIGH);
  #endif

  Serial.println("\n\n Project: throughput");
  Serial.println("  Source: https://tutoduino.fr/en/esp32-wifi-performance/");
  Serial.printf("   Board: %s\n", TITLE);
  Serial.printf(" Antenna: %s\n", ANTENNA);
  Serial.printf(" STA MAC: %s\n", STA_MAC_STR);
  Serial.printf(" Network: %s\n", ssid);
 
  hasBSSID = bssidStr2bytes(bssid, &APmac[0]);
  if (hasBSSID) {
    Serial.print("AP BSSID: ");
    printMAC(&APmac[0]);
  } else if (strlen(bssid)) {
    Serial.printf("bssid \"%s\" in secrets.h is invalid and thus ignored\n", bssid);
  }
  Serial.println();

  // Connect to WiFi access point
  WiFi.mode(WIFI_STA);

  #ifdef USE_BAND_MODE
    if (USE_BAND_MODE != WiFi.getBandMode()) {
      printWiFiBandMode((char*)"Switching to WiFi Band Mode:", USE_BAND_MODE);
      if (!WiFi.setBandMode(USE_BAND_MODE))  // this may fail with SoCs other than ESP32-C5
        Serial.println("Failed");  
      printWiFiBandMode((char*)"WiFi Band Mode set to:", WiFi.getBandMode()); // report the actual mode
    }   
    delay(10);
  #endif

  Serial.printf("Connecting to %s access point\n", ssid);
  
  if (hasBSSID) {
    WiFi.begin(ssid, password, 0, &APmac[0]); // or (uint8_t*) &APmac);
  } else {
    WiFi.begin(ssid, password);
  }
  // Wait until connected (blocking loop)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("Connected to WiFi AP %s (%s)\n", ssid, WiFi.BSSIDstr().c_str());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Only run the test if WiFi is still connected
  if (WiFi.status() == WL_CONNECTED) {
    const char* host = SERVER_IP;
    const int port = SERVER_PORT;              // Test server port
    const char* path = FILEPATH;         // Resource to download
    Serial.println("\n\nStarting throughput test...");
    Serial.print("RSSI: "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
    Serial.printf("Connecting to %s:%d to download %s\n", host, port, path);
    
    int totalBytes = 0;
    // Open TCP connection to the server
    int err = client.connect(host, port);
    if (err) {
      Serial.println("Connected to server");
      // Send a minimal HTTP GET request
      client.print(String("GET ") + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
      // Wait for HTTP response headers to arrive
      while (client.connected() && !client.available()) {
        delay(1);
      }
      Serial.println("Reading HTTP headers");
      // Skip HTTP headers (read until end of header line '\n')
      while (client.available() && client.read() != '\n') {
        // just discard header data
      }
      Serial.println("Starting timed download");
      // Read HTTP body data and count bytes

      const int bufsize = 1460;
      uint8_t buffer[bufsize];
      unsigned long startTime = millis();
      while (client.connected() || client.available()) {
          int bytesAvailable = client.available();
          // Read and discard data (we only count bytes)
          int len = client.readBytes(buffer, min(bytesAvailable, bufsize));
          totalBytes += len;
      }
      // Close TCP connection
      client.stop();
      // Compute duration in seconds
      unsigned long endTime = millis();
      unsigned long duration = (endTime - startTime) / 1000;  // seconds
      // Avoid division by zero
      if (duration == 0) duration = 1;
      // Compute throughput in megabit/s : 1 byte = 8 bits ; 1 Mbit = 1000 bits * 1000 bits
      float speedMbitps = (totalBytes * 8.0 / (1000.0 * 1000.0)) / duration;
      Serial.print("Downloaded size: ");
      Serial.print(totalBytes);
      Serial.println(" bytes");
      Serial.print("Duration: ");
      Serial.print(duration);
      Serial.println(" s");
      Serial.print("Estimated throughput: ");
      Serial.print(speedMbitps, 2);
      Serial.println(" Mbit/s");
    } else {
      Serial.printf("Failed to connect to server (result: %d)\n", err); 
    }
  } else {
    Serial.println("WiFi disconnected");
  }
  // Wait 10 seconds before the next test
  Serial.println("Waiting 10 seconds before next test");
  delay(10000);
}
