/*
 * Beeper.cpp
 *
 *  Created on: 14.12.2014
 *      Author: dab
 */

#include <Arduino.h>
#include "defines.h"

#include "Beeper.h"

#define BEEPER_ACTIVE
#define PAUSE 1
#define PULS 2

Beeper::Beeper() {
	pin = 0;
	tobeep = 0;
	cnt = 0;
	active = false;
	pause = PAUSE;
	puls = PULS;
}

Beeper::~Beeper() {
	// TODO Auto-generated destructor stub
}

void Beeper::attach( uint8_t ui8PinNumber ) {
	pin = ui8PinNumber;
	if( pin ) {
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}

void Beeper::ack( void ) {
	tobeep = 1;
	pause = 2;
	puls = 1;

}

void Beeper::nack( void ) {
	tobeep = 2;
	pause = 2;
	puls = 1;
}

void Beeper::beep( uint8_t ui8beeps ) {
	tobeep = ui8beeps;
	pause = PAUSE;
	puls = PULS;
}

void Beeper::update( void ) {
#ifdef BEEPER_ACTIVE
	if( cnt > 0 ){
		cnt--;
	}else{
		if( active == true ) {
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
#endif
}
