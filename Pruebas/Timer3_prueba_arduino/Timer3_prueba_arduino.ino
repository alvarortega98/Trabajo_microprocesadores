#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define getBit(P, B) (P & (1 << B))
#define setBit(P, B) (P |= (1 << B))
#define clrBit(P, B) (P &= ~(1 << B))
#define toggleBit(P, B) (P ^= (1 << B))

volatile uint16_t ovfl, ovfl_rise;
volatile uint16_t Ttemp = 10;
volatile uint16_t temperatura;

void setup() {
   Serial.begin(9600);
//Timer 0:mode 7, preescalado 256;
  cli();
  
  TCCR0A = (1<<WGM00);
  TCCR0A |= (1<<WGM01);
  TCCR0A |= (1<<COM0B1);

  DDRD = 1<<PD5;

  TCCR0B = (1<<CS00);
  TCCR0B |= (1<<CS01);
  TCCR0B |= (1<<WGM02);

  OCR0A = 199;
  OCR0B = 100;




//Timer 1: mode 0, preescalado 8, 


  //preescalado 8
  TCCR1B |= 1<<CS11;
  /*
  //OC1B en modo COM1B=10

  TCCR1A |= 1<<COM1B1;
  DDRB |= 1<<PB2;
  TIMSK1 |= 1<<OCIE1B;
  TIMSK1 |= 1<<OCIE1A;
  
  //OCR3A = 20 ms, OCR3B d*20ms

  OCR1B = 2000*0.5; //cambiar a 200 parar simulación
  OCR1A = 2000;*/
  
  //Input Capture (PE7)
  TCCR1B |= 1<<ICES1;
  TIMSK1 |= 1 << ICIE1;
  TIFR1 = 1 << ICES1;
  sei();
  delay(100);
}



void loop() {
   delay(500);
   temperatura = get_temp_sensor();
   Serial.println(temperatura);
}

ISR(TIMER1_CAPT_vect){
  static uint16_t Trise = 0;
  if (getBit(TCCR1B, ICES1)){
    clrBit(TCCR1B, ICES1);
    if( ICR1 > Trise){
      Ttemp = ICR1 - Trise;
    }else{
    }
    Trise = ICR1;
  } else {
    setBit(TCCR1B, ICES1);
  }   
}



uint16_t get_temp_sensor(){
  volatile uint16_t T;
  T =  6250/Ttemp; //Periodo = (Ttemp * 256cilos/16MHz)us
                   //Frecuencia = (1/Periodo * 10^6)Hz
                   //Temperatura =(frequencia/10)
  //temp = freq/10
  return T;
}
