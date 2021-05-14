/*
 * inicio.h
 *
 * Created: 03/05/2021 10:37:05
 *  Author: alvar
 */ 

#define BOTONES PINE
#define SS 5
#define UP 6
#define SEGMENTOS PORTA
#define SELECCION 2

#define getBit(P, B) (P & (1 << B))
#define setBit(P, B) (P |= (1 << B))
#define clrBit(P, B) (P &= ~(1 << B))
#define toggleBit(P, B) (P ^= (1 << B))

void setup();
void actualiza_programa();
void actualiza_display();
void inicializa_display();

uint16_t get_temp_sensor();
uint16_t getWeight();
void main_principal();
void setup_principal();
void main_pausa();
