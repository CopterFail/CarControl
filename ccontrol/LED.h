/*
 * LED.h
 *
 *  Created on: 28.02.2014
 *      Author: dab
 */

#ifndef LED_H_
#define LED_H_


#define LED_STATUS_COUNT	10
#define LED_STATUS_MAX 		30


#ifdef LED_STATUS

#define RED		strip.Color(LED_STATUS_MAX, 0,              0)
#define RED_LT		strip.Color(LED_STATUS_MAX>>2, 0,              0)
#define GREEN	strip.Color(0,              LED_STATUS_MAX, 0)
#define BLUE	strip.Color(0,              0,              LED_STATUS_MAX)
#define WHITE	strip.Color(LED_STATUS_MAX, LED_STATUS_MAX, LED_STATUS_MAX)
#define YELLOW	strip.Color(LED_STATUS_MAX, LED_STATUS_MAX, 0)
#define BLACK	strip.Color(0,              0,              0)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STATUS_COUNT, LED_STATUS, NEO_GRB + NEO_KHZ800);

uint32_t LedStatus[5] = { RED, RED, RED, RED, RED };


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
#endif



void LED_Init( void )
{
    // PIN settings:
#ifdef LED_ARDUINO
    pinMode(LED_ARDUINO, OUTPUT);
#endif

#ifdef LED_WHITE
    pinMode(LED_WHITE, OUTPUT);
#endif

#ifdef LED_RED
    pinMode(LED_RED, OUTPUT);
#endif

#ifdef LED_STATUS
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    //colorWipe(strip.Color(LED_STATUS_MAX, 0, 0), 50); // Red
#endif
}

void LED_ON( void )
{
#ifdef LED_WHITE
    digitalWrite(LED_WHITE, HIGH);
#endif
#ifdef LED_RED
    digitalWrite(LED_RED, HIGH);
#endif
}

void LED_OFF( void )
{
#ifdef LED_WHITE
    digitalWrite(LED_WHITE, LOW);
#endif
#ifdef LED_RED
    digitalWrite(LED_RED, LOW);
#endif

}

void LED_50Hz( void )
{
/*
#ifdef LED_WHITE
    // Blink "aircraft beacon" LED
    if ((Beacon_LED_state == 51) || (Beacon_LED_state == 59) || (Beacon_LED_state == 67)) {
        digitalWrite(LED_WHITE, HIGH);
    } else {
        digitalWrite(LED_WHITE, LOW);
    }

    Beacon_LED_state++;

    if (Beacon_LED_state >= 100) {
        Beacon_LED_state = 0;
    }
#endif
*/
}

void LED_10Hz( void )
{
	uint32_t status;
	uint16_t i, update=0;

#ifdef LED_ARDUINO
    // Blink integrated arduino LED
    Arduino_LED_state = !Arduino_LED_state;
    digitalWrite(LED_ARDUINO, Arduino_LED_state);

#endif

// A multi color LED Stripe take 20 x 40us = 0.8ms time to update, no interrupts (only used by sonar)
// LED-Stripe: MultiColor(5V, Status, 1Hz?)
#ifdef LED_STATUS

    status = RED_LT;
    if( ( mod_a < -0.1 ) && ( mod_v > 0.0) ) status = RED;
    if( mod_v < 0 ) status = WHITE; 
    
    for( i=0; i<4; i++ )
    {
    	strip.setPixelColor(i,    status);
    	strip.setPixelColor(i+5,  status);
    }
    strip.show();	// this will take 700us with interrupts disabled!
    
#endif

#ifdef LED_STATUS_TEST
    status = ((uint32_t)throttle - 1000) / 50;
   	for( i=0; i<20; i++ )
    {
    	strip.setPixelColor(i,(status>=i)?GREEN:BLUE);
    }
   	strip.show();	// this will take 700us with interrupts disabled!
#endif

// LED-Stripe: Red(12V), White(12V)
    if (armed) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

void LED_1Hz( void )
{
// LED-Stripe: Red(12V), White(12V)
    // Armed/ Dis-armed indicator
    if (!armed) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

#endif /* LED_H_ */
