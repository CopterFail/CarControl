/*
    Car controller was initially build to support: 
	- a gyro sensor to detect / correct z-axis rotation
	- an acc sensor to detect forward/backward (LED control, x-axis) and check traction (y-axis)
	- hott or ppm connection from rx
	- telemetry to rx (hott) and minimosd(mavlink)?
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
#include <Servo.h>

#include <Wire.h> 	 			// not needed here, but needed to link the complete sketch
#include <Adafruit_NeoPixel.h>  // dto.
#include <EEPROM.h>             // dto.

#include "defines.h"
#include "sensors.h"
#include "dataStorage.h"
#include "receiver.h"
#include "math.h"

// Modulo definitions (integer remainder)
#define TASK_50HZ 2
#define TASK_10HZ 10
#define TASK_1HZ 100

//#include <GPS_ublox.h>
// Current sensor
//#include <BatteryMonitor_current.h>

// Include this last as it contains objects from previous declarations
#include "PilotCommand.h"
#include "SerialCom.h"
#include "LED.h"
#include "model.h"
#ifdef HOTT_TELEMETRIE
#include "HottTelemetrie.h"
#endif
#ifdef SPORT_TELEMETRIE
#include "SPortTelemetrie.h"
#endif
#include "Beeper.h"

Servo servoSteer;
Servo servoCam;
Servo servoEsc;
Beeper beeper;

bool all_ready = false;


void setup() {

    LED_Init();
//    LED_SetStatus( YELLOW_LT );

    // Initialize serial communication
    Serial.begin(38400); // Virtual USB Serial on teensy 3.0 is always 12 Mbit/sec (can be initialized with baud rate 0)

#ifdef GPS
    Serial2.begin(38400);
    pinMode( PIN_BUTTON, INPUT);     
#endif
 
    // Read data from EEPROM to CONFIG union
    readEEPROM();

    // Initialize motors/receivers/sensors
    initializeReceiver();
    
    servoCam.attach( PIN_SERVO_CAM );
    servoSteer.attach( PIN_SERVO_STEER );
    servoEsc.attach( PIN_ESC_MOTOR );
    
//    LED_SetStatus( BLUE_LT );
    sensors.initialize();


#ifdef HOTT_TELEMETRIE
    i32HottTelemetrieInit();
#endif
#ifdef SPORT_TELEMETRIE
    i32SPortTelemetrieInit();
#endif

    beeper.attach( PIN_HORN );
    beeper.ack();

 //   LED_SetStatus( GREEN_LT );

    // All is ready, start the loop
    all_ready = true;
    
}

void loop()
{
	// Main loop variables
	static unsigned long currentTime = 0;
	static unsigned long sensorPreviousTime = micros();;
	static unsigned long previousTime = sensorPreviousTime;
	static uint8_t frameCounter = 0;

    // Dont start the loop until everything is ready
    if (!all_ready) return; 
 
    // Timer
    currentTime = micros();
    
    // Read data (not faster then every ms)
    if ( ( currentTime - sensorPreviousTime ) >= 1000)
    {
        sensors.readGyroSum();
        sensors.readAccelSum();

#ifdef HOTT_TELEMETRIE
        SerialEventHoTT();  // this will be call too often...
#endif
#ifdef SPORT_TELEMETRIE
        SerialEventSPort();  // this will be call too often...
#endif

        sensorPreviousTime = currentTime;
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
    sensors.evaluateGyro();
    sensors.evaluateAccel();
    
#ifdef GPS
    sensors.readGPS();
#endif

    // Listens/read Serial commands on Serial1 interface (used to pass data from configurator)
    readSerial();
    
    // Update kinematics with latest data
    //kinematics_update(gyro[XAXIS], gyro[YAXIS], gyro[ZAXIS], accel[XAXIS], accel[YAXIS], accel[ZAXIS]);
    
}

void process50HzTask()
{
	int16_t iAuxFactor;
	static int16_t iThrottleLimit = 500;
    
#ifdef SUMD_IS_ACTIVE  
    ReceiverReadPacket(); // dab 2014-02-01: non interrupt controlled receiver reading  
#endif
#ifdef SBUS_IS_ACTIVE
    ReceiverReadPacket();
#endif

    if( processPilotCommands() > 0 )
    {
    	beeper.ack();
    }
    updateModell_50Hz();

    // special car commands gyro(2) or acc(3) compensation:
    if( icommandModeSwitch == 2) {
      // primitive, but works:
      iAuxFactor = ( icommandParameter + 500 ) / 4;
      icommandSteer = icommandSteer - iAuxFactor * gyro[ZAXIS];
    }
    else if( icommandModeSwitch >= 3) {

    	// accel[YAXIS] left is positive m/s2

    	iAuxFactor = (int16_t)( accel[YAXIS] * 50 );
    	if( iAuxFactor < 0 ) iAuxFactor = -iAuxFactor;
    	iThrottleLimit = filterSmooth(500 - iAuxFactor, iThrottleLimit, 0.4 );
        iThrottleLimit = constrain( iThrottleLimit, 250, 500 );
    }
    else
    {
    	iThrottleLimit = 500;
    }
    
    // on rx failsafe, stop and pump the lights:
    if(failsafeEnabled)
    {
    	icommandThrottle = 0;
    	iCarLight = (iCarLight+1) & 127;
    }

    // write output data:
    servoCam.write( constrain( icommandCam + TX_CENTER, 1000, 2000 ) );   
    servoSteer.write( constrain( icommandSteer + TX_CENTER, 1000, 2000 ) );
    servoEsc.write( constrain( icommandThrottle + TX_CENTER, TX_CENTER-iThrottleLimit, TX_CENTER+iThrottleLimit ) );
    
    // horn has still no suitable trigger:
    if ( icommandHornSwitch > 0 )
    {
        beeper.beep(5);
    }

    if( icommandLightSwitch )
    {
   		iAuxFactor = ( icommandLight + 500 ) / 4;
   		iCarLight = iAuxFactor;
   		if( iCarLight < 10 )
   		{
   			iCarLight = 0;
   		}
   	}
    else
    {
    	iCarLight = 0;
    }

    LED_50Hz();
    beeper.update();

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

#ifdef GPS
    if( !digitalRead(PIN_BUTTON) )
    {
      if( gpsHome.sats ){
      	gpsHome.state = 1;
	gpsHome.sats = 0;
        beeper.ack();
      }else{
        beeper.nack();
      }
    }
#endif

    updateModell_10Hz();
    LED_10Hz();
}

extern uint8_t ui8SbusStatus;
extern uint32_t u32SbusBadCnt;
extern uint32_t u32SbusGoodCnt;


void process1HzTask()
{
	static uint8_t  sec = 0;
    LED_1Hz();

/*
    Serial.print( "test: ");
    for( int i=0; i<8 ; i++ )
    {
    	Serial.print( RX[i]);
    	Serial.print( " ");
    }
    Serial.println( ui8SbusStatus );
    if(failsafeEnabled)
    	Serial.println( "Failsafe" );
    else
    	Serial.println( "OK" );
*/
/**/
    Serial.print( "sbus: ");
    Serial.print( u32SbusGoodCnt );
    Serial.print( " / ");
    Serial.println( u32SbusBadCnt );

/**/

    if(failsafeEnabled)
    {
    	beeper.nack();
    }
    else if( gpsHome.state < 2)
    {
    	if( (sec & 0xf) == 0); // beeper.ack();
    }
    sec++;
}


