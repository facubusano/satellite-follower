/*
 * usart.c
 *
 * Created: 14-Jan-20 09:36:52
 * Authors : Facundo Busano & Fernando Ribero
 */ 

#include <avr/io.h>
#include "usart.h"

//USART0 module initialization
void USART0_INIT(){
	UCSR0B=0b10011000;							//RX interrupt, RX and TX enabled
	UCSR0C=0b00000110;							//asynchronous mode, no parity, 1 stop bit, 8 bits data
	UBRR0=103;									//9600 bauds (16MHz clock)
}

void USART0_TX_CHAR(unsigned char cChar){
	while( !(UCSR0A & (1<<5)) ){				//waits until UDR0 register is ready to receive new data
		}    
	UDR0 = cChar;								//sends character
}

void USART0_TX_STRING(char	*cString){
	while(*cString !=0x00){						//waits until last character NULL 
		USART0_TX_CHAR(*cString);				//send character
		cString++;
	}
}

unsigned char USART0_RX_CHAR(){
	while( !(UCSR0A & (1<<7)) ){}				//waits until there is unread data in UDR0 register
		return UDR0;							//returns data
}