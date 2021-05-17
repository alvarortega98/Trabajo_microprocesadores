/*
 * Prueba_IC_UART.c
 *
 * Created: 17/05/2021 11:29:29
 * Author : alvar
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART_irq.h"
#include <util/delay.h>

char String[]="Hello world!!"; 


static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit_IO, NULL,_FDEV_SETUP_WRITE);
	
int main(void)
{
	USART_Init(MYUBRR);
	
	sei();
	
	//USART_putstring(String);
	stdout = &mystdout;
	printf("Hello, world!\n");
	
	uint16_t u16Data = 10;
	 
	while(1){
		//USART_Transmit(USART_Receive());
		//USART_putstring(String);
		printf("\nunsigned int = %u",u16Data);
		_delay_ms(1000);
	}
}