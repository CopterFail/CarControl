/*
 * receiver.cpp
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */


#include <Arduino.h>

#include "defines.h"
#include "receiver.h"

volatile uint16_t RX[RX_CHANNELS] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

//#include "Receiver_teensy3_HW_PPM.h"
//#include "Receiver_328p_HW_PPM.h"
#include "Receiver_SUMD.h"

