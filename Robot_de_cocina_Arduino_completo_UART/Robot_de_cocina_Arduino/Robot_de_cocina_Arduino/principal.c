/*
 * principal.c
 *
 * Created: 14/05/2021 8:22:36
 *  Author: alvar
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART_irq.h"
#include <util/delay.h>
#include "RobotCocina.h"
#include "fakeAPI.h"

static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit_IO, NULL,_FDEV_SETUP_WRITE);

volatile extern char estado;
volatile extern uint8_t bandera_SS_corta;
volatile extern uint8_t bandera_SS_larga;
volatile extern uint8_t contador;
volatile extern uint8_t display_0;
volatile extern uint8_t display_1;
volatile extern uint64_t millis;
volatile extern uint8_t iterador;
volatile uint8_t buzzer_en;

volatile uint16_t t_fin;
volatile uint32_t deci;
volatile uint16_t secs;

//Variables para calcular, mediante el IC3, la temperatura medida por el sensor.
volatile uint16_t ovfl, ovfl_rise;
volatile uint16_t Ttemp = 10;
volatile uint16_t temperatura;

//Variables para calcular, mediante el IC1, el peso medida por el sensor.
volatile uint16_t Ton;
volatile uint16_t weight;

//Variables para controlar la lógica del programam principal
volatile uint8_t bandera_getAction;
volatile uint8_t bandera_getHeaterDutyCycle;


void setup_principal(){
	USART_Init(MYUBRR);
	stdout = &mystdout;
	
	
	estado = 'p';
	if (contador == 1){
		t_fin = 600;
	} else {
		t_fin = 3600;
	}
	deci = 0;
	secs = 0;
	buzzer_en = 0;
	

	
	cli();
	
	//----TIMER 2: modo 7 y preescalado 8---------------------------------------
	//Función: controlar el PWM del motor

	//Modo 7: Fast PWM, non-inverting mode
	TCCR2A = (1 << WGM20);
	TCCR2A |= (1 << WGM21);
	TCCR2A |= (1 << COM0A1);
	
	//Preescalado 8
	TCCR2B = (1 << CS21);
	
	DDRB |= 1<<PINB3;
	
	
	//Interrupciones por compare&match, IC.
	TIMSK2 |= 1<<OCIE2A;
	
						
	//--------------------------------------------------------------------------------------------------------
	
	//interrupcion ICP2
	PCICR |= 1<<PCIE1;
	PCMSK1 |= 1<<PCINT8; //ICP2
	sei();
}



void main_principal(){
	static uint16_t dM; //duty cycle motor
	static uint16_t dH; //duty cycle heater
	static uint16_t temperatura_deseada;
	
	
	setup_principal();
	  _delay_ms(100);
	do{
		if (bandera_SS_corta == 1){
			bandera_SS_corta = 0;
			main_pausa();
			estado = 'p';
		}
		if (bandera_SS_larga == 1){
			bandera_SS_larga = 0;
			break;
		}
		if (bandera_getAction == 1){
			weight = getWeight();
			getAction(contador, secs, weight);
			t_fin = getTimeToEnd();
			dM = getMotorDutyCycle();
			OCR2A = dM * 255 / 1000;
			bandera_getAction = 0;
		}
		if (bandera_getHeaterDutyCycle == 1){
			temperatura = get_temp_sensor();
			temperatura_deseada = getTemperature();
			dH = getHeaterDutyCycle(temperatura_deseada, temperatura);
			//OCR3B = dH * OCR3A / 1000;
			bandera_getHeaterDutyCycle = 0;
		}
		
		printf("temperatura = %u\n", weight);
	}while(t_fin > 0);
	
	if (t_fin > 0){
		main_cancelar();
		} else {
		main_fin_programa();
	}
}


ISR(TIMER1_COMPA_vect) {
	static uint8_t display_mode = 0;
	deci++;
	if (estado == 'p'){
		bandera_getHeaterDutyCycle = 1;
	}
	if ((deci % 5) == 0){
		if (estado == 'p'){
			bandera_getAction = 1;
		}
	
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
	}
	//------------------------------------------
	//OCR1A += 15625;
	OCR1A += 6250;	
}

ISR(TIMER1_COMPB_vect) {
	static uint8_t display_mode_p = 0;
	secs++;
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		//t_fin -= 60;
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
	else if (estado == 'f') {
		iterador--;
		buzzer_en = ~buzzer_en;
		if (buzzer_en == 0) {
			DDRB &= ~(1<<PINB3);
		}
		else {
			DDRB |= 1<<PINB3;
		}
	}
	OCR1B += 62500;
	//OCR1B += 31250;
}


ISR ( TIMER1_CAPT_vect ) {
	static uint16_t Trise = 0;
	static uint16_t Tfall = 0;
	
	if (TCCR1B & (1 << ICES1)) {
		TCCR1B &= ~(1 << ICES1);
		Trise = TCNT2;
		} else {
		TCCR1B |= (1 << ICES1);
		Tfall = TCNT2;
		Ton = Tfall - Trise;
	}
	//NOTA: el peso máximoque puede medir el sensor es 1500gr, lo que equivale a una señal de 150000 us de periodo (0.15 segundos).
	//      como el top del timer 1 (2.1 segundos) es mayor que el mayor periodo de la señalno hace falta utilizar overflows en el código
}

uint16_t getWeight() {
	return (Ton*100); //Ciclo_trabajo = (Ton * 256 / 8MHz) u

	//peso = (Ciclo_trabajo(us) / 100) gramos
	
}


ISR(PCINT1_vect){
	static uint16_t Trise = 0;
	if (getBit(PCINT1, IC2)){
		Ttemp = TCNT2 - Trise;
		Trise = TCNT2;
		} else {
	}
	//NOTA: la temperatura de funcionemiento mínima es 20ºC, lo que equivale a una señal de 200Hz frecuencia, recibida del sensor,
	//	    esto equivale a una señal de periodo de 5 ms. Como el top del timer (OCR3A= 20ms) es mayor que el mayor periodo de la señal,
	//		no hace falta utilizar overflows en el código
}



uint16_t get_temp_sensor(){
	return (Ttemp); //Periodo = (Ttemp * 8cilos/8MHz)us
	//Frecuencia = ((1/Periodo) * 10^6)Hz
	//Temperatura =(frequencia/10)
}