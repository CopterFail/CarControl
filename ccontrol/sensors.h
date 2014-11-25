#define GYROSCOPE_DETECTED      0x01
#define ACCELEROMETER_DETECTED  0x02
#define MAGNETOMETER_DETECTED   0x04
#define BAROMETER_DETECTED      0x08
#define GPS_DETECTED            0x10

class SensorArray {
    public:
        uint16_t sensors_detected;
        
        SensorArray() {
            sensors_detected = 0x00;
        };
        void initialize();
        void initializeGyro();
        void readGyroSum();
        void evaluateGyro();
        void initializeAccel();
        void calibrateAccel();
        void readAccelSum();
        void evaluateAccel();
        void initializeMag();
        void readMag();
        void evaluateMag();
        void initializeBaro();
        void readBaroSum();
        void evaluateBaroAltitude();
        void initializeGPS();
        void readGPS();
        void evaluateGPS();
        
        // I2C stuff
        void i2c_write8 (int16_t deviceAddress, uint8_t registerAddress, int16_t registerValue);
        int16_t i2c_read16 (int16_t deviceAddress, uint8_t registerAddress);
};    

extern float gyro[3];
extern float accel[3];
extern float gyro_temperature;

extern int16_t magRaw[3];
extern float magHeadingX, magHeadingY;
extern float magHeadingAbsolute;

extern struct tgpsData {
    int32_t  lat,lon;   // position as degrees (*10E7)
    int32_t  course;    // degrees (*10E5)
    uint32_t speed;     // cm/s
    int32_t  height;    // mm (from ellipsoid)
    uint32_t accuracy;  // mm
    uint32_t fixtime;   // fix
    uint32_t sentences; // sentences/packets processed from gps (just statistics)
    uint8_t  state;     // gps state
    uint8_t  sats;      // number of satellites active
} gpsData, gpsHome;


extern SensorArray sensors;

