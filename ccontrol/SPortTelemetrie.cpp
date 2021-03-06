
#include <Arduino.h>

#include "defines.h"
#include "PilotCommand.h"
#include "sensors.h"
#include "model.h"

#include "SPortTelemetrie.h"

//#define DEBUG_FRSKY_SENSOR_REQUEST
#define debugSerial Serial

#define _FrSkySPort_Serial            Serial3
#define _FrSkySPort_C1                UART2_C1
#define _FrSkySPort_C3                UART2_C3
#define _FrSkySPort_S2                UART2_S2
#define _FrSkySPort_BAUD              57600

short crc;                         // used for crc calc of frsky-packet
boolean waitingForSensorId = false;
uint8_t cell_count = 0;
uint8_t latlong_flag = 0;
uint32_t latlong = 0;

uint8_t nextFLVSS = 0;
uint8_t nextFAS = 0;
uint8_t nextVARIO = 0;
uint8_t nextGPS = 0;
uint8_t nextDefault = 0;

// Scale factor for roll/pitch:
// We need to scale down 360 deg to fit when max value is 256, and 256 equals 362 deg
float scalefactor = 360.0/((362.0/360.0)*256.0);

void FrSkySPort_ProcessSensorRequest(uint8_t sensorId);
void FrSkySPort_UpdateCRC(uint8_t byte);
void FrSkySPort_SendCrc();
void FrSkySPort_SendPackage(uint16_t id, uint32_t value);
void printDebugPackageSend(const char* pkg_name, uint8_t pkg_nr, uint8_t pkg_max);
void FrSkySPort_SendByte(uint8_t byte);
//uint32_t handle_A2_A3_value(uint32_t value);


// ***********************************************************************
int32_t i32SPortTelemetrieInit(void)
{
  _FrSkySPort_Serial.begin(_FrSkySPort_BAUD);
  _FrSkySPort_C3 = 0x10;            // Tx invert
  _FrSkySPort_C1= 0xA0;            // Single wire mode
  _FrSkySPort_S2 = 0x10;           // Rx Invert
  return 0;
}

// ***********************************************************************
void SerialEventSPort(void)
{
  uint8_t data = 0;
  while ( _FrSkySPort_Serial.available()) 
  {
    data =  _FrSkySPort_Serial.read();

    if(data == START_STOP)
    {
      waitingForSensorId = true; 
      continue; 
    }
    if(!waitingForSensorId)
      continue;

    FrSkySPort_ProcessSensorRequest(data);

    waitingForSensorId = false;
  }
}

