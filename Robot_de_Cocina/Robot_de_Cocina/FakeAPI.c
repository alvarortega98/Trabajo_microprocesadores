/*
 * FakeAPI.c
 *
 * Created: 29/03/2021 18:52:08
 *  Author: Yago
 */ 

#include "FakeAPI.h"
#include <avr/interrupt.h>

#ifndef F_CPU
  #define F_CPU 8000000UL
#endif

#define CYCLES_PER_ITERATION  34 // Obtained through simulation. Check
#define NS_PER_ITERATION      (CYCLES_PER_ITERATION*(1000000000UL/F_CPU))
#define BLOCKING_LOOP(N)      for(volatile long int i = 0; i < N; i++)
#define BLOCKING_DELAY_MS(T)  BLOCKING_LOOP(T*1000000UL/NS_PER_ITERATION/SPEED_UP_FACTOR)

////////////////////////////////////////////
// Comunes
void setupComm() {
	cli();
	BLOCKING_LOOP(10);
	sei();
}

////////////////////////////////////////////
// Detector de monedas

void sendMoney(char * total) {
	BLOCKING_DELAY_MS(50);
}

char data[] = "Received data";
char * receiveData() {
	BLOCKING_DELAY_MS(50);
	return data;
}

////////////////////////////////////////////
// Lector de códigos de barras

void sendCode(char * code){
	BLOCKING_LOOP(4);
}

uint16_t getSpinDutyCycle(uint16_t rpm) {
	BLOCKING_LOOP(500/CYCLES_PER_ITERATION);
	uint16_t duty = ((2000 - rpm) >> 1) - 1; // Control proporcional fake
	duty = (duty < 0 ? 0 : duty); // El ciclo de trabajo no puede ser negativo
	return duty; 
}

////////////////////////////////////////////
// Robot de cocina

static uint16_t motorDutyCycle;
static uint16_t temperature;
static uint16_t timeToEnd;

void getAction(uint8_t program, uint16_t seconds, uint16_t weight) {
	BLOCKING_DELAY_MS(50);
	motorDutyCycle = 100 * program;
	temperature = 15 * (9 - program);
	timeToEnd = (weight >> 1) / (10 - program);
	timeToEnd = (timeToEnd > seconds ? timeToEnd - seconds : 0);
}

uint16_t getMotorDutyCycle() { return motorDutyCycle; }

uint16_t getTemperature() { return temperature; }

uint16_t getTimeToEnd() { return timeToEnd; }

uint16_t getHeaterDutyCycle(uint16_t Ttarget, uint16_t Tcurrent) {
	BLOCKING_LOOP(2000/CYCLES_PER_ITERATION);
	uint16_t duty = (Ttarget - Tcurrent) * 100 / 15 - 1;  // Control proporcional fake
	duty = (duty < 0 ? 0 : duty); // El ciclo de trabajo no puede ser negativo
	return duty;
}