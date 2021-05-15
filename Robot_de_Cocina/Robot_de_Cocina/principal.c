/*
 * principal.c
 *
 * Created: 13/05/2021 12:07:25
 *  Author: alvar
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "RobotCocina.h"
#include "FakeAPI.h"


volatile extern char estado;

volatile extern uint8_t bandera_SS;
volatile extern uint8_t bandera_SS_corta;
volatile extern uint8_t bandera_SS_larga;

volatile extern uint16_t display_0;
volatile extern uint16_t display_1;
volatile extern uint8_t display_1_7seg;
volatile extern uint8_t display_0_7seg;

volatile extern uint64_t millis;
volatile extern uint8_t programa;

volatile extern uint8_t iterador;
volatile extern uint8_t buzzer_en;

//Variables para calcular, mediante el IC3, la temperatura medida por el sensor.
volatile uint16_t ovfl, ovfl_rise;
volatile uint16_t Ttemp = 10000;
volatile uint16_t temperatura;

volatile uint32_t deci;
volatile uint16_t secs;

//Variables para calcular, mediante el IC1, el peso medida por el sensor.
volatile uint16_t Ton;
volatile uint16_t weight;

//Variables para controlar la lógica del programam principal 
volatile uint8_t bandera_getAction;
volatile uint8_t bandera_getHeaterDutyCycle;

volatile uint16_t t_fin=111;


void setup_principal(){
	
	cli();
	
	//Se inicializan las variables a 0
	ovfl = 0;
	ovfl_rise = 0;
	deci = 0;
	secs = 0;
	
	//Se guarda el estado de programa principal
	estado = 'p';
	
//----TIMER 0: modo 7 y preescalado 8---------------------------------------
	//Función: controlar el PWM del motor

	//Modo 7: Fast PWM, non-inverting mode
	TCCR0A = (1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << WGM02);
	TCCR0A |= (1 << COM0B1);
	
	//Preescalado 8
	TCCR0B = (1 << CS01);

	//PG5 como salida:
	DDRG |= (1 << PG5);

	//Registros de comparación:
	OCR0B = 0.4*100;
	OCR0A = 100;


//----TIMER 1: modo 0 y preescalado 256 -----------------------------------------
	//Función: controlar getHeaterDutyCycle (cada 0.1 s), getAction (cada 0.5 s) y la alternancia de los displays en el programa principal
	//		   contar segundos
	//		   Realizar el parpadeo de los displays (para el modo pausa)
	//         Calcular el peso medido por el sensor (IC1)
	
	//Preescalado 256:
	TCCR1B = (1 << CS12);

	//Evento por flanco de subida en IC.
	TCCR1B |= (1 << ICES1);


	//Interrupciones por compare&match, IC.
	TIMSK1 = 1<<OCIE1A;
	TIMSK1 |= 1<<OCIE1B;
	TIMSK1 |= 1<<OCIE1C;
	TIMSK1 |= (1 << ICIE1);

	//Borrado de potencial bandera.
	TIFR1 = (1 << ICIE1);

	//Registros de comparación:
	OCR1A = 3124;				//0.1 sec, 3124
	OCR1B = 15624;				//0.5 sec, 15624
	OCR1C = 31249;				//1 sec, 31249

	
//----TIMER 3: modo 15 y preescalado 8----------------------------------------------------
	//Función: controlar el PWM de calentador (PE4) y calcular la temperatura medido por el sensor (IC3)
	
	//Modo 15
	TCCR3A |= 1<<WGM30;
	TCCR3A |= 1<<WGM31;
	TCCR3B |= 1<<WGM32;
	TCCR3B |= 1<<WGM33;
	
	//Preescalado 8
	TCCR3B |= 1<<CS31;
	
	//OC3B en modo COM3B=10
	TCCR3A |= 1<<COM3B1;
	DDRE |= 1<<PE4;
	
	//OCR3A = 20 ms, OCR3B d*20ms
	OCR3A = 20000;
	
	//Input Capture (PE7)
	TCCR3B |= 1<<ICES3;
	TIMSK3 |= 1 << ICIE3;
	TIFR3 |= 1 << TOIE3;
	TIMSK3 |= 1 << TOV3;

	sei();
}



void main_principal(){
	static uint16_t dM; //duty cycle motor
	static uint16_t dH; //duty cycle heater
	static uint16_t temperatura_deseada;
	
	
	setup_principal();
	
	do{
		if (bandera_SS_corta == 1){
			bandera_SS_corta = 0;
			main_pausa();
			estado = 'p';
		}
		if (bandera_SS_larga == 1){
			bandera_SS_corta = 0;
			break;
		}
		if (bandera_getAction == 1){
			weight = getWeight();
			getAction(programa, secs, weight);
			t_fin = getTimeToEnd();	
			dM = getMotorDutyCycle();
			OCR0B = dM * OCR0A / 1000;
			bandera_getAction = 0;
		}
		if (bandera_getHeaterDutyCycle == 1){
			temperatura = get_temp_sensor();
			temperatura_deseada = getTemperature();
			dH = getHeaterDutyCycle(temperatura_deseada, temperatura);
			OCR3B = dH * OCR3A / 1000;
			bandera_getHeaterDutyCycle = 0;
		}
		
	}while(t_fin > 0);
	
	if (t_fin == 0){
		main_fin_programa();
	} else {
		main_cancelar();
	}
}

//---TIMER1-------------------------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect) {
	
	deci++;
	//solo se ejecuta en elprograma principal
	if (estado == 'p'){
		bandera_getHeaterDutyCycle = 1;
	}
	
	//OCR1A += 3125;
	OCR1A += 3125;
}

ISR(TIMER1_COMPB_vect) {
	static uint8_t display_mode = 0;
	
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		bandera_getAction = 1;
	}
	
	
	//Parpadear displays (solo en modo pausa)
	if (estado == 's') {
		display_mode =~ display_mode;
		if (display_mode == 1){
			display_1 = (t_fin/60)/10;
			display_0 = (t_fin/60)%10;
		} else {
			display_1 = 10;
			display_0 = 10;
		}
		actualiza_display();
	}
		
	else if (estado == 'f') {
		iterador--;
		buzzer_en = ~buzzer_en;
		if (buzzer_en == 0) {
			DDRE &= (1<<PE4);
		} 
		else {
			DDRE |= 1<<PE4;
		}
	}
	OCR1B += 15625;	
}

ISR(TIMER1_COMPC_vect) {
	static uint8_t display_mode = 0;
	
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		if (estado == 'p'){
			display_mode =~ display_mode;
			if (display_mode){
				display_1 = (t_fin/60)/10;
				display_0 = (t_fin/60)%10;
				} else {
				display_1 = 11;
				display_0 = programa;
			}
			actualiza_display();
		}
		secs++;
	}
	OCR1C += 31250;
}

ISR ( TIMER1_CAPT_vect ) {
	static uint16_t Trise = 0;
	static uint16_t Tfall = 0;
	
	if (TCCR1B & (1 << ICES1)) {
		TCCR1B &= ~(1 << ICES1);
		Trise = ICR1;
		} else {
		TCCR1B |= (1 << ICES1);
		Tfall = ICR1;
		Ton = Tfall - Trise;	
	}
	 //NOTA: el peso máximoque puede medir el sensor es 1500gr, lo que equivale a una señal de 150000 us de periodo (0.15 segundos).
     //      como el top del timer 1 (2.1 segundos) es mayor que el mayor periodo de la señalno hace falta utilizar overflows en el código    
}

uint16_t getWeight() {
	return (Ton*32/100); //Ciclo_trabajo = (Ton * 256 / 8MHz) us
						 //peso = (Ciclo_trabajo(us) / 100) gramos
	
}
//---------------------------------------------------------------------------------------------------------------

//--IC Timer 3-------------------------------------------------------------------------------------------------------
ISR(TIMER3_CAPT_vect){
	static uint16_t Trise = 0;
	if (getBit(TCCR3B, ICES3)){
		clrBit(TCCR3B, ICES3);
		Ttemp = ICR3 - Trise;
		ovfl_rise = ovfl;
		Trise = ICR3;
		} else {
		setBit(TCCR3B, ICES3);
	}
	//NOTA: la temperatura de funcionemiento mínima es 20ºC, lo que equivale a una señal de 200Hz frecuencia, recibida del sensor,
	//	    esto equivale a una señal de periodo de 5 ms. Como el top del timer (OCR3A= 20ms) es mayor que el mayor periodo de la señal,
	//		no hace falta utilizar overflows en el código 	
}



uint16_t get_temp_sensor(){
	return (100000/Ttemp); //Periodo = (Ttemp * 8cilos/8MHz)us
					       //Frecuencia = ((1/Periodo) * 10^6)Hz
					       //Temperatura =(frequencia/10)				
}
//-------------------------------------------------------------------------------------------------------------------------------

