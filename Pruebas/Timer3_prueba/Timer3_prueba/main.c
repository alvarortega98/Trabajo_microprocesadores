/*
 * Timer3_prueba.c
 *
 * Created: 06/05/2021 18:13:33
 * Author : alvar
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define getBit(P, B) (P & (1 << B))
#define setBit(P, B) (P |= (1 << B))
#define clrBit(P, B) (P &= ~(1 << B))
uint16_t get_temp_sensor();

uint16_t ovfl, ovfl_rise = 0;

volatile uint16_t Ttemp = 10000;
volatile uint16_t temperatura;

volatile uint16_t d = 50;

void setup(){
	
	cli();
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

	OCR3B = 20000*d/100; //cambiar a 200 parar simulación
	OCR3A = 20000;
	
	//Input Capture (PE7)
	TCCR3B |= 1<<ICES3;
	TIMSK3 |= 1 << ICIE3;	
	TIFR3 |= 1 << TOIE3;
	TIMSK3 |= 1 << TOV3;
	
	sei();
}

int main(void){
	setup();
	while (1) {
		for (int i=0; i<156;){
			i++;
		}
		temperatura = get_temp_sensor();
	}
}

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

