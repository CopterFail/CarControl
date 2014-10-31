/*
 * HottTelemetrie.h
 *
 *  Created on: 31.10.2014
 *      Author: dab
 */

#ifndef HOTTTELEMETRIE_H_
#define HOTTTELEMETRIE_H_

extern "C" {
int32_t i32HottTelemetrieInit( void );
int32_t i32HottTelemetrieLoop( uint8_t data );
uint8_t * aui8HottTelemetrieSendBuffer( void );
uint32_t ui32HottCount;
}



#endif /* HOTTTELEMETRIE_H_ */
