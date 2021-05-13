/*
 * FakeAPI.h
 *
 * Created: 29/03/2021 18:51:25
 *  Author: Yago
 */ 

// Las funciones implementadas son ficticias, y lo �nico que hacen es perder tiempo
// tal y como est� especificado en el documento de especificaciones de cada trabajo
// y generar, si la funci�n lo requiere, unos valores tambi�n ficticios.


#ifndef FAKEAPI_H_
#define FAKEAPI_H_

#include <stdint.h>


// Para calcular la acci�n a realizar. La funci�n recibe:
// El n�mero del programa a ejecutar (entre 0 y 9)
// El tiempo transcurrido desde el inicio del programa en segundos
// El peso de los alimentos en gramos
// Esta funci�n debe llamarse 2 veces por segundo
// Bloqueante (50 ms), interrumpible
void getAction(uint8_t program, uint16_t seconds, uint16_t weight);

// Funciones para acceder a los resultados de getAction() (muy cortas)
uint16_t getMotorDutyCycle();

// Devuelve el ciclo de trabajo en tanto por mil
uint16_t getTemperature();

// Devuelve la temperatura a aplicar en �C
uint16_t getTimeToEnd();

// Tiempo que falta para finalizar en segundos
// Para calcular el ciclo de trabajo a aplicar al calefactor:
// Recibe la temperatura actual y la objetivo en d�cimas de grado
// Devuelve el ciclo de trabajo en tanto por mil
// Debe ser llamada 10 veces por segundo
// Bloqueante (unos 2000 ciclos), interrumpible
uint16_t getHeaterDutyCycle(uint16_t Ttarget, uint16_t Tcurrent);



#endif /* FAKEAPI_H_ */