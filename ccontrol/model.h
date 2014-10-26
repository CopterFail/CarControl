

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