// ***********************************************************************
uint16_t sendValueFlvssVoltage = 0;
uint16_t sendValueFASCurrent = 0;
uint16_t sendValueFASVoltage = 0;
void FrSkySPort_ProcessSensorRequest(uint8_t sensorId) 
{
  uint32_t temp=0;
  uint32_t temp2=0;
  uint8_t offset;

/* Simulate GPS data for testing...
  gpsData.state=3;
  gpsData.lon=10000000*50;
  gpsData.lat=10000000*5;
  gpsData.course = 100000*123;
  gpsData.speed = 100 * 10;
  gpsData.height = 1000 * 50;
  gpsData.accuracy= 5000;
  gpsData.sats = 7;
*/
  
  switch(sensorId)
  {
#ifdef SENSOR_ID_FLVSS
  case SENSOR_ID_FLVSS:
    {
      printDebugPackageSend("FLVSS", nextFLVSS+1, 3);
      switch(nextFLVSS)
      {
      case 0:
    	// First 2 cells
        offset = 0x00 ;
        temp2 = temp = (0x100) & 0xFFF;
        FrSkySPort_SendPackage(FR_ID_CELLS,(temp2 << 20) | (temp << 8) | offset);  // Battery cell 0,1
        break;
      case 1:    
        // Optional 3 and 4 Cells
        break;
      case 2:    // Optional 5 and 6 Cells
        break;
      case 3:    // Optional 7 and 8 Cells
        break;
      case 4:    // Optional 9 and 10 Cells
        break;
      case 5:    // Optional 11 and 12 Cells
    	break;
      }
      break;
      nextFLVSS++;
      if(nextFLVSS>2)
        nextFLVSS=0;
    }
    break;
#endif
#ifdef SENSOR_ID_VARIO
  case SENSOR_ID_VARIO:
    {
      printDebugPackageSend("VARIO", nextVARIO+1, 2);
      switch(nextVARIO)
      {
      case 0:
        FrSkySPort_SendPackage(FR_ID_VARIO,100 );       // 100 = 1m/s
        break;
      case 1: 
        FrSkySPort_SendPackage(FR_ID_ALTITUDE,100);   // from barometer, 100 = 1m
        break;
      }
      if(++nextVARIO > 1)
        nextVARIO = 0;
    }
    break;
  #endif
  #ifdef SENSOR_ID_FAS
  case SENSOR_ID_FAS:
    {
      printDebugPackageSend("FAS", nextFAS+1, 2);
      // Use average of atleast 2 samples
      if(nextFAS == 0)
      {
        sendValueFASVoltage = 100; //readAndResetAverageVoltage();
        sendValueFASCurrent = 100; //readAndResetAverageCurrent();
      }
      if(sendValueFASVoltage < 1)
        break;
      
      switch(nextFAS)
      {
      case 0:
        FrSkySPort_SendPackage(FR_ID_VFAS,sendValueFASVoltage/10); // Sends voltage as a VFAS value
        break;
      case 1:
        FrSkySPort_SendPackage(FR_ID_CURRENT, sendValueFASCurrent / 10);
        break;
      }
      if(++nextFAS > 1)
        nextFAS = 0;
    }
    break;
#endif
#ifdef SENSOR_ID_GPS
  case SENSOR_ID_GPS:
    {
      printDebugPackageSend("GPS", nextGPS+1, 5);
      switch(nextGPS)
      {
      case 0:        // Sends the ap_longitude value, setting bit 31 high
        if(gpsData.state==3) {
          if(gpsData.lon < 0)
            latlong=((abs(gpsData.lon)/100)*6)  | 0xC0000000;
          else
            latlong=((abs(gpsData.lon)/100)*6)  | 0x80000000;
          FrSkySPort_SendPackage(FR_ID_LATLONG,latlong);
        }
        break;
      case 1:        // Sends the ap_latitude value, setting bit 31 low  
        if(gpsData.state==3) {
          if(gpsData.lat < 0 )
            latlong=((abs(gpsData.lat)/100)*6) | 0x40000000;
          else
            latlong=((abs(gpsData.lat)/100)*6);
          FrSkySPort_SendPackage(FR_ID_LATLONG,latlong);
        }
        break;  
      case 2:
        if(gpsData.state==3) {
          FrSkySPort_SendPackage(FR_ID_GPS_ALT,gpsData.height / 10);   // from GPS,  100=1m
        }
        break;
      case 3:
      // Note: This is sending GPS Speed now
        if(gpsData.state==3) {
          //            FrSkySPort_SendPackage(FR_ID_SPEED,ap_groundspeed *20 );  // from GPS converted to km/h
          FrSkySPort_SendPackage(FR_ID_SPEED,gpsData.speed *20 );  // from GPS converted to km/h
        }
        break;
      case 4:
         // Note: This is sending Course Over Ground from GPS as Heading
         // before we were sending this: FrSkySPort_SendPackage(FR_ID_HEADING,ap_cog * 100); 

        FrSkySPort_SendPackage(FR_ID_GPS_COURSE, gpsData.course / 1000 );   // 10000 = 100 deg
        break;
      }
      if(++nextGPS > 4)
        nextGPS = 0;
    }
    break;    
#endif
#ifdef SENSOR_ID_RPM
  case SENSOR_ID_RPM:
    printDebugPackageSend("RPM", 1, 1);
    //FrSkySPort_SendPackage(FR_ID_RPM,ap_throttle * 200+ap_battery_remaining*2);   //  * 2 if number of blades on Taranis is set to 2 + First 4 digits reserved for battery remaining in %
    break;
    // Since I don't know the app-id for these values, I just use these two "random"
#endif
  case 0x45:
  case 0xC6:
    switch(nextDefault)
    {
    case 0:        // Note: We are using A2 - previously reported analog voltage when connected to Teensy - as Hdop
      //FrSkySPort_SendPackage(FR_ID_ADC2, ap_gps_hdop);
      break;       
    case 1:
      //FrSkySPort_SendPackage(FR_ID_ACCX, fetchAccX());
      break;
    case 2:
      //FrSkySPort_SendPackage(FR_ID_ACCY, fetchAccY());
      break; 
    case 3:
      //FrSkySPort_SendPackage(FR_ID_ACCZ, fetchAccZ());
      break; 
    case 4:
      temp = gpsData.state+1;
      if(gpsHome.state > 1 ) temp++;
      temp *= 100;
      temp += gpsData.sats;
      FrSkySPort_SendPackage(FR_ID_T1,temp);
      break; 
    case 5:
      //FrSkySPort_SendPackage(FR_ID_A3_FIRST, handle_A2_A3_value((ap_roll_angle+180)/scalefactor));
      break;
    case 6:
      //FrSkySPort_SendPackage(FR_ID_A4_FIRST, handle_A2_A3_value((ap_pitch_angle+180)/scalefactor));
      break;
/*
    case 7:
      {
        // 16 bit value: 
        // bit 1: armed
        // bit 2-5: severity +1 (0 means no message)
        // bit 6-15: number representing a specific text
        uint32_t ap_status_value = ap_base_mode&0x01;
        // If we have a message-text to report (we send it multiple times to make sure it arrives even on telemetry glitches)
        if(ap_status_send_count > 0 && ap_status_text_id > 0)
        {
          // Add bits 2-15
          ap_status_value |= (((ap_status_severity+1)&0x0F)<<1) |((ap_status_text_id&0x3FF)<<5);
          ap_status_send_count--;
          if(ap_status_send_count == 0)
          {
             // Reset severity and text-message after we have sent the message
             ap_status_severity = 0; 
             ap_status_text_id = 0;
          }          
        }
        FrSkySPort_SendPackage(FR_ID_T2, ap_status_value); 
      }
      break;
    case 8:
      // Don't send until we have received a value through mavlink
      if(ap_custom_mode >= 0)
      {
        FrSkySPort_SendPackage(FR_ID_FUEL,ap_custom_mode); 
      }
      break;      
*/
    }
    if(++nextDefault > 8)
      nextDefault = 0;
  default: 
#ifdef DEBUG_FRSKY_SENSOR_REQUEST
    debugSerial.print(millis());
    debugSerial.print("\tRequested data for unsupported appId: ");
    debugSerial.print(sensorId, HEX);
    debugSerial.println();      
#endif
    ;
  }
}

