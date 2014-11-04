
#include <Arduino.h>

#include "defines.h"
#include "sensors.h"
#include "PilotCommand.h"

#define T 0.05


// Kinematics variable definitions
float kinematicsAngle[3];
float mod_v, mod_a, mod_ang, mod_dang, mod_f;
int16_t iCarMode = MODE_NEUTRAL;


void InitModell( void )
{
	mod_f = 0.0;
	mod_v = 0.0;
	mod_a = 0.0;
	mod_ang = 0.0;
	mod_dang = 0.0;
	
        
}

void updateModell_50Hz( void )
{
	mod_a     = accel[XAXIS];
	mod_v    += T * mod_a;
	mod_dang  = gyro[ZAXIS];
	mod_ang  += T * mod_dang; 
	mod_f     = (float)icommandThrottle * 0.005;
	// commandSteer
}

void updateModell_10Hz( void )
{
    static int16_t iNeutralDelay=0;
    switch( iCarMode )
    {
      case MODE_BREAK:
        if( icommandThrottle >= 0 ) iCarMode = MODE_NEUTRAL;
        break;
      case MODE_BACKWARD:
        if( icommandThrottle >= 0 ) iCarMode = MODE_NEUTRAL;
        break;
      case MODE_NEUTRAL:
        if( icommandThrottle >= TX_DEAD_BAND ) iCarMode = MODE_NORMAL;
        if( icommandThrottle <= -TX_DEAD_BAND ) iCarMode = MODE_BACKWARD;
        break;
      case MODE_NORMAL:
      default:
        if( ( icommandThrottle > -TX_DEAD_BAND ) && ( icommandThrottle < TX_DEAD_BAND ) )
        { 
           if( iNeutralDelay > 5 )
             iCarMode = MODE_NEUTRAL;
           else 
             iNeutralDelay++;
        }
        else
        {
          iNeutralDelay=0;
        }
        if( icommandThrottle <= -TX_DEAD_BAND ) iCarMode = MODE_BREAK;
        break;
    }
}
