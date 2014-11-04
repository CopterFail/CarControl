/*
 * dataStorage.cpp
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */

#include <Arduino.h>
#include <EEPROM.h>
//#include </home/dab/arduino-1.0.5/libraries/EEPROM/EEPROM.h>

#include "defines.h"
#include "dataStorage.h"

CONFIG_union CONFIG;

void initializeEEPROM(void) {
    // Default settings should be initialized here
    CONFIG.data.version = EEPROM_VERSION;
    CONFIG.data.calibrateESC = 0;
    CONFIG.data.minimumArmedThrottle = 1100;

    // Accelerometer
    CONFIG.data.ACCEL_BIAS[XAXIS] = 0;
    CONFIG.data.ACCEL_BIAS[YAXIS] = 0;
    CONFIG.data.ACCEL_BIAS[ZAXIS] = 0;

    // RX
    CONFIG.data.CHANNEL_ASSIGNMENT[0] = 0;
    CONFIG.data.CHANNEL_ASSIGNMENT[1] = 1;
    CONFIG.data.CHANNEL_ASSIGNMENT[2] = 2;
    CONFIG.data.CHANNEL_ASSIGNMENT[3] = 3;
    CONFIG.data.CHANNEL_ASSIGNMENT[4] = 4;
    CONFIG.data.CHANNEL_ASSIGNMENT[5] = 5;
    CONFIG.data.CHANNEL_ASSIGNMENT[6] = 6;
    CONFIG.data.CHANNEL_ASSIGNMENT[7] = 7;
    CONFIG.data.CHANNEL_ASSIGNMENT[8] = 8;
    CONFIG.data.CHANNEL_ASSIGNMENT[9] = 9;
    CONFIG.data.CHANNEL_ASSIGNMENT[10] = 10;
    CONFIG.data.CHANNEL_ASSIGNMENT[11] = 11;
    CONFIG.data.CHANNEL_ASSIGNMENT[12] = 12;
    CONFIG.data.CHANNEL_ASSIGNMENT[13] = 13;
    CONFIG.data.CHANNEL_ASSIGNMENT[14] = 14;
    CONFIG.data.CHANNEL_ASSIGNMENT[15] = 15;

    CONFIG.data.CHANNEL_FUNCTIONS[0] = 0x04; // mode select ("stable mode" is set to trigger on AUX1-HIGH by default)
    CONFIG.data.CHANNEL_FUNCTIONS[1] = 0x00; // baro select
    CONFIG.data.CHANNEL_FUNCTIONS[2] = 0x00; // sonar select
    CONFIG.data.CHANNEL_FUNCTIONS[3] = 0x00; // GPS select

    // This function will only initialize data variables
    // writeEEPROM() needs to be called manually to store this data in EEPROM
}

void writeEEPROM(void)
{
    for (uint16_t i = 0; i < sizeof(struct CONFIG_struct); i++) {
        if (CONFIG.raw[i] != EEPROM.read(i)) {
            // Only re-write new data
            // blocks containing the same value will be left alone
            EEPROM.write(i, CONFIG.raw[i]);
        }
    }
}

void readEEPROM(void)
{
    if (EEPROM.read(0) == 255) {
        // No EEPROM values detected, re-initialize
        initializeEEPROM();
    } else {
        // There "is" data in the EEPROM, read it all
        for (uint16_t i = 0; i < sizeof(struct CONFIG_struct); i++) {
            CONFIG.raw[i] = EEPROM.read(i);
        }

        // Verify version
        if (CONFIG.data.version != EEPROM_VERSION) {
            // Version doesn't match, re-initialize
            initializeEEPROM();
        }
    }
}

