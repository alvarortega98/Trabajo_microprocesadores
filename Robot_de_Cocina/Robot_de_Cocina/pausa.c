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
	cli();
	
	//Se detiene el timer 0
	TCCR0B |= 1<<CS00;
	TCCR0B |= 1<<CS01;
	TCCR0B |= 1<<CS02;
	//Se detiene el timer 3
	TCCR3B |= 1<<CS30;
	TCCR3B |= 1<<CS31;
	TCCR3B |= 1<<CS32;
	
	sei();
}

void reanudar_timers(){
		TCCR0B |= (1 << CS01);
		TCCR3B |= 1<<CS31;
}

void main_pausa(){
	setup_pausa();
	do{		
	}while (bandera_SS == 0);
	bandera_SS = 0;
	//Se reanudan los timers para que vuelvan a contar
	reanudar_timers();	
}