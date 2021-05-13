/*
 * timer0.c
 *
 * Created: 12/05/2021 9:19:47
 * Author : angel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "inicio.h"

void setup() {
	
	cli();
	
	//Preescalado 8:
	TCCR0B = (1 << CS01);
	
	//Modo 7: Fast PWM, non-inverting mode
	TCCR0A = (1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << WGM02);
	TCCR0A |= (1 << COM0B1);
	
	//PG5 como salida:
	DDRG |= (1 << PG5); 
	
	OCR0B = 0.4*100;
	OCR0A = 100; 
	
	sei();
	
}

int main(void) {
	
	volatile int aux = 0;
	volatile int aux2 = 0;
	setup();
	
    while (1) 
    {
		aux++;
		if (OCR0A = 100) {
			aux2 = aux;
		}
		
    }
}

