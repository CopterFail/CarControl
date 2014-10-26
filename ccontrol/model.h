

#define T 0.05

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
	mod_f     = commandThrottle;
	// commandSteer
}

#define TX_DEAD_BAND 10
#define TX_CENTER 1500
void updateModell_10Hz( void )
{
    int16_t iThrottle = TX_pitch - TX_CENTER;
    switch( iCarMode )
    {
      case MODE_BREAK:
        if( iThrottle >= 0 ) iCarMode = MODE_NEUTRAL;
        break;
      case MODE_BACKWARD:
        if( iThrottle >= 0 ) iCarMode = MODE_NEUTRAL;
        break;
      case MODE_NEUTRAL:
        if( iThrottle >= TX_DEAD_BAND ) iCarMode = MODE_NORMAL;
        if( iThrottle <= -TX_DEAD_BAND ) iCarMode = MODE_BACKWARD;
        break;
      case MODE_NORMAL:
      default:
        if( ( iThrottle > -TX_DEAD_BAND ) && ( iThrottle < TX_DEAD_BAND ) ) iCarMode = MODE_NEUTRAL;
        if( iThrottle <= -TX_DEAD_BAND ) iCarMode = MODE_BREAK;
        break;
    }
}
