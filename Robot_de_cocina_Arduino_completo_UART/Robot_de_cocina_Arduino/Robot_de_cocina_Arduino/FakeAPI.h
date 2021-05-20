/*
 * FakeAPI.h
 *
 * Created: 29/03/2021 18:51:25
 *  Author: Yago
 */ 

// Las funciones implementadas son ficticias, y lo único que hacen es perder tiempo
// tal y como está especificado en el documento de especificaciones de cada trabajo
// y generar, si la función lo requiere, unos valores también ficticios.


#ifndef FAKEAPI_H_
#define FAKEAPI_H_

#include <stdint.h>

// Las simulaciones pueden resultar muy lentas si se implementan los retardos reales.
// La constante SPEED_UP_FACTOR permite reducir el tiempo de simulación de las funciones 
// en el factor indicado. Se recomienda empezar con un valor de 100, para que la simulación
// sea rápida, y una vez que se piense que el sistema funciona bien, probrar bajándolo
// a 10 y ver si los retardos afectan al funcionamiento, y finalmente a 1 (ahí, cada 
// llamada a una función que suponga un retardo de 50 ms puede tardar más de un minuto!!)
#define SPEED_UP_FACTOR 100

// Hace el setup de las comunicaciones (bloqueante, no interrumpible)
void setupComm();

// Envía el código detectado (código ASCII de los 4 dígitos)
// No bloqueante.
void sendCode(char * code);

// Devuelve un número entre 0 y 999 correspondiente al ciclo
// de trabajo a aplicar al motor en función de la velocidad actual en RPM
// Debe llamarse a intervalos regulares cada 200 ms.
// Bloqueante (unos 500 ciclos) Interrumpible
uint16_t getSpinDutyCycle(uint16_t rpm);

// Para enviar la información del dinero recaudado (4 dígitos en código ASCII)
// 3 dígitos para centenas, decenas y unidades de euro, y 1 dígito para
// las decenas de céntimos (no se admiten monedas de 2 y 5 céntimos)
// Bloqueante (unos 50 ms), interrumpible
void sendMoney(char * total);

// Para recibir datos del ordenador y operar en consecuencia
// Bloqueante (unos 50 ms), interrumpible
char * receiveData();

// Para calcular la acción a realizar. La función recibe:
// El número del programa a ejecutar (entre 0 y 9)
// El tiempo transcurrido desde el inicio del programa en segundos
// El peso de los alimentos en gramos
// Esta función debe llamarse 2 veces por segundo
// Bloqueante (50 ms), interrumpible
void getAction(uint8_t program, uint16_t seconds, uint16_t weight);

// Funciones para acceder a los resultados de getAction() (muy cortas)
uint16_t getMotorDutyCycle();

// Devuelve el ciclo de trabajo en tanto por mil
uint16_t getTemperature();

// Devuelve la temperatura a aplicar en ºC
uint16_t getTimeToEnd();

// Tiempo que falta para finalizar en segundos
// Para calcular el ciclo de trabajo a aplicar al calefactor:
// Recibe la temperatura actual y la objetivo en décimas de grado
// Devuelve el ciclo de trabajo en tanto por mil
// Debe ser llamada 10 veces por segundo
// Bloqueante (unos 2000 ciclos), interrumpible
uint16_t getHeaterDutyCycle(uint16_t Ttarget, uint16_t Tcurrent);



#endif /* FAKEAPI_H_ */