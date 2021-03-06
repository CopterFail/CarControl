/*
 * parts are copied from the taulabs project,
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include <Arduino.h>

#include "defines.h"
#include "PilotCommand.h"
#include "sensors.h"
#include "model.h"

#include "HottTelemetrieDef.h"
//#include "HottTelemetrie.h"
int32_t i32HottTelemetrieLoop( uint8_t data );
uint8_t * aui8HottTelemetrieSendBuffer( void );

// Private variables
static uint8_t tx_buffer[HOTT_MAX_MESSAGE_LENGTH];
uint32_t ui32HottCount;


// Private functions
static void uavoHoTTBridgeTask(void *parameters);
static uint16_t build_VARIO_message(struct hott_vario_message *msg);
static uint16_t build_GPS_message(struct hott_gps_message *msg);
static uint16_t build_GAM_message(struct hott_gam_message *msg);
static uint16_t build_EAM_message(struct hott_eam_message *msg);
static uint16_t build_ESC_message(struct hott_esc_message *msg);
static uint16_t build_TEXT_message(struct hott_text_message *msg);
static uint8_t calc_checksum(uint8_t *data, uint16_t size);
static uint8_t generate_warning();
static void update_telemetrydata();
static void convert_long2gps(int32_t value, uint8_t *dir, uword_t *min, uword_t *sec);
static uint8_t scale_float2uint8(float value, float scale, float offset);
static int8_t scale_float2int8(float value, float scale, float offset);
static uword_t scale_float2uword(float value, float scale, float offset);


// timing variables
#define HOTT_IDLE_TIME 9000 //10000  // idle line delay to prevent data crashes on telemetry line.
#define HOTT_DELAY_TIME 3000  // time between 2 transmitted bytes
#define SERIALHOTT Serial3
#define RECEIVING	0
#define SENDING		1



void SerialEventHoTT( void )
{
	static int16_t  i16Size = 0;
	static int16_t  i16Pos = 0;
	static uint32_t ui32Start = 0u;
	static uint32_t ui32Waiting = 0u;
	static uint8_t ui8Mode = RECEIVING;
	uint8_t c;


	if( SERIALHOTT.available() )
	{
		c = SERIALHOTT.read();
		if( ui8Mode == RECEIVING )
		{
			i16Size = i32HottTelemetrieLoop( c );
			if( i16Size > 0 )
			{
				ui32Start = micros();
				ui32Waiting = HOTT_IDLE_TIME;
				i16Pos = 0;
				ui8Mode = SENDING;
			}
		}
		else
		{
			// if "sending" we will receive our sended chars - check this?
			if( i16Pos == 0 )	// receive a char while waiting for idle time ...
			{
				i16Size = i32HottTelemetrieLoop( c );
				ui8Mode = RECEIVING;
			}
		}
	}

	if( (ui32Waiting > 0) && (ui8Mode == SENDING) )
	{
		if ( ( micros() - ui32Start ) >= ui32Waiting )
		{
			if( i16Pos < i16Size )
			{
				c = tx_buffer[i16Pos];
				SERIALHOTT.write( c );
				i16Pos++;
				ui32Start = micros();
				if(i16Pos < i16Size)
				{
					ui32Waiting = HOTT_DELAY_TIME;
				}
				else
				{
					ui32Waiting = 500;
				}
			}
			else
			{
				ui8Mode = RECEIVING;
				ui32Waiting = 0;
				ui32HottCount++;
			}
		}
	}
}





/**
 * Initialise the module
 * \return -1 if initialisation failed
 * \return 0 on success
 */
int32_t i32HottTelemetrieInit(void)
{
	// HoTT telemetry baudrate is fixed to 19200
	SERIALHOTT.begin(19200);
	//HoTTSettingsInitialize();

	// allocate memory for telemetry data
	//telestate = (struct telemetrydata *)PIOS_malloc(sizeof(*telestate));

	ui32HottCount = 0;

	return 0;
}

uint8_t * aui8HottTelemetrieSendBuffer( void )
{
	return tx_buffer;
}

/**
 * Main task. It does not return.
 */
