/*
 * pausa.c
 *
 * Created: 13/05/2021 17:09:43
 *  Author: alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern char estado;
volatile extern uint8_t bandera_SS;

void setup_pausa(){
	estado = 's';	
	
	//Paro el timer 0
	TCCR0B |= 1<<CS00;
	TCCR0B |= 1<<CS01;
	TCCR0B |= 1<<CS02;
	//Paro el timer 3
	TCCR3B |= 1<<CS30;
	TCCR3B |= 1<<CS31;
	TCCR3B |= 1<<CS32;
}

void main_pausa(){
	setup_pausa();
	do{		
	}while (bandera_SS == 0);
	bandera_SS = 1;	
}