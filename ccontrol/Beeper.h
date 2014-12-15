/*
 * Beeper.h
 *
 *  Created on: 14.12.2014
 *      Author: dab
 */

#ifndef BEEPER_H_
#define BEEPER_H_

class Beeper {
public:
	Beeper();
	virtual ~Beeper();

	void attach( uint8_t ui8PinNumber );
	void ack( void );
	void nack( void );
	void beep( uint8_t ui8beeps );

	void update( void );

private:
	uint8_t pin;
	uint8_t tobeep;
	uint8_t cnt;
	uint8_t puls, pause;
	bool active;
};

#endif /* BEEPER_H_ */
