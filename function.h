/*
 * function.h
 *
 * Created: 14-Jan-20 16:09:36
 * Authors : Facundo Busano & Fernando Ribero
 */ 

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

//VARs DECLARATION
extern unsigned char cData;
extern unsigned char cCommand_Array[35];
extern unsigned char cAZ_PosArray[3];
extern unsigned char cEL_PosArray[2];
extern unsigned int iAZ_PosNum;
extern unsigned int iEL_PosNum;
extern unsigned char cAZ_DegreeArray[3];
extern unsigned char cEL_DegreeArray[2];
extern unsigned int iAZ_DegreeNum;
extern unsigned int iEL_DegreeNum;
extern unsigned int iAZ_Pulses;
extern unsigned int iEL_Pulses;
extern unsigned int iAZ_PosPulses;
extern unsigned int iEL_PosPulses;
extern unsigned int s;
extern unsigned int i;
extern unsigned int j;
extern unsigned int iZ_Char;
extern unsigned int iE_Char;
extern unsigned int iU_Char;
extern unsigned char cAZ_RotorDir;
extern unsigned char cEL_RotorDir;
extern unsigned int iAZ_OriginFlag;
extern unsigned int iEL_OriginFlag;
extern unsigned int iRotatorFlag;
extern volatile int iStringFlag;
extern unsigned int iAZ_PulseFlag;
extern unsigned int iEL_PulseFlag;
extern unsigned int iSpeedFlag;

//CONSTANTS DEFINITION
#define AZ_enable	PORTD2																	//D2 pin
#define DIR_AZ		PORTD3																	//D3 pin
#define AZ_output	PORTD4																	//D4 pin
#define AZ_origin	PORTD5																	//D5 pin
#define EL_enable	PORTD6																	//D6 pin
#define DIR_EL		PORTD7																	//D7 pin
#define EL_output	PORTB0																	//D8 pin
#define EL_origin	PORTB1																	//D9 pin
#define ReDePin		PORTB5																	//D13 pin


#define T_ON_FOLLOW		5																	//Period of rotor following movement
#define T_OFF_FOLLOW	30

#define T_ON_SET		5																	//Period of rotor initial position movement
#define T_OFF_SET		5

#define FIRMWARE_V	"v1.0"

//PROTOTYPE FUNCTIONS DEFINITION
void read_command();
int char_to_num (char);
char num_to_char(int);
void convert_to_num();
void convert_to_pulses();
void convert_to_degree();
void convert_to_char();
void move_rotators();

#endif /* FUNCTION_H_ */