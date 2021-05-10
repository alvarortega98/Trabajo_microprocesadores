/*
 * Prueba_inicio_arduino.c
 *
 * Created: 02/05/2021 17:40:22
 * Author : alvar
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>

#define getBit(P, B) (P&(1<<B))
#define setBit(P, B) (P|=(1<<B))
#define clrBit(P, B) (P&=~(1<<B))
#define toggleBit(P, B) (P^=(1<<B))

#define BOTONES PINB
#define SS PCINT0
#define UP PCINT0
#define SEGMENTOS PORTD
#define SELECCION 2



volatile uint16_t bandera_UP = 0;
volatile uint16_t bandera_SS = 0;
volatile uint16_t sel;
volatile uint16_t contador = 0;
volatile uint16_t display_0;
volatile uint16_t display_1;
volatile int millis = 0;			
volatile unsigned int display_1_7seg;
volatile unsigned int display_0_7seg;

volatile uint16_t bandera_antirrebotes = 0;	//Esto puede estar mal (ver debug)
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

void setup() {
	
	cli();
	
	//Las siguientes líneas de código son para habilitar las interrupciones de los botones SS y UP.
	PCICR = 1<<PCIE0;
	PCMSK0 |= 1<<PCINT0; // boton UP (PB2)
	
	//Habilitamos el timer de refresco del display (modo CTC, preescalado 8)
	TCCR2A = 1<<WGM21;
	TCCR2B = 1<<CS21;
	TIMSK2 = 1<<OCIE2A;
	OCR2A = 250;
	
	//Se utiliza el timer 0 en modo ctc, con preescalado de 64 y con OCR1A = 125 (1ms)
	TCCR0A |= 1<<WGM01;
	TCCR0B |= 1<<CS00;
	TCCR0B |= 1<<CS01;
	TIMSK0 |= 1<<OCIE1A;
	OCR0A = 125;  // 0.001s / (1s / 8*10^6 ciclos)*64 = 125 ciclos
	
	//Seteamos entradas y salidas
	DDRD = 0xFF;		//Display
	DDRB = 1<<1;
	DDRB |= 1<<2;
	
	
	inicializa_display();
	actualiza_display();
	
	sei();
}


int main(void) {
    
	setup();
	
	do {
		if (bandera_UP == 1) {
			actualiza_contador();
			actualiza_display(); 
			bandera_UP = 0;
		}
	} while (bandera_SS == 0);
	bandera_SS = 0;
		
	
	
}


/*ISR(PCINT0_vect) {
	static bool bandera_antirrebotes = 0;	//Esto puede estar mal (ver debug)
	static int T = 0;
	
	if (bandera_antirrebotes == 1) {
		if (millis - T > 50) {
			bandera_antirrebotes = 0;
		}
	}
	if (bandera_antirrebotes == 0) {
		if (getBit(BOTONES, SS) == 1) {
			T = millis;
			bandera_antirrebotes = 1;
		} else {
			bandera_SS = 1;
			T = millis;
			bandera_antirrebotes = 1;
		}
	}
	
}*/

ISR(PCINT0_vect) {
	
	if (bandera_antirrebotes == 1) {
		if (millis - T > 50) {
			bandera_antirrebotes = 0;
		}
	}
	if (bandera_antirrebotes == 0) {
		if (getBit(BOTONES, UP) == 1) {
			T = millis;
			bandera_antirrebotes = 1;
			bandera_UP = 1;
			} else {
			
			T = millis;
			bandera_antirrebotes = 1;
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