/*
uint32_t handle_A2_A3_value(uint32_t value)
{
  return (value *330-165)/0xFF;
}
*/

// ***********************************************************************
void printDebugPackageSend(const char* pkg_name, uint8_t pkg_nr, uint8_t pkg_max)
{
#ifdef DEBUG_FRSKY_SENSOR_REQUEST
  debugSerial.print(millis());
  debugSerial.print("\tCreating frsky package for ");
  debugSerial.print(pkg_name);
  debugSerial.print(" (");
  debugSerial.print(pkg_nr);
  debugSerial.print("/");
  debugSerial.print(pkg_max);
  debugSerial.print(")");
  debugSerial.println();
#endif
}


// ***********************************************************************
void FrSkySPort_SendByte(uint8_t byte)
{

  if(byte == 0x7E)
  {
    _FrSkySPort_Serial.write(0x7D);
    _FrSkySPort_Serial.write(0x5E);
  }
  else if(byte == 0x7D)
  {
    _FrSkySPort_Serial.write(0x7D);
    _FrSkySPort_Serial.write(0x5D);
  }
  else
  {
    _FrSkySPort_Serial.write(byte);
  }
  FrSkySPort_UpdateCRC(byte);
}

void FrSkySPort_UpdateCRC(uint8_t byte)
{
   // CRC update
  crc += byte;         //0-1FF
  crc += crc >> 8;   //0-100
  crc &= 0x00ff;
}

// ***********************************************************************
void FrSkySPort_SendCrc()
{
  _FrSkySPort_Serial.write(0xFF-crc);
  crc = 0;          // CRC reset
}


// ***********************************************************************
void FrSkySPort_SendPackage(uint16_t id, uint32_t value)
{

//  if(MavLink_Connected)
//  {
//    digitalWrite(led,HIGH);
//  }
  _FrSkySPort_C3 |= 32;      //  Transmit direction, to S.Port
  FrSkySPort_SendByte(DATA_FRAME);
  uint8_t *bytes = (uint8_t*)&id;
  FrSkySPort_SendByte(bytes[0]);
  FrSkySPort_SendByte(bytes[1]);
  bytes = (uint8_t*)&value;
  FrSkySPort_SendByte(bytes[0]);
  FrSkySPort_SendByte(bytes[1]);
  FrSkySPort_SendByte(bytes[2]);
  FrSkySPort_SendByte(bytes[3]);
  FrSkySPort_SendCrc();
  _FrSkySPort_Serial.flush();
  _FrSkySPort_C3 ^= 32;      // Transmit direction, from S.Port

//  digitalWrite(led,LOW);
}
