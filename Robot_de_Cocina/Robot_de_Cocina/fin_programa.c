/*
 * fin_programa.c
 *
 * Created: 15/05/2021 4:01:43
 *  Author: angel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern uint32_t deci;			//declarar como extern en prog ppal
volatile  uint8_t iterador;		//declarar en programa ppal
volatile extern uint8_t bandera_SS;
volatile  uint8_t buzzer_en;

void main_fin_programa() {
	
	iterador = 20;	
	OCR3A = 1000;
	OCR3B = 500;
	
	do {
		if (bandera_SS == 0) {
			break;
		}
	} while (iterador != 0);
	DDRE &= (1<<PE4);
	
}


