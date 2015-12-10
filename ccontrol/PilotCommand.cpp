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

// commands definitions
int16_t icommandSteer, icommandCam, icommandThrottle;
int16_t icommandLight, icommandParameter;
int16_t icommandModeSwitch, icommandLightSwitch, icommandHornSwitch;

uint64_t AUX_chan_mask; // not longer used

int16_t processPilotCommands( void )
{
	int16_t iVal, rVal=0;
    
    // process analog data
    icommandSteer = RX[CHANNEL_STEER] - TX_CENTER;
    icommandCam = RX[CHANNEL_CAM] - TX_CENTER;
    icommandThrottle = RX[CHANNEL_THROTTLE] - TX_CENTER;
    icommandParameter = RX[CHANNEL_CORRECTION] - TX_CENTER;
    icommandLight = RX[CHANNEL_LIGHT] - TX_CENTER;

    // process switch data
    iVal = RX[CHANNEL_SWITCH_MODE] / 630;
    if( icommandModeSwitch != iVal )
    {
    	icommandModeSwitch = iVal;
    	rVal=1;
    }
    iVal = RX[CHANNEL_SWITCH_HORN] > TX_CENTER ? 1 : 0;
    if( icommandHornSwitch != iVal )
    {
    	icommandHornSwitch = iVal;
    	rVal=1;
    }
    iVal = RX[CHANNEL_SWITCH_LIGHT] > TX_CENTER ? 1 : 0;
    if( icommandLightSwitch != iVal )
    {
    	icommandLightSwitch = iVal;
    	rVal=1;
    }

    return rVal;
}    
