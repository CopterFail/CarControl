/*  Ublox protocol (UBX) implementation in C++
    
    Currently i am running neo 6m module on Serial bus 2.
    readGPS function is called every 10ms (100Hz), baud rate should be adjusted accordingly.
    By "default" Serial2 uses 64 byte long RX buffer, using faster baud rate at the same "reading/sampling"
    rate will result in buffer overflow and data being lost/damaged.
    
    If you are using faster bus please adjust reading speed accordingly (or increase RX buffer size) to prevent
    buffer overflow.
    
    Big thanks to kha from #aeroquad for helping me get this up and running.
*/


#define UBX_CLASS_ACK 0x05 // Ack/Nack Messages
#define UBX_CLASS_AID 0x0B // AssistNow Aiding Messages
#define UBX_CLASS_CFG 0x06 // Configuration Input Messages
#define UBX_CLASS_ESF 0x10 // External Sensor Fusion Messages
#define UBX_CLASS_INF 0x04 // Information Messages
#define UBX_CLASS_MON 0x0A // Monitoring Messages

#define UBX_CLASS_NAV 0x01 // Navigation Results
#define UBX_ID_AOPSTATUS 0x60
#define UBX_ID_CLOCK     0x22
#define UBX_ID_DGPS      0x31
#define UBX_ID_DOP       0x04
#define UBX_ID_EKFSTATUS 0x40
#define UBX_ID_POSECEF   0x01
#define UBX_ID_POSLLH    0x02
#define UBX_ID_SBAS      0x32
#define UBX_ID_SOL       0x06
#define UBX_ID_STATUS    0x03
#define UBX_ID_SVINFO    0x30
#define UBX_ID_TIMEGPS   0x20
#define UBX_ID_TIMEUTC   0x21
#define UBX_ID_VELECEF   0x11
#define UBX_ID_VELNED    0x12

#define UBX_CLASS_RXM 0x02 // Receiver Manager Messages
#define UBX_CLASS_TIM 0x0D // Timing Messages

class UBLOX {
    public:
        // Constructor
        UBLOX();
        void read_packet();
        void process_data();
        
    private:
        // UBLOX binary message definitions (done by kha)
        struct ublox_NAV_STATUS { // 0x01 0x03 (16 bytes)
            uint32_t iTow;
            uint8_t  gpsFix;
            uint8_t  flags;
            uint8_t  fixStat;
            uint8_t  flags2;
            uint32_t ttfx;
            uint32_t msss;
        };

        struct ublox_NAV_POSLLH { // 0x01 0x02 (28 bytes)
            uint32_t iTow;
            int32_t lon; // 1e-7 degrees
            int32_t lat; // 1e-7 degrees
            int32_t height; // mm
            int32_t hMSL; // mm
            uint32_t hAcc; //mm
            uint32_t vAcc; //mm
        };

        struct ublox_NAV_SOL { // 0x01 0x6 (52 bytes)
            uint32_t iTow;
            int32_t  fTow;
            int16_t  week;
            uint8_t  gpsFix;
            uint8_t  flags;
            int32_t  ecefX;
            int32_t  ecefY;
            int32_t  ecefZ;
            int32_t  pAcc;
            int32_t  ecefVX;
            int32_t  ecefVY;
            int32_t  ecefVZ;
            int32_t  sAcc;
            uint16_t pDOP;
            uint8_t  res1;
            uint8_t  numSV;
            uint32_t res2;
        };

        struct ublox_NAV_VELNED { // 0x01 0x12h (36 bytes)
            uint32_t iTow; // time of week ms
            int32_t  velN; // cm/s
            int32_t  velE; // cm/s
            int32_t  velD; // cm/s
            uint32_t  speed; // cm/s
            uint32_t  gSpeed; // cm/s
            int32_t  heading; // dev 1e-5
            uint32_t sAcc; // cm/s
            uint32_t cAcc; // deg 1e-5
        };

        union ublox_message {
            struct ublox_NAV_STATUS nav_status;
            struct ublox_NAV_POSLLH nav_posllh;
            struct ublox_NAV_VELNED nav_velned;
            struct ublox_NAV_SOL nav_sol;
            uint8_t raw[52];
        } ubloxMessage;  
        
        
        // Private variables used inside the class/loop
        uint8_t data; // variable used to store a single byte from serial
        
        uint8_t UBX_step; // used to select a correct step (data packet is read by state machine)

        int8_t UBX_class;
        int8_t UBX_id;
        uint16_t UBX_expected_length;
        uint16_t UBX_data_length;
        uint8_t UBX_CK_A;
        uint8_t UBX_CK_B;        
};

