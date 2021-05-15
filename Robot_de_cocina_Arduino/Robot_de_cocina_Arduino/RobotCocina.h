/*
 * RobotCocina.h
 *
 * Created: 14/05/2021 8:21:54
 *  Author: alvar
 */ 


#ifndef ROBOTCOCINA_H_
#define ROBOTCOCINA_H_

#define getBit(P, B) (P&(1<<B))
#define setBit(P, B) (P|=(1<<B))
#define clrBit(P, B) (P&=~(1<<B))
#define toggleBit(P, B) (P^=(1<<B))

#define BOTON_UP PINB
#define BOTON_SS PIND
#define SS PCINT23
#define UP PCINT0
#define SEGMENTOS PORTD
#define SELECCION 2

void inicializa_display();
void actualiza_display();
void actualiza_contador();
void main_principal();
void main_pausa();
void main_fin_programa();
void main_cancelar();



#endif /* ROBOTCOCINA_H_ */