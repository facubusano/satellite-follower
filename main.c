/*
 * antenna.c
 *
 * Created: 14-Jan-20 16:08:03
 * Authors : Facundo Busano & Fernando Ribero
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "function.h"
#include "usart.h"

//VARs AND CONSTANTS DEFINITION
unsigned int s = 0;
unsigned char cData = 0;
volatile int iStringFlag = 0;
unsigned char cCommand_Array[35];																		

//MAIN LOOP
int main(void)
{
	USART0_INIT();																			//USART0 initialization
	sei();																					//interrupts enabled
	
	DDRD |= (1<<AZ_enable) | (1<<DIR_AZ) | (1<<AZ_output) | (1<<EL_enable) | (1<<DIR_EL);	//outputs pins PTD 
	DDRD &= ~(1<<AZ_origin);																//input pins PTD
	PORTD |= (1<<AZ_origin);																//pull up pins PTD
	PORTD |= (1<<AZ_enable) | (1<<EL_enable);												//high state pins PTD												
	
	DDRB |= (1<<EL_output) | (1<<ReDePin);													//outputs pins PTB
	DDRB &= ~(1<<EL_origin);																//input pins PTD
	PORTB |= (1<<EL_origin);																//pull up pins PTD
	PORTB &= ~(1<<ReDePin);																	//low state pins PTD
	
	while (1) {
		if (iStringFlag == 1)																//checks complete string received
		{
			read_command();																	//identifies type of command 
			iStringFlag = 0;																//state flag reset
		}
	    if (!iRotatorFlag){														            //checks if some command enabled the rotor movement (ENABLE = LOW)
		    move_rotators();                                                                //executes move_antenna();
	    }
	    else{                                                                               //checks if none command enabled the rotor movement (DISABLE = HIGH)
		    PORTD |= (1<<AZ_enable) | (1<<EL_enable);                                       //disables rotors (DISABLE = HIGH)
	    }
	}
}

//SERIAL INTERRUPT ROUTINE
ISR(USART_RX_vect){
	cData = USART0_RX_CHAR();
	if (cData != 10){																		//compares to "end of line" (DEC:10) that means end of command
		cCommand_Array[s] = cData;															//stores all the characters of the command in the vector
		s++;
	}
	else{																				
		iStringFlag = 1;																	//sets complete string flag
		s = 0;																				//counter var reset											
	}
}