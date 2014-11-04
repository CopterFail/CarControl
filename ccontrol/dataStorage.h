/*  EEPROM based configuration data storage structure

    Whole flight controller configuration is being stored inside an CONFIG structure
    that can be accesed as data union (which is required for easy manipulation with
    the configuration data over serial and easy way of storing this data in EEPROM).
    
    This method allow us to do "smart" memory checking with the new data against
    data stored in EEPROM, which means we doesn't have to re-write whole configuration
    union inside the EEPROM, but we just modify bytes that changed.
    
    This will protect EEPROM from unnecessary writes, extending its lifetime
    (because EEPROM writes are limited, actual number of writes depends on the chip).
*/

#if defined(__MK20DX128__)
    #define EEPROM_SIZE 512
#endif    
#if defined(__MK20DX256__)
    #define EEPROM_SIZE 1024	//ToDo: Check this value, I expect 2048
#endif    


#define EEPROM_VERSION 5

struct AXIS_MAP_struct {
    uint8_t axis1:2;
    uint8_t axis1_sign:1;
    uint8_t axis2:2;
    uint8_t axis2_sign:1;
    uint8_t axis3:2;
    uint8_t axis3_sign:1;
    uint8_t initialized:1;
};

struct __attribute__((packed)) CONFIG_struct {
    uint8_t version;
    bool calibrateESC;
    uint16_t minimumArmedThrottle;
    
    // Sensor axis map
    struct AXIS_MAP_struct GYRO_AXIS_MAP;
    struct AXIS_MAP_struct ACCEL_AXIS_MAP;
    struct AXIS_MAP_struct MAG_AXIS_MAP;
    
    // Accelerometer
    int16_t ACCEL_BIAS[3];    
    
    // RX
    uint8_t CHANNEL_ASSIGNMENT[16];
    uint64_t CHANNEL_FUNCTIONS[4];
    
    // Attitude
    float PID_YAW_c[4];
    float PID_PITCH_c[4];
    float PID_ROLL_c[4];
    
    // Rate
    float PID_YAW_m[4];
    float PID_PITCH_m[4];
    float PID_ROLL_m[4];    
    
    float PID_BARO[4];
    float PID_SONAR[4]; 
    
    // GPS
    float PID_GPS[4];
};

union CONFIG_union {
    struct CONFIG_struct data;
    uint8_t raw[sizeof(struct CONFIG_struct)];
};

extern CONFIG_union CONFIG;

void initializeEEPROM(void);
void writeEEPROM(void);
void readEEPROM(void);
