/*
 * cancelar.c
 *
 * Created: 15/05/2021 14:34:21
 *  Author: alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern uint32_t deci;
volatile extern char estado;


void main_cancelar() {
	estado = 'c';
	
	
	uint32_t t_buzz;
	int i;	
	
	for (i = 0; i < 2; i++) {
		t_buzz = deci;
		DDRB |= 1<<PINB3;
		do {} while (deci - t_buzz < 3);
		DDRB &= ~(1<<PINB3);
		do {} while (deci - t_buzz < 6);
	}
}