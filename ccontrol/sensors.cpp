/*
 * sensor.cpp
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */

#include <Arduino.h>
#include <Wire.h>
//#include </home/dab/arduino-1.0.5/libraries/Wire/Wire.h>

#include "defines.h"
#include "dataStorage.h"
#include "model.h"

#include "sensors.h"
#include "mpu6050_10DOF_stick_px01.h"
#include "Magnetometer_HMC5883L.h"

float gyro[3];
float accel[3];
float gyro_temperature;

int16_t magRaw[3];
float magHeadingX, magHeadingY;
float magHeadingAbsolute = 0.0;


SensorArray sensors;

void SensorArray::initialize( void )
{
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

    sensors.initializeGyro();
    sensors.initializeAccel();

#ifdef Magnetometer
    sensors.initializeMag();
#endif

}

// I2C stuff
void SensorArray::i2c_write8 (int16_t deviceAddress, uint8_t registerAddress, int16_t registerValue)
{
    // I am using int16_t for dataValue becuase we don't know if we are writing
    // 8 bit signed or unsigned value.

    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    Wire.write(registerValue);
    Wire.endTransmission();
};

int16_t SensorArray::i2c_read16 (int16_t deviceAddress, uint8_t registerAddress) {
    int16_t data;

    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 2);

    data = (Wire.read() << 8) | Wire.read();

    return data;
};
