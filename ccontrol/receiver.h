/*
 * receiver.h
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#define SUMD_IS_ACTIVE
#define RX_CHANNELS 16
//extern
extern volatile uint16_t RX[RX_CHANNELS];


void ReceiverReadPacket( void );
void initializeReceiver( void );
void RX_failSafe( void );

#endif /* RECEIVER_H_ */