int32_t i32HottTelemetrieLoop(uint8_t data)
{
	static uint8_t rx_buffer[2] = { 0, 0 };
	static uint16_t message_size = 0;
	static uint8_t last_rx_0=0;

	// shift receiver buffer. make room for one byte.
	rx_buffer[1] = rx_buffer[0];
	rx_buffer[0] = data;
	// examine received data stream
	if (rx_buffer[1] == HOTT_BINARY_ID) {
		// first received byte looks like a binary request. check second received byte for a sensor id.
		if( last_rx_0 == rx_buffer[0] )
		{
			rx_buffer[0] = 0;
		}
		last_rx_0 = rx_buffer[0];
		switch (rx_buffer[0]) {
			case HOTT_VARIO_ID:
				message_size = build_VARIO_message((struct hott_vario_message *)tx_buffer);
				break;
			case HOTT_GPS_ID:
				message_size = build_GPS_message((struct hott_gps_message *)tx_buffer);
				break;
			case HOTT_GAM_ID:
				message_size = build_GAM_message((struct hott_gam_message *)tx_buffer);
				break;
			case HOTT_EAM_ID:
				message_size = build_EAM_message((struct hott_eam_message *)tx_buffer);
				break;
			case HOTT_ESC_ID:
				message_size = build_ESC_message((struct hott_esc_message *)tx_buffer);
				break;
			default:
				message_size = 0;
		}
	}
	else if (rx_buffer[1] == HOTT_TEXT_ID) {
		// first received byte looks like a text request. check second received byte for a valid button.
		switch (rx_buffer[0]) {
			case HOTT_BUTTON_DEC:
			case HOTT_BUTTON_INC:
			case HOTT_BUTTON_SET:
			case HOTT_BUTTON_NIL:
			case HOTT_BUTTON_NEXT:
			case HOTT_BUTTON_PREV:
				message_size = build_TEXT_message((struct hott_text_message *)tx_buffer);
				break;
			default:
				message_size = 0;
		}
	}
	else {
		message_size = 0;
	}
	return message_size;
}

/**
 * Build requested answer messages.
 * \return value sets message size
 */
