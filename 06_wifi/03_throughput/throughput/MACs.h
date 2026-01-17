#pragma once

/*
 * Copyright 2026, Michel Deslierres. No rights reserved, this code is in the public domain.
 * In those jurisdictions where this may be a problem, the BSD Zero Clause License applies.
 * <https://spdx.org/licenses/0BSD.html>
 */
// SPDX-License-Identifier: 0BSD

extern String defaultMAC(void); // CONFIG_SOC_IEEE802154_SUPPORTED is defined it is an 8 byte address else 6 byte
extern String baseMAC(void);    // 6 bytes 
extern String staMAC(void);     // administered address, 6 bytes
extern String softapMAC(void);  // administered address, 6 bytes
extern String btMAC(void);      // administered address, 6 bytes
extern String etherMAC(void);   // administered address, 6 bytes

