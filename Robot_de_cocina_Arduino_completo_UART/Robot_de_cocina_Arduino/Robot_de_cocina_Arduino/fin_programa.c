/*
 * fin_programa.c
 *
 * Created: 15/05/2021 14:25:07
 *  Author: alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern uint32_t deci;			//declarar como extern en prog ppal
volatile  uint8_t iterador;		//declarar en programa ppal
volatile extern uint8_t bandera_SS;
volatile extern char estado;

void main_fin_programa() {
	estado = 'f';
	iterador = 20;
	OCR2A = 128;
	
	do {
		if (bandera_SS == 1) {
			break;
		}
	} while (iterador > 0);
	DDRB |= 1<<PINB3;
}