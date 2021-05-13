/*
 * timer1.c
 *
 * Created: 12/05/2021 5:32:31
 * Author : angel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "inicio.h"

volatile int deci = 0;
volatile int secs = 0;
volatile uint16_t Ton;

volatile uint8_t flag_gA;
volatile uint8_t flag_H;

volatile uint8_t program;
volatile uint16_t seconds = 0;		//realmente se pone a 0 cuando empieza el programa
volatile uint16_t weight;
volatile uint8_t display_mode;

void setup() {
	
	cli();
	
	//Preescalado 256:
	TCCR1B = (1 << CS12);	
	
	//Evento por flanco de subida en IC.
	TCCR1B |= (1 << ICES1);
			
	
	//Interrupciones por compare&match, IC.
	TIMSK1 = 1<<OCIE1A;
	TIMSK1 |= 1<<OCIE1B;
	TIMSK1 |= 1<<OCIE1C;
	TIMSK1 |= (1 << ICIE1);
	
	//Borrado de potencial bandera.
	TIFR1 = (1 << ICIE1);	
	
	//Registros de comparación:
	OCR1A = 31;				//0.1 sec, 3124
	OCR1B = 156;				//0.5 sec, 15264
	OCR1C = 312;				//1 sec, 31249
	
	sei();
}

int main(void) {
	
	setup();
	
	while (1) {
	}
	
}


ISR(TIMER1_COMPA_vect) {
	deci++;
	flag_H = 1;
	//OCR1A += 3125;	
	OCR1A += 31;
}

ISR(TIMER1_COMPB_vect) {
	flag_gA = 1;
	//OCR1B += 15625;
	OCR1B += 156;	
}

ISR(TIMER1_COMPC_vect) {
	//change displays
	seconds++;
	OCR1C += 312;
	//OCR1C += 31250;
}

ISR ( TIMER1_CAPT_vect ) {
	
	uint16_t Trise, Tfall;
	
	if (TCCR1B & (1 << ICES1)) {
		TCCR1B &= ~(1 << ICES1);
		Trise = ICR1;
	} else {
		TCCR1B |= (1 << ICES1);
		Tfall = ICR1;
		Ton = Tfall - Trise;
	}
	
}

uint16_t getWeight() {
	return (Ton*32/100);
	//Es entre 32 por el preescalado, que es de 256.
}
