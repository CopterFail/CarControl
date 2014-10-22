Car Controller based on Arduino (Teensy 3.1)
------------------------------------------------

Feature support:
----------------
	- a gyro sensor to detect / correct z-axis rotation for drift stabilisation
	- an acc sensor to detect forward/backward (LED control, x-axis) and check traction (y-axis)
	- hott or ppm connection from rx
	- telemetry to rx (hott) and minimosd(mavlink)
	- RGB LED (ws2812b), color and intensity
	- switches for power LED
	- 2 servo outputs steering , camera
	- 1 PWM output for ESC
	- switching outputs for horn, power led, etc.
	- battery monitor for telemetry
	- compass and GPS for telemetry
	- speed detection ???

PIN setup (Teensy 3.1 pin numbering)
------------------------------------
  - I2C SCL 19
  - I2C SDA 18
  
  - PPM in (receiver) 3
  
  - Rotor 1 
  - Rotor 2
  - Rotor 3 
  - Rotor 4 
  
  - Lights / Armed-Disarmed indicator 14
  - RGB LEDs 6
  
  - Battery Monitor (current sensor) 17
  
