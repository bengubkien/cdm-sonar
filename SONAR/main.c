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
 *		Timer 4 (16 bits) en modo Input Capture para medir el tiempo que es proporcional a la distancia en cm
 */ 

// Definiciones de constantes usadas en el programa

#include "lcd.h"
#include "sonar.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Comienzo del main
int main(void)
{
	sonar_setup();
	lcd_init_8d();
	while (1)
	{
		servo_rotation();
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
  Propósito:	Medir el tiempo en us desde que se mando el pulso hasta que retorno el sensor por echo (Valor de ICR4/2),
  		obtener la distancia a partir del valor de tiempo, y escribir esto y el angulo en el display
*/

ISR(TIMER4_CAPT_vect)					// Vector de interrupción de input capture para el Timer 4.
{
	TCCR4B |= (0<<CS41);				// Freno el timer.
	int dist_cm = ICR4 / (2*58);			// Una cuenta de 2 equivale a 1 us con 8 de prescaler. La cuenta para la distancia en cm es t_us/58 = dist_cm  ==>  count/(2*58) = dist_cm.
	ICR4 = 0;					// Limpio los registros contadores.
	TCNT4 = 0;
	int angulo = (int) (OCR1A - t_0grados)*0.0878;	// Obtengo el angulo (lo paso a int es vez de usar floor(), para no usar math.h)
	unsigned char string_angulo[9] = "Angulo: ";	
	//strcat(string_angulo, str(angulo))							// Concateno el string y el ángulo.
	lcd_write_string_8d(string_angulo);						// Escribo el angulo en el display.
	
	if(dist_cm<150) {											// Si el objeto se encuentra a una distancia aceptable...	
		unsigned char string_dist[12] = "Distancia: ";				    // Defino el string para el display.
		lcd_write_instruction_8d(lcd_setcursor | lcd_line_two);	// Muevo el cursor a la segunda línea.
		//strcat(string_dist, dist_cm)						
		lcd_write_string_8d(string_dist);				// Escribo la distancia.
		lcd_write_instruction_8d(lcd_setcursor | lcd_line_one);  // Muevo el cursor de vuelta a la primer línea.
	}
	
}
