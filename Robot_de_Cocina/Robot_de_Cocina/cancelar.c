/*
 * cancelar.c
 *
 * Created: 15/05/2021 4:37:20
 *  Author: angel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern uint32_t deci;	

void main_cancelar() {
	
	uint32_t t_buzz;
	int i;
	
	OCR3A = 1000;
	OCR3B = 500;
	
	for (i = 0; i < 2; i++) {
		t_buzz = deci;
		DDRE |= 1<<PE4;
		do {} while (deci - t_buzz < 3);
		DDRE &= ~(1<<PE4);
		do {} while (deci - t_buzz < 6);
		}
}