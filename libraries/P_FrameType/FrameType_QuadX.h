/*
          FRONT
    CW (0)     CCW (1)
    
       O         O
        \       /
         \     /
          \   /
           \ /
  LEFT      #       RIGHT
           / \
          /   \
         /     \
        /       \ 
       O         O 
       
    CCW (3)     CW (2)
           BACK
*/

#define MOTORS 4
uint16_t MotorOut[MOTORS] = {1000, 1000, 1000, 1000}; 

void updateMotorsMix() {
	float throttlebuffer;
    // Limit YAW to +- 200 (20%)
    YawMotorSpeed = constrain(YawMotorSpeed, -200, 200);
    
    // reduce throttle to make room for controller corrections
    throttlebuffer = abs(PitchMotorSpeed) + abs(RollMotorSpeed) + abs(YawMotorSpeed);
    if( (throttle + throttlebuffer) > 2000 ){
    	throttle = 2000 - throttlebuffer;
    	}

    // All of the motor outputs are constrained to standard 1000 - 2000 us PWM
    // Minimum Armed Throttle variable was added to prevent motors stopping during flight (in extreme "stabilization" behaviours)
    MotorOut[0] = constrain(throttle + PitchMotorSpeed + RollMotorSpeed + YawMotorSpeed, CONFIG.data.minimumArmedThrottle, 2000);
    MotorOut[1] = constrain(throttle + PitchMotorSpeed - RollMotorSpeed - YawMotorSpeed, CONFIG.data.minimumArmedThrottle, 2000);
    MotorOut[2] = constrain(throttle - PitchMotorSpeed - RollMotorSpeed + YawMotorSpeed, CONFIG.data.minimumArmedThrottle, 2000);
    MotorOut[3] = constrain(throttle - PitchMotorSpeed + RollMotorSpeed - YawMotorSpeed, CONFIG.data.minimumArmedThrottle, 2000);    
}
