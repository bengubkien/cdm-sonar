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

#include <avr/io.h>
#include <util/delay.h>

void setup_servo(void);
void rotacion_servo(int ms);

int main(void)
{
	setup_servo();
    	while (1) 
    	{
		rotacion_servo(25);
    	}
}

void setup_servo(void){
	TCCR1A |= (1 << COM1A1);								// Limpio OC1A para upcounting en compare match y seteo 0C1A para downcounting en compare match
	TCCR1B |= (1 << WGM13) | (1 << CS11);							// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler en 8
	DDRB |= (1 << DDB5);									// Port B5 (Pin 11, OC1A) como salida
	ICR1 = 20000;										// 20 ms de periodo PWM
	OCR1A = t_0grados;
}

void rotacion_servo(int ms){
	for( OCR1A = t_0grados; OCR1A <= t_180grados; OCR1A = OCR1A + t_paso){
		_delay_ms(ms);							
	}
	for( OCR1A = t_180grados; OCR1A >= t_0grados; OCR1A = OCR1A - t_paso){
		_delay_ms(ms);
	}
}
// Implementar el timer que cuente el tiempo entre mandar un pulso del sonar y el siguiente (Pensaba timer en modo Fast PWM que salte con overflow, como en el TP2)
// 
