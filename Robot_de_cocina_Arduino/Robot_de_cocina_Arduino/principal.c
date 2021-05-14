/*
 * principal.c
 *
 * Created: 14/05/2021 8:22:36
 *  Author: alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"


volatile extern char estado;
volatile extern uint8_t bandera_SS_corta;
volatile extern uint8_t bandera_SS_larga;
volatile extern uint8_t contador;
volatile extern uint8_t display_0;
volatile extern uint8_t display_1;

volatile uint16_t t_fin;

void setup_principal(){
	estado = 'p';
	t_fin = 3600;
	
	cli();
	//---Timer 1------------------------------------------------------------------------------------------------
	//Preescalado 256:
	TCCR1B = (1 << CS12);

	//Interrupciones por compare&match, IC.
	TIMSK1 |= 1<<OCIE1A;
	TIMSK1 |= 1<<OCIE1B;

	//Registros de comparación:
	OCR1A = 31250;				//0.5 sec, 15635
	OCR1B = 62500;				//1 sec, 31250
	//--------------------------------------------------------------------------------------------------------
	sei();
}


void main_principal(){
	setup_principal();
	do {
		if (bandera_SS_corta == 1){
			bandera_SS_corta = 0;
			main_pausa();
			estado = 'p';
		}
		if (bandera_SS_larga == 1){
			bandera_SS_larga = 0;
			break;
		}
	}while(t_fin != 0);
}


ISR(TIMER1_COMPA_vect) {
	static uint8_t display_mode = 0;
	
	//--Modo pausa, parpadear displays ---------
	if (estado == 's'){
		display_mode =~ display_mode;
		if (display_mode){
			display_1 = (t_fin/60)/10;
			display_0 = (t_fin/60)%10;
			} else {
			display_1 = 10;
			display_0 = 10;
		}
		actualiza_display();
	}
	//------------------------------------------
	//OCR1A += 15625;
	OCR1A += 31250;	
}

ISR(TIMER1_COMPB_vect) {
	static uint8_t display_mode_p = 0;
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		t_fin -= 60;
		display_mode_p =~ display_mode_p;
		if (display_mode_p){
			display_1 = (t_fin/60)/10;
			display_0 = (t_fin/60)%10;
		} else {
			display_1 = 11;
			display_0 = contador;
		}
		actualiza_display();	
	}
	OCR1B += 62500;
	//OCR1B += 31250;
}