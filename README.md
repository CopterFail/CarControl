RC Car Controller based on Arduino
----------------------------------

Feature support:
----------------
	- a gyro sensor to detect / correct z-axis rotation for drift stabilisation
	- an acc sensor to detect forward/backward (LED control, x-axis) and check traction (y-axis)
	- sbus, hott or ppm connection from rx
	- telemetry to rx (hott or sport)
	- RGB LED (ws2812b), color and intensity (Rear, break, back lights)
	- switches for power LED
	- 2 servo outputs for steering , camera
	- 1 PWM output for ESC
	- switching outputs for horn, power led, etc.
	- battery monitor for telemetry
	- compass and GPS for telemetry
	- model based speed detection (?)

PIN setup
------------------------------------
  - I2C SCL 19
  - I2C SDA 18
  
  - Hott Rx Serial 1 (receiver) 3
  - Receiver PPM

  - GPS Rx Serial 2 (NEO 6M) 9
  
  - Hott Telemetrie Serial 3 Tx
  - Hott Telemetrie Serial 3 Rx

  - Camera Servo 22 
  - Steering Servo 21
  - Motor ESC 20 
  
  - Heardbeat LED 13 (Arduino Led)
  - RGB LEDs 6
  - Power LEDs PWM 23
  - Horn 11
  

  