uint16_t build_VARIO_message(struct hott_vario_message *msg)
{
	float deg;

	if (VARIO_SENSOR_DISABLED)
		return 0;

	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_VARIO_ID;
	msg->warning = generate_warning();
	msg->sensor_text_id = HOTT_VARIO_TEXT_ID;

	// alarm inverse bits. invert display areas on limits
	msg->alarm_inverse |= (0) ? VARIO_INVERT_ALT : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_ALT : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_MAX : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_MIN : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR1S : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR1S : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR3S : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR3S : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR10S : 0;
	msg->alarm_inverse |= (0) ? VARIO_INVERT_CR10S : 0;

	// altitude relative to ground
	msg->altitude = scale_float2uword(gyro[ZAXIS]*1000, 1, OFFSET_ALTITUDE);
	msg->min_altitude = scale_float2uword(accel[YAXIS]*1000, 1, OFFSET_ALTITUDE);
	msg->max_altitude = scale_float2uword((float)ui32HottCount, 1, OFFSET_ALTITUDE);

	// climbrate
	msg->climbrate = scale_float2uword(gpsHome.speed, M_TO_CM, OFFSET_CLIMBRATE);
	msg->climbrate3s = scale_float2uword(2, M_TO_CM, OFFSET_CLIMBRATE);
	msg->climbrate10s = scale_float2uword(3, M_TO_CM, OFFSET_CLIMBRATE);

	// compass
	deg = mod_ang * 180.0 / M_PI + 180.0;
	msg->compass = scale_float2int8(deg, DEG_TO_UINT, 0);

	// statusline
	memcpy(msg->ascii, statusline, sizeof(msg->ascii));

	// free display characters
	msg->ascii1 = 'A';
	msg->ascii2 = 'B';
	msg->ascii3 = 'C';

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

uint16_t build_GPS_message(struct hott_gps_message *msg)
{
	float dx,dy,dist;
	float l1,l2,p1,p2,deg;
	uint32_t t1,t2;

	if (GPS_SENSOR_DISABLED)
		return 0;

	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_GPS_ID;
	msg->warning = generate_warning();
	msg->sensor_text_id = HOTT_GPS_TEXT_ID;

	// alarm inverse bits. invert display areas on limits
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_HDIST : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_SPEED : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_SPEED : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_ALT : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_ALT : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_CR1S : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_CR1S : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_CR3S : 0;
	msg->alarm_inverse1 |= (0) ? GPS_INVERT_CR3S : 0;
	msg->alarm_inverse2 |= (0) ? GPS_INVERT2_POS : 0;

	// gps direction, groundspeed and postition
	//msg->flight_direction = scale_float2uint8( mod_ang * 180.0 / M_PI + 180.0, DEG_TO_UINT, 0);
	//msg->flight_direction = scale_float2uint8( magHeadingAbsolute * 180.0 / M_PI + 180.0, DEG_TO_UINT, 0);
	msg->flight_direction = scale_float2uint8( gpsData.course, DEG_TO_UINT / 1000.0, 0);
	msg->gps_speed = scale_float2uword( gpsData.speed, MS_TO_KMH / M_TO_CM, 0);
	convert_long2gps(gpsData.lat, &msg->latitude_ns, &msg->latitude_min, &msg->latitude_sec);
	convert_long2gps(gpsData.lon, &msg->longitude_ew, &msg->longitude_min, &msg->longitude_sec);

	// homelocation distance, course and state
	dx = (gpsData.lon - gpsHome.lon) * 715 / 100000;
	dy = (gpsData.lat - gpsHome.lat) * 1113 / 100000;
	dist = sqrt( dx * dx + dy * dy );

	//l2 = gpsHome.lat * M_PI / 180.0 / 10e7;
	//p2 = gpsHome.lon * M_PI / 180.0 / 10e7;
	//l1 = gpsData.lat * M_PI / 180.0 / 10e7;
	//p1 = gpsData.lon * M_PI / 180.0 / 10e7;
    //deg = acos(sin(p1)*sin(p2)+ cos(p1)*cos(p2)*cos(l2 - l1));
    //deg *= 180.0 / M_PI;

    // math.h, #define TAN_89_99_DEGREES 5729.57795f
    //cf:    *bearing = 9000.0f + atan2f(-dLat, dLon) * TAN_89_99_DEGREES;      // Convert the output radians to 100xdeg
    deg = 90.0f + atan2f(dy, -dx) * 57.2957795f;


	msg->distance = scale_float2uword(dist, 1, 0);
	msg->home_direction = scale_float2uint8(deg, DEG_TO_UINT, 0);
	msg->ascii5 = ( (gpsHome.state>1) ? 'H' : '-');

	// altitude relative to ground and climb rate
	msg->altitude = scale_float2uword(gpsData.height/1000, 1, OFFSET_ALTITUDE);
	msg->climbrate = scale_float2uword(gpsHome.speed, M_TO_CM, OFFSET_CLIMBRATE);
	msg->climbrate3s = scale_float2uint8(0, 1, OFFSET_CLIMBRATE3S);

	// number of satellites,gps fix and state
	msg->gps_num_sat = gpsData.sats;
	msg->gps_fix_char = '0' + gpsData.state;

	msg->ascii6 = 0;

	// model angles
	msg->angle_roll = scale_float2int8(0, DEG_TO_UINT, 0);
	msg->angle_nick = scale_float2int8(0, DEG_TO_UINT, 0);
	deg = mod_ang * 180.0 / M_PI + 180.0;
	msg->angle_compass = scale_float2int8(deg, DEG_TO_UINT, 0);

	// gps time
	t1 = gpsData.fixtime / 1000;  //s
	msg->gps_msec = 0;
	t2 = t1 / 60;  //m
	msg->gps_sec = t1 - t2*60;
	t1 = t2;
	t2 = t1 / 60;  //h
	msg->gps_min = t1 - t2*60;
	t1 = t2;
	t2 = t1 / 24;  //d
	msg->gps_hour = t1 - t2*24;

	// gps MSL (NN) altitude MSL
	msg->msl = scale_float2uword(gpsData.height/1000, 1, 0);

	// free display chararacter
	msg->ascii4 = 'D';

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

uint16_t build_GAM_message(struct hott_gam_message *msg)
{
	if (GAM_SENSOR_DISABLED)
		return 0;

	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_GAM_ID;
	msg->warning = generate_warning();
	msg->sensor_text_id = HOTT_GAM_TEXT_ID;

	// alarm inverse bits. invert display areas on limits
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_CURRENT : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_VOLTAGE : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_VOLTAGE : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_ALT : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_ALT : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_CR1S : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_CR1S : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_CR3S : 0;
	msg->alarm_inverse2 |= (0) ? GAM_INVERT2_CR3S : 0;

	// temperatures
	msg->temperature1 = scale_float2uint8(10, 1, OFFSET_TEMPERATURE);
	msg->temperature2 = scale_float2uint8(20, 1, OFFSET_TEMPERATURE);

	// altitude
	msg->altitude = scale_float2uword(0, 1, OFFSET_ALTITUDE);

	// climbrate
	msg->climbrate = scale_float2uword(gpsHome.speed, M_TO_CM, OFFSET_CLIMBRATE);
	msg->climbrate3s = scale_float2uint8(0, 1, OFFSET_CLIMBRATE3S);

	// main battery
	float voltage = 0;
	float current = 0;
	float energy = 0;
	msg->voltage = scale_float2uword(voltage, 10, 0);
	msg->current = scale_float2uword(current, 10, 0);
	msg->capacity = scale_float2uword(energy, 0.1, 0);

	// pressure kPa to 0.1Bar
	msg->pressure = scale_float2uint8(0, 0.1, 0);

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

uint16_t build_EAM_message(struct hott_eam_message *msg)
{
	if (EAM_SENSOR_DISABLED)
		return 0;

	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_EAM_ID;
	msg->warning = generate_warning();
	msg->sensor_text_id = HOTT_EAM_TEXT_ID;

	// alarm inverse bits. invert display areas on limits
	msg->alarm_inverse1 |= (0) ? EAM_INVERT_CAPACITY : 0;
	msg->alarm_inverse1 |= (0) ? EAM_INVERT_CURRENT : 0;
	msg->alarm_inverse1 |= (0) ? EAM_INVERT_VOLTAGE : 0;
	msg->alarm_inverse1 |= (0) ? EAM_INVERT_VOLTAGE : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_ALT : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_ALT : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_CR1S : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_CR1S : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_CR3S : 0;
	msg->alarm_inverse2 |= (0) ? EAM_INVERT2_CR3S : 0;

	// main battery
	float voltage = 0;
	float current = 0;
	float energy = 0;
	msg->voltage = scale_float2uword(voltage, 10, 0);
	msg->current = scale_float2uword(current, 10, 0);
	msg->capacity = scale_float2uword(energy, 0.1, 0);

	// temperatures
	msg->temperature1 = scale_float2uint8(0, 1, OFFSET_TEMPERATURE);
	msg->temperature2 = scale_float2uint8(0, 1, OFFSET_TEMPERATURE);

	// altitude
	msg->altitude = scale_float2uword(0, 1, OFFSET_ALTITUDE);

	// climbrate
	msg->climbrate = scale_float2uword(gpsHome.speed, M_TO_CM, OFFSET_CLIMBRATE);
	msg->climbrate3s = scale_float2uint8(0, 1, OFFSET_CLIMBRATE3S);

	// flight time
	float flighttime = 5999;
	msg->electric_min = flighttime / 60;
	msg->electric_sec = flighttime - 60 * msg->electric_min;

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

uint16_t build_ESC_message(struct hott_esc_message *msg)
{
	if (ESC_SENSOR_DISABLED)
		return 0;

	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_ESC_ID;
	msg->warning = 0;
	msg->sensor_text_id = HOTT_ESC_TEXT_ID;

	// main batterie
	float voltage = 0;
	float current = 0;
	float max_current = 0;
	float energy = 0;
	msg->batt_voltage = scale_float2uword(voltage, 10, 0);
	msg->current = scale_float2uword(current, 10, 0);
	msg->max_current = scale_float2uword(max_current, 10, 0);
	msg->batt_capacity = scale_float2uword(energy, 0.1, 0);

	// temperatures
	msg->temperatureESC = scale_float2uint8(10, 1, OFFSET_TEMPERATURE);
	msg->max_temperatureESC = scale_float2uint8(0, 1, OFFSET_TEMPERATURE);
	msg->temperatureMOT = scale_float2uint8(20, 1, OFFSET_TEMPERATURE);
	msg->max_temperatureMOT = scale_float2uint8(0, 1, OFFSET_TEMPERATURE);

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

uint16_t build_TEXT_message(struct hott_text_message *msg)
{
	update_telemetrydata();

	// clear message buffer
	memset(msg, 0, sizeof(*msg));

	// message header
	msg->start = HOTT_START;
	msg->stop = HOTT_STOP;
	msg->sensor_id = HOTT_TEXT_ID;

	//memcpy(msg->text[0], "1234567", 8);

	msg->checksum = calc_checksum((uint8_t *)msg, sizeof(*msg));
	return sizeof(*msg);
}

/**
 * update telemetry data
 * this is called on every telemetry request
 * calling interval is 200ms depending on TX
 * 200ms telemetry request is used as time base for timed calculations (5Hz interval)
*/
void update_telemetrydata ()
{
	char *cbuffer1, *cbuffer2;

    switch( icommandModeSwitch )
    {
    default:
    	cbuffer1 = "Manual";
    	break;
    case 2:
    	cbuffer1 = "Gyro";
    	break;
    case 3:
    	cbuffer1 = "Acc";
    	break;
    }

    //snprintf(statusline, sizeof(statusline), "%11s,%8s", cbuffer1, cbuffer2);	// statusline can hold 21 chars incl terminating 0 ... 12 + 1 + 8 + 1 = 22 ; 12->11
    snprintf(statusline, sizeof(statusline), "%11s,%8d", cbuffer1, ui32HottCount);

}

/**
 * generate warning beeps or spoken announcements
*/
uint8_t generate_warning()
{
	//ToDo: set warning tone with hardcoded priority

	// there is no warning
	return 0;
}

/**
 * calculate checksum of data buffer
 */
uint8_t calc_checksum(uint8_t *data, uint16_t size)
{
	uint16_t sum = 0;
	uint16_t i;
	for(i = 0; i < size; i++)
		sum += data[i];
	return sum;
}

/**
 * scale float value with scale and offset to unsigned byte
 */
uint8_t scale_float2uint8(float value, float scale, float offset)
{
	uint16_t temp = (uint16_t)roundf(value * scale + offset);
	uint8_t result;
	result = (uint8_t)temp & 0xff;
	return result;
}

/**
 * scale float value with scale and offset to signed byte (int8_t)
 */
int8_t scale_float2int8(float value, float scale, float offset)
{
	int8_t result = (int8_t)roundf(value * scale + offset);
	return result;
}

/**
 * scale float value with scale and offset to word
 */
uword_t scale_float2uword(float value, float scale, float offset)
{
	uint16_t temp = (uint16_t)roundf(value * scale + offset);
	uword_t result;
	result.l = (uint8_t)temp & 0xff;
	result.h = (uint8_t)(temp >> 8) & 0xff;
	return result;
}

/**
 * convert dword gps value into HoTT gps format and write result to given pointers
 */
void convert_long2gps(int32_t value, uint8_t *dir, uword_t *min, uword_t *sec)
{
	//convert gps decigrad value into degrees, minutes and seconds
	uword_t temp;
	uint32_t absvalue = abs(value);
	uint16_t degrees = (absvalue / 10000000);
	uint32_t seconds = (absvalue - degrees * 10000000) * 6;
	uint16_t minutes = seconds / 1000000;
	seconds %= 1000000;
	seconds = seconds / 100;
	uint16_t degmin = degrees * 100 + minutes;
	// write results
	*dir = (value < 0) ? 1 : 0;
	temp.l = (uint8_t)degmin & 0xff;
	temp.h = (uint8_t)(degmin >> 8) & 0xff;
	*min = temp;
	temp.l = (uint8_t)seconds & 0xff;
	temp.h = (uint8_t)(seconds >> 8) & 0xff;
	*sec = temp;
}

/**
 * @}
 * @}
 */
