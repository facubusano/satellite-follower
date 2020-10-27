/*
 * usart.h
 *
 * Created: 14-Jan-20 09:36:30
 * Authors : Facundo Busano & Fernando Ribero
 */ 

#ifndef USART_H_
#define USART_H_

#include <avr/io.h>

//PROTOTYPE FUNCTIONS DEFINITION
void USART0_INIT();
void USART0_TX_CHAR(unsigned char);
void USART0_TX_STRING(char*);
unsigned char USART0_RX_CHAR();

#endif /* USART_H_ */