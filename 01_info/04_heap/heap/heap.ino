/*
 * heap.ino
 * Investigates the ESP32-C5 heap 
 *
 * This is a stub to satisfy the Arduino IDE, the source code is in
 * the file main.cpp in the same directory.
 *
 * This sketch will compile in the Arduino IDE
 *
 *  1- Add https://espressif.github.io/arduino-esp32/package_esp32_index.json
 *     in the Additional Boards Manager URLS in the Preferences window.
 *  2- Install platform esp32 by Espressif version 3.3.6 or newer with the Boards Manager
 *  3- Select the XIAO_ESP32C5 board
 *  4- Enable PSRAM in the Tools menu
 *
 * Michel Deslierres
 * February 5, 2026
 *
 * Copyright 2026, Michel Deslierres. No rights reserved, this code is in the public domain.
 * In those jurisdictions where this may be a problem, the BSD Zero Clause License applies.
 * <https://spdx.org/licenses/0BSD.html>
 */
// SPDX-License-Identifier: 0BSD

/*
 * The ESP32-C5 contains multiple types of RAM:
 * 
 * DRAM (Data RAM) is memory that is connected to CPU's data bus and is 
 *      used to hold data. This is the most common kind of memory 
 *      accessed as a heap.
 *
 * IRAM (Instruction RAM) is memory that is connected to the CPU's 
 *      instruction bus and usually holds executable data only 
 *      (i.e., instructions). If accessed as generic memory, all 
 *      accesses must be aligned to 32-Bit Accessible Memory.
 * 
 * D/IRAM is RAM that is connected to CPU's data bus and instruction 
 *        bus, thus can be used either Instruction or Data RAM.
 *
 * It is also possible to connect external SPI RAM to the ESP32-C5. The 
 * external RAM is integrated into the ESP32-C5's memory map via the 
 * cache, and accessed similarly to DRAM.
 * 
 * All DRAM memory is single-byte accessible, thus all DRAM heaps 
 * possess the MALLOC_CAP_8BIT capability. Users can call 
 * heap_caps_get_free_size(MALLOC_CAP_8BIT) to get the free size of all 
 * DRAM heaps.
 *
 * Source: 'Heap Memory Allocation' 
 * @ https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32c5/api-reference/system/mem_alloc.html
 * 
 * 
 * Other References
 *   'ESP32 – How To Use PSRAM' by Dani Eichhorn 2019-10-09 @ https://thingpulse.com/esp32-how-to-use-psram
 *   'PSRAM & partitions pour les ESP32 dans l’IDE Arduino' 2026-01-28 @ https://tutoduino.fr/psram-partitions-esp32-arduino/
 *   'PSRAM & partitions for ESP32 in Arduino IDE' 2026-01-25 @ https://tutoduino.fr/en/partition-esp32-arduino-en/
 *   'Himem API Example' @ https://github.com/espressif/esp-idf/tree/master/examples/system/himem
 *   'ESP32-PSRAM Library' by Phil Schatzmann (pschatzmann) @ https://github.com/pschatzmann/esp32-psram
 *   'Chapter 23: Using Heap Memory Efficiently' @ https://circuitlabs.net/using-heap-memory-efficiently/
 *   'esp32 heap Memory Management' @ https://blog.csdn.net/abc517789065/article/details/79680214 with the help of translation tool
*/
