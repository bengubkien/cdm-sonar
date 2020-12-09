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
#define ms 50

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setup_timers(void);
void rotacion_servo(void);

int main(void)
{
	setup_timers();
	while (1)
	{
		rotacion_servo();
	}
}

void setup_timers(void){
	// Timer 1 usado para la PWM PFC del servo
	TCCR1A |= (1 << COM1A1);					// Limpio OC1A para upcounting en compare match y seteo 0C1A para downcounting en compare match
	TCCR1B |= (1 << WGM13) | (1 << CS11);				// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler en 8
	DDRB |= (1 << DDB5);						// Port B5 (Pin 11, OC1A) como salida
	ICR1 = 20000;							// 20 ms de periodo PWM
	
	// Timer 4 para contar tiempo entre pulsos
	TCCR3A |= (1 << WGM31);								// Modo fast PWM con overflow en el valor de ICR3
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);		// 64 de prescaler
	TIMSK3 |= (1 << TOIE3);								// Activo el interrupt por overflow
	ICR3 = 50000;									// Seteo el TOP para que el overflow se de a los 200 ms
}	

void rotacion_servo(void){
	for( OCR1A = t_0grados; OCR1A <= t_180grados; OCR1A = OCR1A + t_paso){
		_delay_ms(ms);
	}
	for( OCR1A = t_180grados; OCR1A >= t_0grados; OCR1A = OCR1A - t_paso){
		_delay_ms(ms);
	}
}

ISR(TIMER4_OVF_vect){
	// Aca se llama a la funcion que envia el pulso y mide la duracion
}

// Implementar el timer que cuente el tiempo entre mandar un pulso del sonar y el siguiente (Pensaba timer en modo Fast PWM que salte con overflow, como en el TP2)
// 
