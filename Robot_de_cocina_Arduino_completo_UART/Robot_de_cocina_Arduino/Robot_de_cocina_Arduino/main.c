/*
 * Robot_de_cocina_Arduino.c
 *
 * Created: 14/05/2021 8:19:27
 * Author : alvar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"


volatile char estado;

volatile uint8_t bandera_UP;
volatile uint8_t bandera_SS;
volatile uint8_t bandera_SS_corta;
volatile uint8_t bandera_SS_larga;
volatile uint8_t sel;
volatile uint8_t contador;
volatile uint8_t display_0;
volatile uint8_t display_1;
volatile uint64_t millis;			
volatile unsigned int display_1_7seg;
volatile unsigned int display_0_7seg;

volatile int T = 0;


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
	sel = 0;
}

void setup_inicio() {
	
	estado = 'i';
	
	bandera_UP = 0;
	bandera_SS = 0;
	bandera_SS_corta = 0;
	bandera_SS_larga = 0;
	contador = 0;
	millis = 0;
	
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
	
	
	//Input Capture (PE7)
	TCCR1B |= 1<<ICES1;
	TIMSK1 |= 1 << ICIE1;
	TIFR1 = 1 << ICES1;
	//--------------------------------------------------------------------------------------------------------
	
	
	
	
	//Las siguientes líneas de código son para habilitar las interrupciones de los botones SS y UP.
	PCICR = 1<<PCIE0;
	PCICR |= 1<<PCIE2;
	PCMSK0 |= 1<<PCINT5; // boton UP 
	PCMSK2 |= 1<<PCINT7; // boton SS 
	
	//Habilitamos el timer de refresco del display (modo CTC, preescalado 64)
	TCCR2A = 1<<WGM21;
	TCCR2B = 1<<CS20;
	TCCR2B |= 1<<CS21;
	TIMSK2 = 1<<OCIE2A;
	OCR2A = 62;
	
	//Se utiliza el timer 0 en modo ctc, con preescalado de 64 y con OCR1A = 125 (1ms)
	TCCR0A |= 1<<WGM01;
	TCCR0B |= 1<<CS00;
	TCCR0B |= 1<<CS01;
	TIMSK0 |= 1<<OCIE1A;
	OCR0A = 250;  // 0.001s / (1s / 16*10^6 ciclos)*64 = 250 ciclos
	
	//Seteamos entradas y salidas
	DDRD = 0x7F;		//Display
	DDRB = 1<<1;
	DDRB |= 1<<2;
	
	
	inicializa_display();
	actualiza_display();
	
	sei();
}


int main(void) {
   do{ 
		setup_inicio();
		do {
			if (bandera_UP == 1) {
				actualiza_contador();
				actualiza_display();
				bandera_UP = 0;
			}
		} while (bandera_SS == 0);
		bandera_SS = 0;
		main_principal();
	}while(1);
}


ISR(PCINT2_vect) {
	static uint32_t Trise = 0;
	static uint32_t Ton = 0;
	static uint8_t bandera_antirrebotes_SS = 0;
	static uint32_t T_SS = 0;
	
	if (bandera_antirrebotes_SS == 1) {
		if (millis - T_SS > 50) {   //Cambiar el tiempo a 5 ms para simulación
			bandera_antirrebotes_SS = 0;
		}
	}
	if (bandera_antirrebotes_SS == 0) {
		if (getBit(BOTON_SS, SS)) {
			Trise = millis;
			T_SS = millis;
			bandera_antirrebotes_SS = 1;
		} else {
			if (estado == 'i'){
				bandera_SS = 1;
			}
			if (estado == 's'){
				bandera_SS = 1;
			}
			if (estado == 'f'){
				bandera_SS = 1;
			}

			Ton = millis - Trise;
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

ISR(PCINT0_vect) {
	static uint8_t bandera_antirrebotes_UP = 0;
	static uint64_t T_UP = 0;
	
	if (bandera_antirrebotes_UP == 1) {
		if (millis - T_UP > 50) {        //Cambiar el tiempo a 5 ms para simulación
			bandera_antirrebotes_UP = 0;
		}
	}
	if (bandera_antirrebotes_UP == 0) {
		if (getBit(BOTON_UP, 5)) {
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

ISR(TIMER0_COMPA_vect){
	millis++;
}

ISR(TIMER2_COMPA_vect) {
	if (sel == 1) {
		clrBit(PORTB, 2);
		setBit(PORTB, 1);
		SEGMENTOS = display_1_7seg;
		sel = 0;
	} else {
		clrBit(PORTB, 1);
		setBit(PORTB, 2);
		SEGMENTOS = display_0_7seg;
		sel = 1;
	}
}