/*  ESC / Servo signal generation done in hardware by FLEX timer, without ISR (yay!)

    We are using flex timer0 which supports 8 channels.

    Currently generating 400 Hz PWM signal that is fed into electronic speed controllers
    corresponding to each rotor.

    This code will probably be expanded to also generate servo signal for
    gimbal stabilization (hopefully in near future).

    Big thanks to kha from #aeroquad for helping me get this up and running.
   
    == PIN Configuration ==
   
    Channel - PIN name - Teensy 3.0 PIN numbering

    FTM0_CH0 - PTC1 - 22
    FTM0_CH1 - PTC2 - 23
    FTM0_CH2 - PTC3 - 9
    FTM0_CH3 - PTC4 - 10
    FTM0_CH4 - PTD4 - 6
    FTM0_CH5 - PTD5 - 20
    FTM0_CH6 - PTD6 - 21
    FTM0_CH7 - PTD7 - 5   
*/
/*
#define MOTOR_0_VALUE	FTM0_C0V	// PTC1 - 22
#define MOTOR_1_VALUE	FTM0_C1V	// PTC2 - 23
#define MOTOR_2_VALUE	FTM0_C2V	// PTC3 - 9
#define MOTOR_3_VALUE	FTM0_C3V	// PTC4 - 10
#define MOTOR_4_VALUE	FTM0_C4V	// PTD4 - 6
#define MOTOR_5_VALUE	FTM0_C5V	// PTD4 - 20
#define MOTOR_6_VALUE	FTM0_C6V	// PTD6 - 21
#define MOTOR_7_VALUE	FTM0_C7V	// PTD7 - 5

#define MOTOR_0_PORT	PORTC_PCR1
#define MOTOR_1_PORT	PORTC_PCR2
#define MOTOR_2_PORT	PORTC_PCR3
#define MOTOR_3_PORT	PORTC_PCR4
#define MOTOR_4_PORT	PORTD_PCR4
#define MOTOR_5_PORT	PORTD_PCR5
#define MOTOR_6_PORT	PORTD_PCR6
#define MOTOR_7_PORT	PORTD_PCR7
*/
#define MOTOR_0_VALUE	FTM0_C0V	// PTC1 - 22
#define MOTOR_1_VALUE	FTM0_C1V	// PTC2 - 23
#define MOTOR_2_VALUE	FTM0_C5V	// PTD4 - 20
#define MOTOR_3_VALUE	FTM0_C6V	// PTD6 - 21
#define MOTOR_4_VALUE	FTM0_C2V	// PTC3 - 9
#define MOTOR_5_VALUE	FTM0_C3V	// PTC4 - 10
#define MOTOR_6_VALUE	FTM0_C4V	// PTD4 - 6
#define MOTOR_7_VALUE	FTM0_C7V	// PTD7 - 5

#define MOTOR_0_PORT	PORTC_PCR1
#define MOTOR_1_PORT	PORTC_PCR2
#define MOTOR_2_PORT	PORTD_PCR5
#define MOTOR_3_PORT	PORTD_PCR6
#define MOTOR_4_PORT	PORTC_PCR3
#define MOTOR_5_PORT	PORTC_PCR4
#define MOTOR_6_PORT	PORTD_PCR4
#define MOTOR_7_PORT	PORTD_PCR7

