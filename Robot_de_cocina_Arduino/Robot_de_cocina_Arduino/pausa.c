/*
 * pausa.c
 *
 * Created: 14/05/2021 9:05:36
 *  Author: alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile extern char estado;
volatile extern uint8_t bandera_SS;

void setup_pausa(){
	estado = 's';
}

void main_pausa(){
	static uint8_t aux=0;
	setup_pausa();
	do{
		aux++;
	}while (bandera_SS == 0);
	bandera_SS = 0;
}