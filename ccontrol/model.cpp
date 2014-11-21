
#include <Arduino.h>
#include <math.h>

#include "defines.h"
#include "sensors.h"
#include "PilotCommand.h"

#define T 0.02


// Kinematics variable definitions
float kinematicsAngle[3];
float mod_v, mod_a, mod_ang, mod_dang, mod_f;
int16_t iCarMode = MODE_NEUTRAL;
int16_t iCarLight = 0;


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
	static uint16_t CntNoThrottle = 0;
	mod_a     = accel[XAXIS];
	mod_v    += T * mod_a;
	mod_dang  = gyro[ZAXIS];
	mod_ang  += T * mod_dang; 

	if( mod_ang > M_PI ) mod_ang -= 2 * M_PI;
	else if( mod_ang < -M_PI ) mod_ang += 2 * M_PI;

	mod_f     = (float)icommandThrottle * 0.005;
	// commandSteer

	if( (icommandThrottle > -10)  && (icommandThrottle < 10) )
	{
		if( CntNoThrottle > 40 )
		{
			mod_v = 0.0;
		}
		else
		{
			CntNoThrottle++;
		}
	}
	else
	{
		CntNoThrottle = 0;
	}

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


/**
 * winkel des vector x,y zum x-Einheitsvector
 */
float AngelXY( float x, float y )
{
	float cosa, a;
	int neg = 0;

	if( y< 0 )
	{
		y = -y;
		neg = 1;
	}
	cosa = ( x + y ) / sqrt( x*x + y*y );
	a = acos( cosa );
	if( neg )
	{
		a = -a;
	}
	return a;
}


