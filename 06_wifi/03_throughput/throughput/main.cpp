/*
 *  See throughput.ino for license and attribution.
 */

#include <Arduino.h>
#include <WiFi.h>

#include "MACs.h"     // get ESP's MAC addresses
#include "secrets.h"  // Contains WiFi network credentials

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
///  WiFi band (2.4 or 5 GHz) in which to find the specified 
///  Specify one only - only meaningful with ESP32-C5
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

  Serial.println("\n\nProject: Wi-Fi Throughput");
  Serial.println(" Source: https://tutoduino.fr/en/esp32-wifi-performance/");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.print("STA MAC: ");
  Serial.println(staMAC());
  Serial.printf("Antenna: %s\n\n", ANTENNA);

  // Connect to WiFi access point
  WiFi.mode(WIFI_STA);
  if (USE_BAND_MODE != WiFi.getBandMode()) {
    printWiFiBandMode((char*)"Switching to WiFi Band Mode:", USE_BAND_MODE);
    if (!WiFi.setBandMode(USE_BAND_MODE))  // this may fail with SoCs other than ESP32-C5
      Serial.println("Failed");  
    printWiFiBandMode((char*)"WiFi Band Mode set to:", WiFi.getBandMode()); // report the actual mode
  }   
  delay(10);
  Serial.printf("Connecting to wireless network %s\n", ssid);
  WiFi.begin(ssid, password);
  // Wait until connected (blocking loop)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
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

