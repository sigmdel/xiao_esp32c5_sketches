# Arduino-ESP32 Zigbee On/Off Light Example

This example shows how a XIAO ESP32C6 or a XIAO ESP32C5 can be Zigbee end device and how it could be the basis for a remote light or other on/off device in a Zigbee compatible Home Automation (HA) system.

**The original README was renamed [README-org.md](README-org.md)**

---

**Table of Content**
<!-- TOC -->

- [1. Supported Targets](#1-supported-targets)
- [2. Hardware Required](#2-hardware-required)
- [3. Configure the Project](#3-configure-the-project)
- [4. Compiling and Uploading the Sketch](#4-compiling-and-uploading-the-sketch)
  - [4.1. Using Arduino IDE](#41-using-arduino-ide)
  - [4.2. Using PlatformIO](#42-using-platformio)
- [5. Connecting to a Zigbee Coordinator](#5-connecting-to-a-zigbee-coordinator)
  - [5.1. Connecting to a Zigbee2MQTT Coordinator](#51-connecting-to-a-zigbee2mqtt-coordinator)
  - [5.2. Connecting to a Zigbee_On_Off_Switch coordinator](#52-connecting-to-a-zigbee_on_off_switch-coordinator)
  - [5.3. Switching between coordinators](#53-switching-between-coordinators)
  - [5.4. Troubleshooting](#54-troubleshooting)

<!-- /TOC -->
---

## 1. Supported Targets

Currently, this example supports the following targets.

| Supported SoCs | ESP32-C5 |ESP32-C6 | ESP32-H2 |
| --- | --- | --- | --- |
## 2. Hardware Required

* A development board (ESP32-H2, ESP32-C6 or ESP32-C5 based) acting as a Zigbee end device (running the Zigbee_On_Off_Light example).

* A USB cable for power supply and programming

* A compatible Zigbee coordinator such as Zigbee2MQTT **or** another development board (ESP32-H2, ESP32-C5 or ESP32-C6 based) acting as a Zigbee coordinator (running the Zigbee_On_Off_Switch example).

## 3. Configure the Project

The sketch should work as is if uploaded to a [Seeed Studio XIAO ESP32C6](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html) or the soon to be released XIAO ESP23C5. If using a XIAO ESP32C6 with a connected external antenna, please define the `USE_EXTERNAL_ANTENNA` macro near the beginning of the source code in the `///<User settable macros>` section.

The project may work with other development boards based on a supported SoC. There are macros in the `///<User settable macros>` section at the beginning of the source code that can be used to override automatic definitions valid for the XIAO boards.

## 4. Compiling and Uploading the Sketch

### 4.1. Using Arduino IDE

Before Compile/Verify, go to the `Tools` menu to modify the following options.

**XIAO ESP32C6**

* Select the correct **Board** (example: *"XIAO_ESP32C6"*).
* Select the correct **Port** (examples: *"/dev/ttyACM0"* in Linux, *"Com4"* in Windows).
* Set the **Partition Scheme** to *"Zigbee 4MB with spiffs"*.
* Set the **Zigbee mode** *"Zigbee ED (end device)"*.
* Optional: Set **Core Debug Level** to the desired level such as *"Verbose"* (default is *"None"*).
* Optional: Set **Erase All Flash Before Sketch Upload** to *"Enabled"* (default is *"Disabled"*).

![IDE-Tools-screenshot](../tools_config.jpg)

**XIAO ESP32C5**

* Select the correct **Board** (example: *"XIAO_ESP32C5"*).
* Select the correct **Port** (examples: *"/dev/ttyACM0"* in Linux, *"Com4"* in Windows).
* Set the **Partition Scheme** to *"Zigbee 8MB with spiffs"*.
* Set the **Zigbee mode** *"Zigbee ED (end device)"*.
* Optional: Set **Core Debug Level** to the desired level such as *"Verbose"* (default is *"None"*).
* Optional: Set **Erase All Flash Before Sketch Upload** to *"Enabled"* (default is *"Disabled"*).

Same as for the XIAO ESP32C6 except for the bigger partition scheme.

### 4.2. Using PlatformIO

*forthcoming*

## 5. Connecting to a Zigbee Coordinator

Assuming that the Zigbee_On_OFF_Light firmware was uploaded to the development board with **Erase All Flash Before Sketch Upload** set to *"Enabled"*, here are the steps to connect to a coordinator for the first time.

### 5.1. Connecting to a Zigbee2MQTT Coordinator

It is best to ensure that the Zigbee2MQTT coordinator is the nearest Zigbee coordinator to the ESP32 dev board. 

- Open the Zigbee2MQTT web interface (usually at https:[coordinator_ip]:8080/)
- Enable end device connections to the zibgee network by clicking on the **[Permit join (All)]** button in the top menu.
- Power up the development board running the `Zigbee_On_Off_Light` sketch.

After a short while, the board should show up in the Devices table as *Model* `ZBlightBulb`, while the *Manufacturer* is listed as `Unsupported`.

![](../zigbee2mqtt_devices.jpg)

Click on the **Friendly name** so see details about the Zigbee end device and then click on its **Exposes** tab.

![](../zigbee2mqtt_state.jpg)

Clicking on the **OFF** and **ON** buttons in that view controls the LED of the  ESP32 development board running the Zigbee_On_Off_Light sketch. 

Clicking on the boot button of the development board toggles the state of the onboard LED and the State of the end device is updated accordingly in its **Exposes** tab in the Zigbee2MQTT web interface. 

### 5.2. Connecting to a Zigbee_On_Off_Switch coordinator

It is best to ensure that the development board running the Zigbee_On_Off_Switch coordinator firmware is very near to the ESP32 dev board running the Zigbee_On_Off_Light end device firmware. Tests have shown that a Zigbee2MQTT coordinator or a Zigbee router a few metres away will not interfere.

- Power up the development board running the `Zigbee_On_Off_Light` sketch.
- Power up the development board running the `Zigbee_On_Off_Switch` sketch (network joining is enabled by default).

After a short while, the end device (...Light) should have joined the Zigbee network created by the coordinator (...Switch). The serial output of the two devices should make that clear, especially if the **core debug level** was set to `"Verbose"`. In any case, test by pressing the boot button of the coordinator. It should toggle the state of the LED on the end device.

### 5.3. Switching between coordinators

It should not be necessary to erase the flash memory of the end device and then to upload its firmware again in order to connect it to a new coordinator, although that should work.

The easiest way to switch between coordinators is to perform a `Zigbee.factoryReset()` of the end device (...Light) and then to reset the device.

  - A `Zigbee.factoryReset()` of the end device (...Light) is performed when its boot button is depressed for more than 3 seconds.

  - A reset of the end device (...Light) is performed by pressing on its Reset button.

Of course, it is important to ensure that the end device is closer to the desired coordinator than to the other coordinator.

### 5.4. Troubleshooting

**Not Enough Current**

The XIAO may be caught in a *boot loop* which is an indication that not enough current is available to the board. The following messages may be printed over and over again in the serial monitor. 

```
Project: Zigbee On/Off Light
  Board: Seeed XIAO ESP32C5
Antenna: A-01 FPC

Adding ZigbeeLight endpoint to Zigbee Core
```

At the same time the "Not connected..." message flashes at the top of the serial monitor. If the firmware is compiled with the `Core Debug Level` set to `Debug` in the **Tools** menu, a brownout error might be visible when some of the reboots are starting.

This is more likely to occur if the XIAO is connected through an unpowered USB hub. The solution is to connect the XIAO to a different USB port capable of delivering more power. 

**Unable to connect to the Zigbee coordinator**

If the Zigbee coordinator is not in pairing mode and the XIAO has never been connected to it, then the XIAO will not be able to connect to the Zigbee network. The XIAO will endlessly print periods after "." saying that it was connectin to the network.

```
Project: Zigbee On/Off Light
  Board: Seeed XIAO ESP32C5
Antenna: A-01 FPC

Adding ZigbeeLight endpoint to Zigbee Core
Connecting to network
..........................................................................................................................................................................................................
```

If the firware was compiled with e `Core Debug Level` set to `Info` (or `Debug`) then a message indicating that if failed to connect is displayed along with ".".

```
...[  8369][I][ZigbeeCore.cpp:352] esp_zb_app_signal_handler(): Network steering was not successful (status: ESP_FAIL)
...[ 11605][I][ZigbeeCore.cpp:352] esp_zb_app_signal_handler(): Network steering was not successful (status: ESP_FAIL)
...[ 14842][I][ZigbeeCore.cpp:352] esp_zb_app_signal_handler(): Network steering was not successful (status: ESP_FAIL)
```

The solution is to allow the enable pairing in the Zigbee coordinator. Note that pairing is enabled by default in the Zigbee_On_Off_Switch sketch.

**Other hints**

The [original README](README-org.md) has more troubleshooting advice.
