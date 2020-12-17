/*
 * Proyecto SONAR
 *
 * Created: 12/7/2020 7:20:46 PM
 * Author : Tomas Tavella y Benjamin Gubkien
 *
 * Outputs: 	PortL 1 (Pin 48) para mandar el pulso al sensor
 * 	    	PortB 5 (Pin 11, OC1A) para mandar la PWM que maneja el servo
 *		PortA 0 - PortA 7 (Pin 22 - Pin 29) para D0 - D7 del display (definido en hardware_const.h)
 *		PortB 0 (Pin 53) para el Register Select (RS) del display
 *		PortB 1 (Pin 52) para el Enable (E) del display
 *
 * Inputs:  	PortL 0 (Pin 49, ICP4) para recibir el pulso de echo del sensor
 *
 * Timers:	Timer 1 (16 bits) en modo PWM PFC para controlar el servo
 *		Timer 3 (16 bits) en modo Fast PWM con overflow en TOP = ICR3 para contar tiempo entre pulsos mandados al sensor
 *		Timer 4 (16 bits) en modo Input Capture para medir el tiempo entre mandar un pulso al sensor y que el mismo devuelva un pulso, 
 *				  que es proporcional a la distancia medida en cm
 */ 

// Definiciones de constantes usadas en el programa

#include "sonar.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

volatile int retorno_sensor=0;
volatile int flag_sensor=0;

// Comienzo del main
int main(void)
{
	sonar_setup();
	lcd_setup();
	sei();
	while (1)
	{
		for( OCR1A = t_0grados; OCR1A <= t_180grados; OCR1A = OCR1A + t_paso){				// Rotacion del servo, aumentando OCR1A con t_paso, cada ms_servo milisegundos
			if (flag_sensor == 0){
				_delay_ms(ms_servo);
			}else{
				volatile int pulse_width = OCR1A;											// Si flag_sensor es 1 (es decir que ocurrio el input capture en el timer 4)
				dist_calc(retorno_sensor, pulse_width);										// llamo a la funcion que calcula angulo y distancia, y los escribe en el display
				flag_sensor=0;																// Reseteo la flag del sensor
			}
		}
		for( OCR1A = t_180grados; OCR1A >= t_0grados; OCR1A = OCR1A - t_paso){				// Rotacion del servo en el otro sentido, decrementando OCR1A con t_paso, cada ms_servo milisegundos
			if (flag_sensor == 0){
				_delay_ms(ms_servo);							
			}else{
				volatile int pulse_width = OCR1A;											// Se repite lo mismo que el giro en el otro sentido
				dist_calc(retorno_sensor, pulse_width);
				flag_sensor=0;
			}
		}
	}
}

/*============================== INTERRUPCIONES ======================*/

/*
  Nombre:	TIMER3_OVF_vect
  Fuente:	Flag de overflow del timer 3	
  Propósito:	Mandar un nuevo pulso al sensor luego de pasar un cierto tiempo (200 ms en este caso)
*/

ISR(TIMER3_OVF_vect){					// Vector de interrupcion del overflow del timer 3
	trigger_pulse();
}

/*...........................................................................*/

/*
  Nombre:	TIMER4_CAPT_vect
  Fuente:	Flag de input capture del timer 4
  Propósito:	Almacernar el valor de ICR4 en una variable global, activar la flag indicando la entrada a la subrutina
				y reseteo de los contadores del timer
*/

ISR(TIMER4_CAPT_vect)							// Vector de interrupción de input capture para el Timer 4.
{	
	retorno_sensor = ICR4;						// Almaceno el valor de ICR4 en una variable global
	flag_sensor = 1;							// Seteo la flag que indica que el sensor devolvio un pulso
	TCCR4B |= (0<<CS41);						// Freno el timer.
	ICR4 = 0;									// Limpio los registros contadores.
	TCNT4 = 0;
}
