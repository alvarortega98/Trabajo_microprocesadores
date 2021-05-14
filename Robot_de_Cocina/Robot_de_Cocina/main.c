/*
 * Robot_de_cocina.c
 *
 * Created: 03/05/2021 10:32:52
 * Authors: Álvaro Ortega Lozano y Ángel Jarillo Misme
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"

volatile char estado; //i:inicio, p:principal, s: pausa, c: cancelar, f:fin

volatile uint8_t bandera_UP = 0;
volatile uint8_t bandera_SS = 0;
volatile uint8_t bandera_SS_corta = 0;
volatile uint8_t bandera_SS_larga = 0;
volatile uint16_t display_0;
volatile uint16_t display_1;
volatile uint64_t millis = 0;
volatile uint8_t display_1_7seg;
volatile uint8_t display_0_7seg;
volatile uint8_t contador = 0;


void setup() {
	
	cli();
	
	estado = 'i';
	
	//paro los timers del programa principal
	//Paro el timer 0
	TCCR0B |= 1<<CS00;
	TCCR0B |= 1<<CS01;
	TCCR0B |= 1<<CS02;
	//Paro el timer 1
	TCCR1B |= 1<<CS10;
	TCCR1B |= 1<<CS11;
	TCCR1B |= 1<<CS12;
	//Paro el timer 3
	TCCR3B |= 1<<CS30;
	TCCR3B |= 1<<CS31;
	TCCR3B |= 1<<CS32;
	
	//Las siguientes líneas de código son para habilitar las interrupciones de los botones SS (INT5) y UP (INT6).
	EICRB = 1<<ISC50;   //PE5
	EICRB |= 1<<ISC60;  //PE6
	EIMSK = 1<<INT5;
	EIMSK |= 1<<INT6;
	
	
	
	//Seteamos entradas y salidas
	DDRA = 0xFF;		    //Display
	DDRB = 1<<SELECCION;    //Señal selección display
	DDRE = 0x00;
	
	//Habilitamos el timer para contar milisegunos y realizar el refresco del display (modo CTC, preescalado 64)
	TCCR2A = 1<<WGM21;
	TCCR2B = 1<<CS20;
	TCCR2B |= 1<<CS21;
	TIMSK2 = 1<<OCIE2A;
	TIMSK2 |= 1<<OCIE2B;
	OCR2A = 250;  //cada milisegundo
	OCR2B = 64;   //cada 0.25 milisegundos

	inicializa_display();
	actualiza_display();
	
	sei();
}

void actualiza_contador() {
	if (contador < 9) {
		contador++;
		} else {
		contador = 0;
	}
	display_0 = contador;
	
}

void actualiza_display() {
	switch (display_1) {
		case 0: display_1_7seg = 0b00111111; break; //Número 0;
		case 1: display_1_7seg = 0b00000110; break; //Número 1;
		case 2: display_1_7seg = 0b01011011; break; //Número 2;
		case 3: display_1_7seg = 0b01001111; break; //Número 3;
		case 4: display_1_7seg = 0b01100110; break; //Número 4;
		case 5: display_1_7seg = 0b01101101; break; //Número 5;
		case 6: display_1_7seg = 0b01111101; break; //Número 6;
		case 7: display_1_7seg = 0b00000111; break; //Número 7;
		case 8: display_1_7seg = 0b01111111; break; //Número 8;
		case 9: display_1_7seg = 0b01101111; break; //Número 9;
		case 10: display_1_7seg = 0b00000000; break; //Display apagado;
		case 11: display_1_7seg = 0b01110011; break; //Letra P;
	}
	
	switch (display_0) {
		case 0: display_0_7seg = 0b00111111; break; //Número 0;
		case 1: display_0_7seg = 0b00000110; break; //Número 1;
		case 2: display_0_7seg = 0b01011011; break; //Número 2;
		case 3: display_0_7seg = 0b01001111; break; //Número 3;
		case 4: display_0_7seg = 0b01100110; break; //Número 4;
		case 5: display_0_7seg = 0b01101101; break; //Número 5;
		case 6: display_0_7seg = 0b01111101; break; //Número 6;
		case 7: display_0_7seg = 0b00000111; break; //Número 7;
		case 8: display_0_7seg = 0b01111111; break; //Número 8;
		case 9: display_0_7seg = 0b01101111; break; //Número 9;
		case 10: display_0_7seg = 0b00000000; break; //Display apagado;
		case 11: display_0_7seg = 0b01110011; break; //Letra P;
	}
}

void inicializa_display() {
	display_0 = 0;
	display_1 = 11;
}



int main(void) {
	do{	
		setup();
		
		do {
			if (bandera_UP == 1) {
				actualiza_contador();
				actualiza_display();
				bandera_UP = 0;
			}
		} while (bandera_SS == 0);
		bandera_SS = 0;
		main_principal();
	
	} while(1);
}




ISR(INT5_vect) {
	static uint64_t Trise = 0;
	static uint64_t Ton = 0;
	static uint8_t bandera_antirrebotes_SS = 0;
	static uint64_t T_SS = 0;
	
	if (bandera_antirrebotes_SS == 1) {
		if (millis - T_SS > 5) {   //Cambiar el tiempo a 5 ms para simulación
			bandera_antirrebotes_SS = 0;
		}
	}
	if (bandera_antirrebotes_SS == 0) {
		if (getBit(BOTONES, SS)) {
			Trise = millis;
			T_SS = millis;
			bandera_antirrebotes_SS = 1;
		} else {
			if ((estado == 'i')||(estado == 's')){
				bandera_SS = 1;
			}
			Ton = millis -Trise;
			T_SS= millis;
			bandera_antirrebotes_SS = 1;
			if (estado == 'p'){
				if (Ton < 2000){
					bandera_SS_corta = 1;
				} else {
					bandera_SS_larga = 1;
				}
			}
		}
	}
}

ISR(INT6_vect) {
	static uint8_t bandera_antirrebotes_UP = 0;
	static uint64_t T_UP = 0;
	
	if (bandera_antirrebotes_UP == 1) {
 		if (millis - T_UP > 50) {        //Cambiar el tiempo a 5 ms para simulación
			bandera_antirrebotes_UP = 0;
		}
	}
	if (bandera_antirrebotes_UP == 0) {
		if (getBit(BOTONES, UP)) {			
			T_UP = millis;
			bandera_antirrebotes_UP = 1;
			} else {
			if (estado == 'i'){
				bandera_UP = 1;
			}
			T_UP = millis;
			bandera_antirrebotes_UP = 1;
		}
	}
	
}

ISR(TIMER2_COMPA_vect) {
	//Cuando el sistema está en pausa, cancelar o fin de programa no se cuentan milisegundos
	if((estado == 'i')||(estado == 'p')){
		millis++;
	}
}

ISR(TIMER2_COMPB_vect) {
	static uint8_t sel = 0;
	static int aux = 0;
	aux++;
	OCR2B += 64;
	sel = ~sel;
	if (sel == 1) {
		setBit(PORTB, SELECCION);
		SEGMENTOS = display_1_7seg;
		} else {
		clrBit(PORTB, SELECCION);
		SEGMENTOS = display_0_7seg;
	}
}
