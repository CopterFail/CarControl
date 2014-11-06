/*  Pilot / Receiver command handling routine

    Featuring dynamic channel assignment and dynamic auxiliary funtion assignment
    with "side" support for automatic receiver failsafe routines.
    
    Dynamic channel assignment was initially requested by Politt @aeroquad hangout.
    
    Dynamic auxiliary funtion assignment was inspired by very similar feature originally found
    in multiwii flight control software.
*/

#include <Arduino.h>
#include "defines.h"
#include "receiver.h"
#include "dataStorage.h"
#include "PilotCommand.h"

// FlightController commands definitions
int16_t icommandSteer, icommandCam, icommandThrottle;
int16_t icommandMode, icommandAux, icommandParameter;


static int16_t TX_roll, TX_pitch, TX_throttle, TX_yaw, TX_AUX1, TX_AUX2, TX_AUX3, TX_AUX4;
static int16_t TX_AUX5, TX_AUX6, TX_AUX7, TX_AUX8, TX_AUX9, TX_AUX10, TX_AUX11, TX_AUX12;
uint64_t AUX_chan_mask;

void processPilotCommands()
{
    // read data into variables
    // Channel assignment variables are loaded from eeprom
    // allowing user to "dynamically" (via configurator) change the rx channel assignment
    // potentionally allowing to "wire" channels from RX to FC completely wrong 
    // (and then fixing them manually in Channel Assigner)
    TX_roll     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[0]];
    TX_pitch    = RX[CONFIG.data.CHANNEL_ASSIGNMENT[1]];
    TX_throttle = RX[CONFIG.data.CHANNEL_ASSIGNMENT[2]];
    TX_yaw      = RX[CONFIG.data.CHANNEL_ASSIGNMENT[3]];
    TX_AUX1     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[4]];
    TX_AUX2     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[5]];
    TX_AUX3     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[6]];
    TX_AUX4     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[7]];
    TX_AUX5     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[8]];
    TX_AUX6     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[9]];
    TX_AUX7     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[10]];
    TX_AUX8     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[11]];
    TX_AUX9     = RX[CONFIG.data.CHANNEL_ASSIGNMENT[12]];
    TX_AUX10    = RX[CONFIG.data.CHANNEL_ASSIGNMENT[13]];
    TX_AUX11    = RX[CONFIG.data.CHANNEL_ASSIGNMENT[14]];
    TX_AUX12    = RX[CONFIG.data.CHANNEL_ASSIGNMENT[15]];
    
    icommandSteer = TX_yaw - TX_CENTER;
    icommandCam = TX_roll - TX_CENTER;
    icommandThrottle = TX_pitch - TX_CENTER;
    icommandParameter = TX_throttle - TX_CENTER;
    icommandMode = TX_AUX1 / 630;
    icommandAux = TX_AUX2 - TX_CENTER;
}    
