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

// FlightController commands definitions
extern int16_t icommandSteer, icommandCam, icommandThrottle;
extern int16_t icommandMode, icommandAux, icommandParameter;
extern uint64_t AUX_chan_mask;



void processPilotCommands( void );
