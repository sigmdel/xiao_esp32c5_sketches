/*
 *  See wifi_connect.ino for license and attribution.
 */

#include <Arduino.h>  // for Platformio, not needed in the Arduino IDE
#include "WiFi.h"

#include "MACs.h"
#include "secrets.h"

//////// User configuration //////
///
///  Define this when using XIAO ESP32C6 with a connected external antenna 
///#define USE_EXTERNAL_ANTENNA 
///
/// Disconnection time in milliseconds after which an attempt will be
/// made to connect to a network
#define RECONNECT_TIME 5000
///
/// Timeout in milliseconds when trying to connect to a network
#define STOP_CONNECTING 8000
///
/// Time allowed for user input in milliseconds. If the user does not 
/// select a network in that time period, the first network will be
/// chosen by default. The timer is restarted if the user enters an
/// invalid number.
#define CHOICE_TIMEOUT 10000
///
/// Time period in milliseconds between report of WiFi status
#define STATUS_TIME 10000
///
/// If defined the current state is reported along with the WiFi status
/// used to debug state machine
///#define REPORT_STATE
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

// Scand for all network with the given secrets.ssid
// and displayd them. Returns the number of networks found.
// This creates an array of found networks. The user will
// be asked to choose a network in that array.
//
int findNetworks(void) {
  // find all networks in both 2.4 and 5GHz bands (if supported)
  Serial.printf("Searching for networks named %s\n", ssid);
  if (WiFi.setBandMode(WIFI_BAND_MODE_AUTO)) 
    Serial.println("Scanning both the 2.4 and 5GHz WiFi bands");
  else
    Serial.println("Scanning the 2.4GHz WiFi band only, the SoC does not support 5GHz WiFi");   

  int n = WiFi.scanNetworks(false, false, false, 300, 0, ssid);
  if (n) {
    Serial.printf("Found %d networks named %s\n", n, ssid);
    for (int i=0; i<n; i++) {
      int ch = WiFi.channel(i);
      Serial.printf("%d: BSSID: %s, RSSI: %ld dBm, channel: %d, band: %s, encrpytion: ", i+1, WiFi.BSSIDstr(i).c_str(), WiFi.RSSI(i), ch, (ch > 14) ? "5GHz" : "2.4GHz");   
      switch (WiFi.encryptionType(i)) {
          case WIFI_AUTH_OPEN:            Serial.print("open"); break;
          case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
          case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
          case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
          case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
          case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
          case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
          case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
          case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
          default:                        Serial.print("unknown");
      }    
      Serial.println();
    }
  } else {
    Serial.printf("Found no networks named %s\n", ssid);
  }    
  return n;
}

// Prints current Wi-Fi band mode 
void printWiFiBandMode(char msg[]) {
  if (msg) Serial.printf("%s ", msg);
  switch (WiFi.getBandMode()) {
    case WIFI_BAND_MODE_2G_ONLY: Serial.println("2.4 GHz only"); break;
    case WIFI_BAND_MODE_5G_ONLY: Serial.println("5GHz only"); break;
    default: Serial.println("Automatic (2.4 or 5GHz)"); break;    
  }
}   

// The loop() is a state machine. Currently there are 3 states.
// Hopefully their names are self-explanatory
enum state_t {
  NOT_CONNECTED,
  CONNECTING,
  CONNECTED
};

// The current state of the state machine.
// This is modified in the loop().
///
enum state_t state = NOT_CONNECTED;

// Time at which a state was begun
// This will first be updated at the end of the
// setup() function and then whenver the 
// state is change in the loop() function.
unsigned long statetime = 0;

// This will be the index of the network the user wants to connect to
// Networks with the same SSID are distinguished by their BSSID 
//
int networkindex = -1;

// Timer for the printWiFiStatus function
unsigned long lastCheckReport = 0;

