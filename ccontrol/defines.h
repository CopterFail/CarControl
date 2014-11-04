/*
 * defines.h
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */

#ifndef DEFINES_H_
#define DEFINES_H_

// Hardware definitions
#define LED_WHITE       23
//#define LED_BLUE 4
#define LED_ARDUINO     13
#define LED_STATUS       6
#define PIN_HORN 		11

#define PIN_SERVO_CAM   20
#define PIN_SERVO_STEER 21
#define PIN_ESC_MOTOR   22

// Features requested
//#define Magnetometer
//#define BatteryMonitorCurrent
//#define GPS
#define HOTT_TELEMETRIE



// Primary channel definitions
#define ROLL        0
#define PITCH       1
#define THROTTLE    2
#define YAW         3

// Axis definitions
#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

// Arbitrary ON/OFF definitions
#define OFF 0
#define ON 1

// Car Mode definitions
#define MODE_NEUTRAL 1
#define MODE_NORMAL  2
#define MODE_BREAK   3
#define MODE_BACKWARD 4

// +- PI normalization macro
#define NORMALIZE(x) do { if ((x) < -PI) (x) += 2 * PI; else if ((x) > PI) (x) -= 2 * PI; } while (0);

#endif /* DEFINES_H_ */
