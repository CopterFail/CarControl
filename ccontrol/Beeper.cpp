/*
 * Beeper.cpp
 *
 *  Created on: 14.12.2014
 *      Author: dab
 */

#include <Arduino.h>
#include "defines.h"

#include "Beeper.h"

#define PAUSE 1
#define PULS 2

Beeper::Beeper( uint8_t ui8PinNumber ) {
	pin = ui8PinNumber;
	tobeep = 0;
	active = false;
	if( pin ) {
		pinMode(pin, OUTPUT);
	}

}

Beeper::~Beeper() {
	// TODO Auto-generated destructor stub
}

void Beeper::ack( void ) {
	tobeep = 1;
}

void Beeper::nack( void ) {
	tobeep = 2;
}

void Beeper::beep( uint8_t ui8beeps ) {
	tobeep = ui8beeps;
}

void Beeper::update( void ) {
	static uint8_t cnt=0;
	if( cnt > 0 ){
		cnt--;
	}else{
		if( active ) {
			active = false;
			cnt = PAUSE;
			if( pin )digitalWrite(pin, LOW);
		} else {
			if( tobeep > 0 ){
				tobeep--;
				active = true;
				cnt = PULS;
				if( pin )digitalWrite(pin, HIGH);
			}

		}
	}
	if( active ){
		active = false;
	}
}