// Prints the current WiFi status. Continuously called at the 
// start of the loop() function, it prints out the status
// only if STATUS_TIME has elapsed since the last print out.
//
void printWiFiStatus(void) {
  if (millis() - lastCheckReport < STATUS_TIME) return;

  #ifdef REPORT_STATE
    Serial.printf("state: (%d) ", state);
    switch (state) {
      case NOT_CONNECTED: Serial.println("NOT_CONNECTED"); break;
      case CONNECTING: Serial.println("CONNECTING"); break;
      case CONNECTED: Serial.println("CONNECTED"); break;
      default: Serial.println("**unknown**");
    }
  #endif 

  switch (WiFi.status()) {
    case WL_IDLE_STATUS:     Serial.println("WiFi radio is idle"); break;
    case WL_NO_SSID_AVAIL:   Serial.println("SSID not found"); break;
    case WL_SCAN_COMPLETED:  Serial.println("Scan is completed"); break;
    case WL_CONNECTED:       Serial.printf("WiFi connected to %s, BSSID: %s\n", ssid, WiFi.BSSIDstr().c_str()); break;
    case WL_CONNECT_FAILED:  Serial.print("WiFi Connection failed"); break;
    case WL_CONNECTION_LOST: Serial.println("WiFi Connection was lost"); break;
    case WL_DISCONNECTED:    Serial.println("WiFi is disconnected"); break;
    default:                 Serial.printf("WiFi not connected status: %d\n", WiFi.status());
  }
  lastCheckReport = millis();
}

// Gets the user choice, a digit between 0 and n
//
int userChoice(int n) {
  unsigned long promptime = millis();
  while (millis() - promptime < CHOICE_TIMEOUT) {
    while (Serial.available()) {
      char c = (char)Serial.read();
      Serial.write(c);
      if (c >= ' ') {
        int choice = c - '0';
        if (choice > n) { 
          choice = -1;
          Serial.printf("\n*** must be a digit less than or equal to %d\n", n);
          promptime = millis();
      } else if ((c = '\n') && (choice >= 0))
        return choice;
      }
    }
  }
  Serial.println("\nTimed out, defaulting to 0 (automatic selection)");
  return 0;
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

  Serial.println("\n\nProject: Wi-Fi Connect / Disconnect");
  Serial.printf("  Board: %s\n", TITLE);
  Serial.printf("STA MAC: %s\n", STA_MAC_STR);
  Serial.printf("Antenna: %s\n", ANTENNA);
  printWiFiBandMode((char*)"WiFi Band Mode:");
  Serial.println();
  
  WiFi.STA.begin();
  
  int n = findNetworks();
  if (n < 1) {
    Serial.println("Will reboot in 10 seconds");
    delay(10000);
    ESP.restart();
  }

  networkindex = 0; // assume only one network
  if (n > 1) {
    Serial.println("Connect to which network? (0 for automatic selection)");  
    networkindex = userChoice(n) - 1;
  } 

  Serial.println("\nModule setup done");
  statetime = millis();
}

void loop() {
  printWiFiStatus();

  switch (state) {
    case NOT_CONNECTED: 
      if (millis() - statetime > RECONNECT_TIME) {
        Serial.printf("WiFi disconnected for %d milliseconds or more\n", RECONNECT_TIME);
        if (networkindex < 0) {
          WiFi.setBandMode(WIFI_BAND_MODE_AUTO);
          Serial.printf("Connecting [WiFi.begin(%s, *****) band mode automatic]\n", ssid);
          WiFi.begin(ssid, password);
        } else {
          Serial.printf("Connecting [WiFi.begin(%s, *******, 0, %s)]\n", ssid, WiFi.BSSIDstr(networkindex).c_str());
          WiFi.begin(ssid, password, 0, WiFi.BSSID(networkindex));
        }  
        state = CONNECTING;
        statetime = millis();
      }
      break;

    case CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("WiFi connected to %s, BSSID: %s\n", ssid, WiFi.BSSIDstr().c_str());
        int ch = WiFi.channel();
        Serial.printf("RSSI: %d dBm, channel %d, band %s\n", WiFi.RSSI(), ch, (ch > 14) ? "5GHz" : "2.4GHz"); 
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        state = CONNECTED;
        statetime = millis();
      } else if (millis() - statetime > STOP_CONNECTING) {
        Serial.printf("Not connected after trying for %d milliseconds, disconnecting", STOP_CONNECTING);
        WiFi.disconnect(); //begin("", "");
        state = NOT_CONNECTED;
        statetime = millis();
      }
      break;

    case CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("No longer connected");
        state = NOT_CONNECTED;
        statetime = millis();
      }             
      break;
  }
}  
    

