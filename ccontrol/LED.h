/*
 * LED.h
 *
 *  Created on: 28.02.2014
 *      Author: dab
 */

#ifndef LED_H_
#define LED_H_


#define LED_STATUS_COUNT	5
#define LED_STATUS_MAX 		255


#ifdef LED_STATUS

#define RED	  strip.Color(LED_STATUS_MAX, 0,              0)
#define RED_LT	  strip.Color(LED_STATUS_MAX>>3, 0,              0)
#define GREEN	  strip.Color(0,              LED_STATUS_MAX, 0)
#define GREEN_LT  strip.Color(0,              LED_STATUS_MAX>>3, 0)
#define BLUE      strip.Color(0,              0,              LED_STATUS_MAX)
#define BLUE_LT   strip.Color(0,              0,              LED_STATUS_MAX>>3)
#define WHITE     strip.Color(LED_STATUS_MAX, LED_STATUS_MAX, LED_STATUS_MAX)
#define WHITE_LT  strip.Color(LED_STATUS_MAX>>2, LED_STATUS_MAX>>2, LED_STATUS_MAX>>2)
#define YELLOW	  strip.Color(LED_STATUS_MAX, LED_STATUS_MAX, 0)
#define YELLOW_LT strip.Color(LED_STATUS_MAX>>3, LED_STATUS_MAX>>3, 0)
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

void LED_SetStatus( uint32_t color )
{
#ifdef LED_STATUS
    uint16_t i;
    for( i=0; i<LED_STATUS_COUNT; i++ )
    {
    	strip.setPixelColor(i,color);
    }
    strip.show();	// this will take some time
#endif
}
  
void LED_50Hz( void )
{
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

// A multi color LED Stripe take 5 x 40us = 0.2ms time to update
// LED-Stripe: MultiColor WS2812B 5V
#ifdef LED_STATUS
    switch( iCarMode )
    {
      case MODE_BREAK:
        status = RED;
        break;
      case MODE_BACKWARD:
        status = WHITE_LT;
        break;
      case MODE_NEUTRAL:
        status = RED_LT;
        break;
      case MODE_NORMAL:
      default:
        status = RED_LT;
        break;
    }
    
    //ToDo: signal failsafe
    LED_SetStatus( status );
#endif


// LED-Stripe: Red(12V), White(12V)
    if (1) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

void LED_1Hz( void )
{
// LED-Stripe: Red(12V), White(12V)
    // Armed/ Dis-armed indicator
    if (1) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

#endif /* LED_H_ */
