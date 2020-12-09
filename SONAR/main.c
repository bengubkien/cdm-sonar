/*
 * Proyecto SONAR
 *
 * Created: 12/7/2020 7:20:46 PM
 * Author : Tomas Tavella y Benjamin Gubkien
 */ 

#define F_CPU 16000000UL
#define t_0grados 350
#define t_180grados 2400
#define t_paso 10	
#define DELAY_BACKWARD_COMPATIBLE		// Permite pasar el tiempo como variable a _delay_ms y _delay_us

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setup_timers(void);
void rotacion_servo(int);
void trigger_pulse(void);

int dist_cm;

int main(void)
{
	setup_timers();
	while (1)
	{
		rotacion_servo(50);
	}
}

/*============================== FUNCIONES ======================*/

/*
  Nombre:     setup_timers
  Propósito:  Setea los timers 1 (para el servo), 3 (para contar tiempo entre pulsos del sensor), y
			  4 (para generar el pulso de 10 us)
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void setup_timers(void){
	// Timer 1 usado para la PWM PFC del servo
	TCCR1A |= (1 << COM1A1);						// Limpio OC1A para upcounting en compare match y seteo 0C1A para downcounting en compare match
	TCCR1B |= (1 << WGM13) | (1 << CS11);					// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler en 8
	DDRB |= (1 << DDB5);							// Port B5 (Pin 11, OC1A) como salida
	ICR1 = 20000;								// 20 ms de periodo PWM
	
	// Timer 3 para contar tiempo entre pulsos
	TCCR3A |= (1 << WGM31);								// Modo fast PWM con overflow en el valor de ICR3
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);		// 64 de prescaler
	TIMSK3 |= (1 << TOIE3);								// Activo el interrupt por overflow
	ICR3 = 50000;									// Seteo el TOP para que el overflow se de a los 200 ms
	
	// Timer 4 para generar el pulso de 10 us
	TCCR4B |= (1<<ICES4);								// Seteo que la interrupción se dé en flanco de subida y un prescaler de 8.
	TIMSK4 |= (1<<ICIE4);
}	

/*
  Nombre:     rotacion_servo
  Propósito:  Se encarga de la rotacion del servo sobre el que se monta el sensor
  Inputs:     ms (tiempo de delay entre reotaciones)
  Outputs:    Ninguno.
*/

void rotacion_servo(int ms){
	for( OCR1A = t_0grados; OCR1A <= t_180grados; OCR1A = OCR1A + t_paso){
		_delay_ms(ms);
	}
	for( OCR1A = t_180grados; OCR1A >= t_0grados; OCR1A = OCR1A - t_paso){
		_delay_ms(ms);
	}
}

/*
  Nombre:     trigger_pulse
  Propósito:  Hacer que el sensor envíe un pulso y activa el timer.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void trigger_pulse(void){
	TCCR4B |= (1<<CS41);				// Comienzo el conteo con prescaler en 8.
	
	trigger_input_bit = 1;				// Envio el pulso de 10us al sensor.
	_delay_us(10);
	trigger_input_bit = 0;
}

ISR(TIMER3_OVF_vect){					// Vector de interrupcion del overflow del timer 3
	trigger_pulse();
}

ISR(TIMER4_CAPT_vect)					// Vector de interrupción de input capture para el Timer 4.
{
	TCCR4B |= (0<<CS41);				// Freno el timer.
	dist_cm = ICR4 / (2*58);			// Una cuenta de 2 equivale a 1 us con 8 de prescaler. La cuenta para la distancia en cm es t_us/58 = dist_cm  ==>  count/(2*58) = dist_cm.
	ICR4 = 0;					// Limpio los registros contadores.
	TCNT4 = 0;
}
