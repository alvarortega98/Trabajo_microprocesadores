/*
 * inicio.h
 *
 * Created: 06/05/2021 12:07:03
 *  Author: alvar
 */ 


#ifndef INICIO_H_
#define INICIO_H_

#define getBit(P, B) (P&(1<<B))
#define setBit(P, B) (P|=(1<<B))
#define clrBit(P, B) (P&=~(1<<B))
#define toggleBit(P, B) (P^=(1<<B))

#define BOTONES PINB
#define SS PCINT0
#define UP PCINT0
#define SEGMENTOS PORTD
#define SELECCION 2

void inicializa_display();
void actualiza_display();
void actualiza_contador();




#endif /* INICIO_H_ */