/*
 * gps.cpp
 *
 *  Created on: 24.11.2014
 *      Author: dab
 */


#include <Arduino.h>

#include "defines.h"
#include "GPS_ublox.h"
#include "sensors.h"

struct tgpsData gpsData;
struct tgpsData gpsHome;
UBLOX ublox;

UBLOX::UBLOX()
{
	UBX_step = 0;
	gpsHome.state = 1;
	gpsHome.sats = 0;
};

// reads "RAW" packet from serial buffer
void UBLOX::read_packet( void )
{
	while (Serial2.available()) {
		data = Serial2.read(); // store single byte from serial buffer into data variable

		switch (UBX_step) {
			case 0:
				if (data == 0xB5) { // UBX sync char 1
					UBX_step++;
				}
			break;
			case 1:
				if (data == 0x62) { // UBX sync char 2
					UBX_step++;
				} else {
					UBX_step = 0; // Restart and try again
				}
			break;
			case 2: // CLASS
				UBX_class = data; // byte containing UBX class

				UBX_CK_A = data; // initial value (also resets the value in new packet)
				UBX_CK_B = data; // initial value (also resets the value in new packet)

				UBX_step++;
			break;
			case 3: // ID
				UBX_id = data; // byte containing UBX id

				UBX_CK_A += data;
				UBX_CK_B += UBX_CK_A;

				UBX_step++;
			break;
			case 4: // PAYLOAD LENGTH LOW byte
				UBX_expected_length = data; // containing LOW byte of payload length

				UBX_CK_A += data;
				UBX_CK_B += UBX_CK_A;

				UBX_step++;
			break;
			case 5: // PAYLOAD LENGTH HIGH byte
				UBX_expected_length += data << 8; // containing HIGH byte of payload length

				UBX_CK_A += data;
				UBX_CK_B += UBX_CK_A;

				if (UBX_expected_length <= sizeof(ubloxMessage)) {
					UBX_step++;
				} else {
					// discard overlong messages
					UBX_step = 0;
				}
			break;
			case 6: // PAYLOAD
				UBX_CK_A += data;
				UBX_CK_B += UBX_CK_A;

				ubloxMessage.raw[UBX_data_length] = data;
				UBX_data_length++;

				if (UBX_data_length >= UBX_expected_length) {
					UBX_data_length = 0;
					UBX_step++;
				}
			break;
			case 7: // FIRST CHECKSUM
				if (UBX_CK_A != data) {
					// checksum failed
					UBX_step = 0;
				} else {
					UBX_step++;
				}
			break;
			case 8: // SECOND CHECKSUM
				if (UBX_CK_B != data) {
					// checksum failed
				} else {
					// checksum OK - process data
					process_data();
				}

				UBX_step = 0; // jump back to start
			break;
		}
	}
};

        // use union to read the binary message
void UBLOX::process_data( void )
{
	if (UBX_class == UBX_CLASS_NAV) {
		if (UBX_id == UBX_ID_POSLLH) {
			gpsData.lat = ubloxMessage.nav_posllh.lat; // +- 90 deg  - degrees multiplied by 10000000
			gpsData.lon = ubloxMessage.nav_posllh.lon; // +- 180 deg - degrees multiplied by 10000000
			gpsData.height = ubloxMessage.nav_posllh.height;
			gpsData.accuracy = ubloxMessage.nav_posllh.hAcc;
			gpsData.fixtime = ubloxMessage.nav_posllh.iTow;
			if( (gpsHome.state == 1) && (gpsData.state == 3) )
			{
				memcpy( &gpsHome, &gpsData, sizeof(tgpsData) );
			}
		} else if (UBX_id == UBX_ID_STATUS) {
			switch (ubloxMessage.nav_status.gpsFix) {
				case 2: // 2D FIX
					gpsData.state = 2;
				break;
				case 3: // 3D FIX
					gpsData.state = 3;
				break;
				default: // NO FIX
					gpsData.state = 1;
				break;
			}
		} else if (UBX_id == UBX_ID_SOL) {
			gpsData.sats = ubloxMessage.nav_sol.numSV;
		} else if (UBX_id == UBX_ID_VELNED) {
			gpsData.course = ubloxMessage.nav_velned.heading / 100; // 10E-5 to millidegrees
			gpsData.speed = ubloxMessage.nav_velned.gSpeed;
		} else {
			// ID wasn't defined above, print out ID for debugging purposes
			// Serial.println(UBX_id, HEX);
		}
	}

	// if GPS wasn't detected, flip the bit
	if ((sensors.sensors_detected & GPS_DETECTED) == false) {
		sensors.sensors_detected |= GPS_DETECTED;
	}
};

void SensorArray::readGPS()
{
    ublox.read_packet();
}


