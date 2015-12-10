/*  Pilot / Receiver command handling routine

    Featuring dynamic channel assignment and dynamic auxiliary funtion assignment
    with "side" support for automatic receiver failsafe routines.
    
    Dynamic channel assignment was initially requested by Politt @aeroquad hangout.
    
    Dynamic auxiliary funtion assignment was inspired by very similar feature originally found
    in multiwii flight control software.
*/


// Tx defines:
#define TX_DEAD_BAND 10
#define TX_CENTER 1500

// Fixed channel definitions:
#define CHANNEL_THROTTLE	0
#define CHANNEL_STEER		1
#define CHANNEL_CAM			2
#define CHANNEL_LIGHT		3
#define CHANNEL_CORRECTION	6

#define CHANNEL_SWITCH_MODE 4
#define CHANNEL_SWITCH_LIGHT 5
#define CHANNEL_SWITCH_HORN 7

// FlightController commands definitions
extern int16_t icommandSteer, icommandCam, icommandThrottle;
extern int16_t icommandLight, icommandParameter;
extern int16_t icommandModeSwitch, icommandLightSwitch, icommandHornSwitch;

extern uint64_t AUX_chan_mask; // not longer used

int16_t processPilotCommands( void );
