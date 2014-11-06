/*
 * model.h
 *
 *  Created on: 04.11.2014
 *      Author: dab
 */

#ifndef MODEL_H_
#define MODEL_H_


extern float kinematicsAngle[3];
extern float mod_v, mod_a, mod_ang, mod_dang, mod_f;
extern int16_t iCarMode;
extern int16_t iCarLight;


void InitModell( void );
void updateModell_50Hz( void );
void updateModell_10Hz( void );



#endif /* MODEL_H_ */
