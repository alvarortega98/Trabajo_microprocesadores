/*
 * USART_irq.h
 *
 * Created: 17/05/2021 12:23:25
 *  Author: alvar
 */ 


#define FOSC 16000000 // Clock Speed
#define F_CPU 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr);
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char data);
void USART_putstring(char* StringPtr);
int USART_Transmit_IO(char data, FILE *stream);