void setupFTM0() {
    // Flex timer0 configuration
    FTM0_SC = 0x0c;   // TOF=0 TOIE=0 CPWMS=0 CLKS=01 PS=100 (divide by 16)
    
#ifdef ESC_400HZ
    // 400Hz PWM signal
    FTM0_MOD = 7500;
#else
    // 250Hz PWM signal
    FTM0_MOD = 12000;
#endif

FTM0_C0SC = 0x28;

// Initial values (3000 = 1ms)
#if MOTORS == 3
    MOTOR_0_VALUE = 3000;
    MOTOR_1_VALUE = 3000;    
    MOTOR_2_VALUE = 3000;  

    // PORT Configuration
    MOTOR_0_PORT |= 0x400;
    MOTOR_1_PORT |= 0x400; 
    MOTOR_2_PORT |= 0x400;        
#elif MOTORS == 4
    MOTOR_0_VALUE = 3000;
    MOTOR_1_VALUE = 3000;    
    MOTOR_2_VALUE = 3000;  
    MOTOR_3_VALUE = 3000;  

    // PORT Configuration
    MOTOR_0_PORT |= 0x400;
    MOTOR_1_PORT |= 0x400; 
    MOTOR_2_PORT |= 0x400;        
    MOTOR_3_PORT |= 0x400;        
#elif MOTORS == 6
    MOTOR_0_VALUE = 3000;
    MOTOR_1_VALUE = 3000;    
    MOTOR_2_VALUE = 3000;  
    MOTOR_3_VALUE = 3000;  
    MOTOR_4_VALUE = 3000;  
    MOTOR_5_VALUE = 3000;  

    // PORT Configuration
    MOTOR_0_PORT |= 0x400;
    MOTOR_1_PORT |= 0x400; 
    MOTOR_2_PORT |= 0x400;        
    MOTOR_3_PORT |= 0x400;        
    MOTOR_4_PORT |= 0x400;        
    MOTOR_5_PORT |= 0x400;        
#elif MOTORS == 8
    MOTOR_0_VALUE = 3000;
    MOTOR_1_VALUE = 3000;    
    MOTOR_2_VALUE = 3000;  
    MOTOR_3_VALUE = 3000;  
    MOTOR_4_VALUE = 3000;  
    MOTOR_5_VALUE = 3000;  
    MOTOR_6_VALUE = 3000;  
    MOTOR_7_VALUE = 3000;  

    // PORT Configuration
    MOTOR_0_PORT |= 0x400;
    MOTOR_1_PORT |= 0x400; 
    MOTOR_2_PORT |= 0x400;        
    MOTOR_3_PORT |= 0x400;        
    MOTOR_4_PORT |= 0x400;        
    MOTOR_5_PORT |= 0x400;        
    MOTOR_6_PORT |= 0x400;        
    MOTOR_7_PORT |= 0x400;        
#endif
}

void updateMotors() {
#if MOTORS == 3
    MOTOR_0_VALUE = MotorOut[0] * 3;
    MOTOR_1_VALUE = MotorOut[1] * 3;
    MOTOR_2_VALUE = MotorOut[2] * 3;    
#elif MOTORS == 4
    MOTOR_0_VALUE = MotorOut[0] * 3;
    MOTOR_1_VALUE = MotorOut[1] * 3;
    MOTOR_2_VALUE = MotorOut[2] * 3;    
    MOTOR_3_VALUE = MotorOut[3] * 3;    
#elif MOTORS == 6
    MOTOR_0_VALUE = MotorOut[0] * 3;
    MOTOR_1_VALUE = MotorOut[1] * 3;
    MOTOR_2_VALUE = MotorOut[2] * 3;    
    MOTOR_3_VALUE = MotorOut[3] * 3;    
    MOTOR_4_VALUE = MotorOut[4] * 3;    
    MOTOR_5_VALUE = MotorOut[5] * 3;    
#elif MOTORS == 8
    MOTOR_0_VALUE = MotorOut[0] * 3;
    MOTOR_1_VALUE = MotorOut[1] * 3;
    MOTOR_2_VALUE = MotorOut[2] * 3;    
    MOTOR_3_VALUE = MotorOut[3] * 3;    
    MOTOR_4_VALUE = MotorOut[4] * 3;    
    MOTOR_5_VALUE = MotorOut[5] * 3;    
    MOTOR_6_VALUE = MotorOut[6] * 3;    
    MOTOR_7_VALUE = MotorOut[7] * 3;    
#endif
}

void initializeESC() {
    setupFTM0();
    
    if (CONFIG.data.calibrateESC) {
        // Calibration sequence requested
        
        // Signal range TOP maximum
        for (uint8_t motor = 0; motor < MOTORS; motor++) {
            MotorOut[motor] = 2000;
        }
        updateMotors();
        
        // Wait for all ESCs to acknowledge (1 beep)
        delay(5000);
        
        // Signal range BOTTOM minimum
        for (uint8_t motor = 0; motor < MOTORS; motor++) {
            MotorOut[motor] = 1000;
        }
        updateMotors();     

        // Wait for all ESCs to acknowledge (2 + 1 beep)
        delay(4000);        
        
        // Calibration done
        // disabling the calibration flag and updating EEPROM
        CONFIG.data.calibrateESC = 0;
        writeEEPROM();
    }
}
