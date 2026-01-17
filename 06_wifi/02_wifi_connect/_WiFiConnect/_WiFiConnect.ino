/*
 * WiFiConnect.ino
 * Connect to a Wi-Fi Network.
 *
 * The Wi-Fi network is on the 2.4GHz the most ESP32 will be able to connect.
 * At this time, if theit is on the 5GHz band the only the ESP32-C5 will be
 * able to connect to the network. Set the USE_BAND to WIFI_BAND_MODE_5G_ONLY
 * and see what happens.
 * 
 * Based on the reference code in the Getting Started with Seeed Studio XIAO ESP32-C5
 * at https://wiki.seeedstudio.com/xiao_esp32c5_wifi_usage/#program
 *
 * Michel Deslierres
 * 17 January, 2026
 */

#include <Arduino.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Which Wi-Fi band should be used
// ***Select only one***
//
//#define USE_BAND  WIFI_BAND_MODE_2G_ONLY
#define USE_BAND  WIFI_BAND_MODE_5G_ONLY
//#define USE_BAND WIFI_BAND_MODE_AUTO

// Prints current Wi-Fi band mode 
void printWiFiBandMode(char msg[], wifi_band_mode_t mode ) {
  if (msg) Serial.printf("%s ", msg);
  switch (mode) {
    case WIFI_BAND_MODE_2G_ONLY: Serial.println("2.4 GHz only"); break;
    case WIFI_BAND_MODE_5G_ONLY: Serial.println("5GHz only"); break;
    default: Serial.println("Automatic (2.4 or 5GHz)"); break;    
  }
}
void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n_WiFiConnect.ino");

    // Explicitly set mode to Station
    WiFi.mode(WIFI_STA);
    printWiFiBandMode("Default WiFi Band Mode:", WiFi.getBandMode());
    printWiFiBandMode("Switching to WiFi Band Mode:", USE_BAND);
    if (!WiFi.setBandMode(USE_BAND))  // this may fail with SoCs other than ESP32-C5
      Serial.println("Failed");
    printWiFiBandMode("WiFi Band Mode set to:", WiFi.getBandMode()); // report the actual mode
     
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nCONNECTED!");
    
    // Print connection details
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS IP: ");
    Serial.println(WiFi.dnsIP());
    
    // C5 Specific: Check which band and channel we are on
    Serial.print("Channel: ");
    Serial.println(WiFi.channel());
    Serial.printf("Wi-Fi Band: %s\n", (WiFi.channel() > 14) ? "5GHz" : "2.4GHz"); 
    Serial.print("RSSI (Signal Strength): ");
    Serial.println(WiFi.RSSI());
}

void loop() {
    // Check if WiFi is still connected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost. Reconnecting...");
        WiFi.disconnect();
        WiFi.reconnect();
    }
    delay(5000);
}
