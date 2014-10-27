/*
    Car controller was initially build to support: 
	- a gyro sensor to detect / correct z-axis rotation
	- an acc sensor to detect forward/backward (LED control, x-axis) and check traction (y-axis)
	- hott or ppm connection from rx
	- telemetry to rx (hott) and minimosd(mavlink)
	- RGB LED (ws2812b), color and intensity
	- switches for power LED
	- 2 servo outputs steering , camera
	- 1 PWM output for ESC
	- switching outputs for horn, power led, etc.
	- battery monitor for telemetry
	- compass and GPS for telemetry
	- speed detection ???
    
    Defines below only enable/disable "pre-defined" hw setups, you can always define your own
    setup in the == Hardware setup == section.
*/

// Arduino standard library imports
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>


// Custom imports
#include "ccontrol.h"
#include "sensors.h"
#include "math.h"
#include "dataStorage.h"


// == Hardware setup/s == 
#define SCT10_SHIELD_V_01

#ifdef SCT10_SHIELD_V_01
    // Led defines
    //#define LED_WHITE 2
    //#define LED_BLUE 4
    #define LED_ARDUINO 13
    #define LED_STATUS 6

    // Features requested
    //#define Magnetometer
    //#define BatteryMonitorCurrent
    //#define GPS
    
    // Critical sensors on board (gyro/accel)
    #include <mpu6050_10DOF_stick_px01.h>
    
    // Magnetometer
    #include <Magnetometer_HMC5883L.h>
    
    // GPS (ublox neo 6m)
    //#include <GPS_ublox.h>
    
    // Current sensor
    #include <BatteryMonitor_current.h>
    
    // Receiver
    //#include <Receiver_teensy3_HW_PPM.h>
    #include <Receiver_teensy3_HW_SUMD.h>

    
    // Motor / ESC / servo
    #include <ESC_teensy3_HW3.h>     

    

#endif

// == END of Hardware setup ==


// Include this last as it contains objects from previous declarations
#include "GPS.h"
#include "PilotCommandProcessor.h"
#include "SerialCommunication.h"  
#include "LED.h"
#include "model.h"

  
void setup() {

    LED_Init();
    LED_SetStatus( YELLOW_LT );

    // Initialize serial communication
    Serial.begin(38400); // Virtual USB Serial on teensy 3.0 is always 12 Mbit/sec (can be initialized with baud rate 0)

#ifdef GPS
    Serial3.begin(38400);
#endif
 
    // Join I2C bus as master
    Wire.begin();

    // I2C bus hardware specific settings
#if defined(__MK20DX128__)
    I2C0_F = 0x00; // 2.4 MHz (prescaler 20)
    I2C0_FLT = 4;
#endif
#if defined(__MK20DX256__)
    //I2C0_F = 0x00; // 2.4 MHz (prescaler 20)
    I2C0_F = 0x1A;	// 400kHz
    I2C0_FLT = 4;
#endif

    
#if defined(__AVR__)
    TWBR = 12; // 400 KHz (maximum supported frequency)
#endif
    
    // Read data from EEPROM to CONFIG union
    readEEPROM();

    // Initialize motors/receivers/sensors
    initializeESC();    
    initializeReceiver();
    
    LED_SetStatus( BLUE_LT );

    //sensors.initializeGyro();
    //sensors.initializeAccel();
    
#ifdef Magnetometer
    sensors.initializeMag();
#endif


    LED_SetStatus( GREEN_LT );

    // All is ready, start the loop
    all_ready = true;
    itterations = 0;
    sensorPreviousTime = micros();
    previousTime = sensorPreviousTime;
    
}

void loop() {   
    // Dont start the loop until everything is ready
    if (!all_ready) return; 
 
    // Used to measure loop performance
    itterations++;
    
    // Timer
    currentTime = micros();
    
    // Read data (not faster then every 2 ms)
    if (currentTime - sensorPreviousTime >= 2000) {
        //sensors.readGyroSum();
        //sensors.readAccelSum();        
    }    
    
    // 100 Hz task loop (10 ms)
    if (currentTime - previousTime > 10000) {
        frameCounter++;
        
        process100HzTask();
        
        // 50 Hz tak (20 ms)
        if (frameCounter % TASK_50HZ == 0) {
            process50HzTask();
        }
        
        // 10 Hz task (100 ms)
        if (frameCounter % TASK_10HZ == 0) {
            process10HzTask();
        }  
        
        // 1 Hz task (1000 ms)
        if (frameCounter % TASK_1HZ == 0) {
            process1HzTask();
        }
        
        // Reset frameCounter back to 0 after reaching 100 (1s)
        if (frameCounter >= 100) {
            frameCounter = 0;
        }
        
        previousTime = currentTime;
    }
}

void process100HzTask() {    
    //sensors.evaluateGyro();
    //sensors.evaluateAccel();
    
#ifdef GPS
    sensors.readGPS();
#endif

    // Listens/read Serial commands on Serial1 interface (used to pass data from configurator)
    readSerial();
    
    // Update kinematics with latest data
    //kinematics_update(gyro[XAXIS], gyro[YAXIS], gyro[ZAXIS], accel[XAXIS], accel[YAXIS], accel[ZAXIS]);
    
}

void process50HzTask() {
    
#ifdef SUMD_IS_ACTIVE  
    ReceiverReadPacket(); // dab 2014-02-01: non interrupt controlled receiver reading  
#endif

    processPilotCommands();
    updateModell_50Hz();

    MotorOut[2] = icommandCam + TX_CENTER;
    MotorOut[3] = icommandSteer + TX_CENTER;
    MotorOut[1] = 1500; // not used
    MotorOut[0] = icommandThrottle + TX_CENTER;;
    updateMotors();
    
    LED_50Hz();
}

void process10HzTask() {
    // Trigger RX failsafe function every 100ms
    RX_failSafe();
    //if(failsafeEnabled) ..
    
#ifdef Magnetometer
    sensors.readMag();
    sensors.evaluateMag();
#endif
    
#ifdef BatteryMonitorCurrent
    readBatteryMonitorCurrent();
#endif   
    
    // Print itterations per 100ms
#ifdef DISPLAY_ITTERATIONS
    Serial.println(itterations);
#endif

    updateModell_10Hz();
    LED_10Hz();
    
    // Reset Itterations
    itterations = 0;    
}

void process1HzTask() {   
 //   LED_1Hz();
 //Serial.println( gyro[ZAXIS] );
}
