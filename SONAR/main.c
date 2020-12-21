/*
 * Proyecto SONAR
 *
 * Created: 12/7/2020 7:20:46 PM
 * Author : Tomás Tavella y Benjamin Gubkien
 *
 * Outputs: 	PortL 1 (Pin 48) para mandar el pulso al sensor (Trigger).
 * 	    		PortB 5 (Pin 11, OC1A) para mandar la PWM que maneja el servo.
 *				PortA 0 - PortA 7 (Pin 22 - Pin 29) para D0 - D7 del display (definido en hardware_const.h).
 *				PortB 0 (Pin 53) para el Register Select (RS) del display.
 *				PortB 1 (Pin 52) para el Enable (E) del display.
				GND para el Register Write (RW).
 *
 * Inputs:  	PortD 0 (Pin 21, INT0) para recibir el pulso de echo del sensor (Echo).
 *
 * Timers:	Timer 0 (8 bits) en modo Fast PWM con overflow en TOP = OCR0A, para medir periodos de 5 us del pulso de Echo.
 *			Timer 1 (16 bits) en modo PWM PFC para controlar el servo.
 *			Timer 3 (16 bits) en modo Fast PWM con overflow en TOP = ICR3 para contar tiempo entre pulsos mandados al sensor.
 *	
 */ 

#include "sonar.h"
#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned int flag_sensor;					// Flag que se setea al terminar el pulso de eco, y se resetea luego de escribir en pantalla.
unsigned int count_5us;						// Variable contador que lleva la cuenta de las iteraciones del Timer 0 de 5us.

int main(void)
{
	sonar_setup();																			// Setea los timers, interrupts y al display.
	lcd_setup();																			// Setea el display LCD.
	sei();																					// Activa las interrupciones globales.
	
	while (1)
	{	
		for(OCR1A = servo_0deg; OCR1A <= servo_180deg; OCR1A += servo_paso){				// Rotación del servo, aumentando OCR1A con t_paso, cada 'ms_servo' milisegundos.
			if (flag_sensor == 0){															// Si el flag está bajo...
				_delay_ms(delay_servo_ms);													// Sólo se realiza el delay para el servo.
			}else{																				
				process_param(count_5us, OCR1A);											// Si el flag está alto, procesa los parámetros de distancia y ángulo y los escribe en el display.
				flag_sensor = 0;															// Resetea la flag del sensor.
				count_5us = 0;																// Resetea el contador de iteraciones.
			}
		}
		for( OCR1A = servo_180deg; OCR1A >= servo_0deg; OCR1A -= servo_paso){				// Rotación del servo en el otro sentido, decrementando OCR1A con t_paso, cada 'ms_servo' milisegundos.
			if (flag_sensor == 0){															// Condicional con el mismo comportamiento que el primer loop.
				_delay_ms(delay_servo_ms);						
			}else{
				process_param(count_5us, OCR1A);
				flag_sensor = 0;		
				count_5us = 0;						
			}
		}
		
	}
}

/*============================== INTERRUPCIONES ======================*/

/*
  Nombre:		TIMER3_OVF_vect
  Fuente:		Flag de overflow del Timer 3.
  Propósito:	Mandar un nuevo pulso al sensor luego de pasar un cierto tiempo (200ms en este caso).
*/

ISR(TIMER3_OVF_vect){					// Vector de interrupcion del overflow del timer 3
	trigger_pulse();					// Activa el sensor de ultrasonido para enviar un pulso.
}

/*...........................................................................*/

/*
  Nombre:		TIMER0_OVF_vect
  Fuente:		Flag de overflow del Timer 0 (8 bits).
  Propósito:	Contar intervalos de 5us de duracion mientras el pin de Echo del sensor este en alto.
*/

ISR(TIMER0_OVF_vect){
	count_5us++;						// Aumenta un contador que lleva la cuenta de cuantos intervalos de 5us pasaron.
}

/*...........................................................................*/

/*
  Nombre:		INT0_vect
  Fuente:		Interrupcion externa 0 del PIND0 (Pin 21 del Arduino).	
  Propósito:	Activar y desactivar el Timer 0 cuando llega el flaco de subida y
				bajada de Echo, respectivamente.
*/

ISR(INT0_vect){
	if (PIND & (1 << PIND0)){				// Chequea que el PIND0 (Echo del sensor) esté en alto.
		TCCR0B |= (1 << CS00);				// Si se cumple, activa el Timer 0 que cuenta de a 5us.
	}else{
		TCCR0B &= ~(1 << CS00);				// Caso contrario, al llegar al flanco de bajada del pulso, apaga el timer
		flag_sensor = 1;					// Sube la flag.
	}
}