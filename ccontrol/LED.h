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
#endif

void LED_Init( void );
void LED_ON( void );
void LED_OFF( void );
//void LED_SetStatus( uint32_t color );
void LED_50Hz( void );
void LED_10Hz( void );
void LED_1Hz( void );

#endif /* LED_H_ */
