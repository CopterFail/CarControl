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

#ifdef SUMD_IS_ACTIVE
#include "Receiver_SUMD.h"
#endif

#ifdef SBUS_IS_ACTIVE
#include "Receiver_SBUS.h"
#endif

//#else
//#include "Receiver_teensy3_HW_PPM.h"
//#include "Receiver_328p_HW_PPM.h"
//#endif

