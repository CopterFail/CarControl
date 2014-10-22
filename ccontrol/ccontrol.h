// Main loop variables
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sensorPreviousTime = 0;
uint8_t frameCounter = 0;
uint32_t itterations = 0;

bool all_ready = false;
bool armed = false;

// Flight modes
#define RATE_MODE 0
#define ATTITUDE_MODE 1

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

// Blinking LED to indicate activity
bool Arduino_LED_state = 0;
uint8_t Beacon_LED_state = 0;

// Modulo definitions (integer remainder)
#define TASK_50HZ 2
#define TASK_10HZ 10
#define TASK_1HZ 100

// Kinematics variable definitions
float kinematicsAngle[3];
float mod_v, mod_a, mod_ang, mod_dang, mod_f;

// FlightController commands definitions
float commandSteer, commandCam, commandThrottle;
uint8_t flightMode = RATE_MODE;

// PID variables
int16_t throttle = 1000;

#define MOTORS 4
int16_t MotorOut[MOTORS];

// +- PI normalization macro
#define NORMALIZE(x) do { if ((x) < -PI) (x) += 2 * PI; else if ((x) > PI) (x) -= 2 * PI; } while (0);

// Custom definitions
//#define DISPLAY_ITTERATIONS
