/*
 * principal.c
 *
 * Created: 06/05/2021 12:09:10
 *  Author: alvar
 */ 

volatile extern char estado;
volatile extern int bandera_UP;
volatile extern int bandera_SS;
volatile extern int bandera_apagar;
volatile extern int display_0;
volatile extern int display_1;

void setup_principal(){
	estado = 'p';
}

void apaga_display(){
	display_0 = 10;
	display_1 = 10;
}

void main_principal(){
	setup_principal();
	do {
		if (bandera_UP == 1) {
			actualiza_contador();
			actualiza_display();
			bandera_UP = 0;
		}
		if (bandera_apagar == 1) {
			apaga_display();
			actualiza_display();
			bandera_apagar = 0;
		}
	} while (bandera_SS == 0);
	bandera_SS = 0;
}