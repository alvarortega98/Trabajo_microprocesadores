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
volatile extern uint64_t millis;
volatile extern uint8_t display_1_7seg;
volatile extern uint8_t display_0_7seg;
volatile extern uint8_t contador;

volatile uint16_t ovfl, ovfl_rise = 0;
volatile uint16_t Ttemp = 10000;
volatile uint16_t temperatura;

volatile uint32_t deci = 0;
volatile uint16_t secs = 0;
volatile uint16_t Ton;
volatile uint8_t flag_gA;
volatile uint8_t flag_H;
volatile uint16_t weight;

volatile uint16_t t_fin=111;


void setup_principal(){
	cli();
	
	estado = 'p';
	
//---TIMER 0-------------------------------------------------------------------------------------------------	
	//Preescalado 8:
	TCCR0B = (1 << CS01);

	//Modo 7: Fast PWM, non-inverting mode
	TCCR0A = (1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << WGM02);
	TCCR0A |= (1 << COM0B1);

	//PG5 como salida:
	DDRG |= (1 << PG5);

	OCR0B = 0.4*100;
	OCR0A = 100;
//--------------------------------------------------------------------------------------------------------

//---Timer 1------------------------------------------------------------------------------------------------
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
	OCR1A = 31;				//0.1 sec, 3124
	OCR1B = 156;				//0.5 sec, 15264
	OCR1C = 312;				//1 sec, 31249
//--------------------------------------------------------------------------------------------------------
	
//---TIMER 3-------------------------------------------------------------------------------------------------

	//Timer 3: mode 15, preescalado 8, activo OC3B en modo COM3B=10, OCR3A = 20 ms, OCR3B d*20ms
	
	//mode 15
	TCCR3A |= 1<<WGM30;
	TCCR3A |= 1<<WGM31;
	TCCR3B |= 1<<WGM32;
	TCCR3B |= 1<<WGM33;
	
	//preescalado 8
	TCCR3B |= 1<<CS31;
	
	//OC3B en modo COM3B=10

	TCCR3A |= 1<<COM3B1;
	DDRE |= 1<<PE4;
	
	//OCR3A = 20 ms, OCR3B d*20ms

	OCR3B = 20000*0.4/100; //cambiar a 200 parar simulación
	OCR3A = 20000;
	
	//Input Capture (PE7)
	TCCR3B |= 1<<ICES3;
	TIMSK3 |= 1 << ICIE3;
	TIFR3 |= 1 << TOIE3;
	TIMSK3 |= 1 << TOV3;
//---------------------------------------------------------------------------------------------------------------


	
	sei();
}



void main_principal(){
	static uint16_t dM;
	static uint16_t dH;
	static uint16_t temperatura_deseada;
	
	
	setup_principal();
	
	do{
		if (bandera_SS_corta == 1){
			bandera_SS_corta = 0;
			main_pausa();
		}
		if (bandera_SS_larga == 1){
			bandera_SS_corta = 0;
			break;
		}
		if (flag_gA == 1){
			weight = getWeight();
			getAction(contador, secs, weight);
			t_fin = getTimeToEnd();	
			dM = getMotorDutyCycle();
			OCR0B = dM * OCR0A / 1000;
			flag_gA = 0;
		}
		if (flag_H == 1){
			temperatura = get_temp_sensor();
			temperatura_deseada = getTemperature();
			dH = getHeaterDutyCycle(temperatura_deseada, temperatura);
			OCR3B = dH * OCR3A / 1000;
			flag_H = 0;
		}
		
	}while(t_fin > 0);
	
	if (t_fin == 0){
		//main_fin_programa();
	} else {
		//main_cancelar();
	}
}

//---TIMER1-------------------------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect) {
	//solo se ejecuta en elprograma principal
	if (estado == 'p'){
		deci++;
		flag_H = 1;
	}
	
	//OCR1A += 3125;
	OCR1A += 3125;
}

ISR(TIMER1_COMPB_vect) {
	static uint8_t display_mode = 0;
	
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		flag_gA = 1;
	}
	
	
//--Modo pausa, parpadear displays ---------
	if (estado == 's'){
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
//------------------------------------------
	//OCR1B += 15625;
	OCR1B += 15625;
	
}

ISR(TIMER1_COMPC_vect) {
	//solo se ejecuta en el programa principal
	if (estado == 'p'){
		//change displays
		secs++;
	}
	OCR1C += 31250;
	//OCR1C += 31250;
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
	
}

uint16_t getWeight() {
	return (Ton*32/100);
	//Es entre 32 por el preescalado, que es de 256.
}
//---------------------------------------------------------------------------------------------------------------

//--IC Timer 3-------------------------------------------------------------------------------------------------------
ISR(TIMER3_CAPT_vect){
	static uint16_t Trise = 0;
	if (getBit(TCCR3B, ICES3)){
		clrBit(TCCR3B, ICES3);
		Ttemp = TCNT3 - Trise + 0x10000 * (ovfl - ovfl_rise);
		ovfl_rise = ovfl;
		Trise = TCNT3;
		} else {
		setBit(TCCR3B, ICES3);
	}
	
	
}

ISR(TIMER3_OVF_vect){
	ovfl++;
}

uint16_t get_temp_sensor(){
	volatile uint32_t T;
	T =  800000/((uint32_t) Ttemp); //freq = (1/(Ttemp/8)) * 10^6
	//temp = freq/10
	return (uint16_t) T;
}
//-------------------------------------------------------------------------------------------------------------------------------

