#include "sonar.h"
#include "lcd.h"

/*============================== FUNCIONES ======================*/

/*
  Nombre:     sonar_setup
  Propósito:  Setea los timers necesarios, los pines de I/O usados, y los modos de bajo consumo.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void sonar_setup(void){
	
	// Timer 0 para contar tiempo entre pulsos (Fast PWM). 					// Setea el Timer 0 (8 bits) para contar intervalos de 5us
	TCCR0A |= (1 << WGM01) | (1 << WGM00);									// para medir el tiempo del sensor
	TCCR0B |= (1 << WGM02);													// WGM0 2:0 para modo Fast PWM con overflow en OCR0A.
	TIMSK0 |= (1 << TOIE3);													// Activa el interrupt por overflow.
	OCR0A = 80;																// Si OCR0A = 80  ==>  T_timer0 = OCR0A/16MHz = 80/16MHz = 5us.
	
	// Timer 1 para la onda PWM P&FC del servo.
	TCCR1A |= (1 << COM1A1);												// Limpia OCR1A para upcounting en compare match y setea 0CR1A para downcounting en compare match.
	TCCR1B |= (1 << WGM13) | (1 << CS11);									// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler de 8.
	DDRB |= (1 << DDB5);													// Port B5 (Pin 11, OC1A) como salida.
	ICR1 = 20000;															// Si ICR1 = 40000  ==>  T_timer1 = ICR1/(16/8)MHz = 40000/2MHz = 20ms de periodo.
	
	// Timer 3 para contar el tiempo entre pulsos del sensor de ultrasonido.
	TCCR3A |= (1 << WGM31);													// Modo Fast PWM con overflow en TOP = ICR3.
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);		// Prescaler de 64.
	TIMSK3 |= (1 << TOIE3);													// Activa la interrupción por overflow.
	ICR3 = 25000;															// Si ICR3 = 25000  ==>  T_timer3 = ICR3/(16/64)MHz = 25000/0.25MHz = 100ms de período.
	DDRL |= (1 << PL1);														// Setea el PortL 1 como salida para el pulso del sensor (Pin 48).
	
	//Interrupt 0 para el eco del sensor de ultrasonido. 											
	EICRA |= (1 << ISC00);													// Interrupt en cualquier flanco (para que detecte el flanco de subida y luego el de bajada del eco).
	EIMSK |= (1 << INT0);													// Activa el interrupt externo.
	
	// Modos de bajo consumo (PRR0 y PRR1).
	PRR0 |= (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);	// Desactiva TWI, SPI, el ADC y los USART.
	PRR1 |= (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1);
}	


/*...........................................................................*/

/*
  Nombre:     process_param
  Propósito:  Calcular la distancia que mide el sensor y el ángulo en el que se encuentra, 
			  y escribir en el display LCD.
  Inputs:     count_5us, la cantidad de iteraciones del contador de Timer 0 de 5us durante el pulso de eco.
			  sonar_echo_width, el valor de OCR1A que determina el ancho de pulso para el ángulo del servo.
  Outputs:    Ninguno.
*/

void process_param(unsigned int count_5us, unsigned int sonar_echo_width){
	
	unsigned int dist_cm = (5*count_5us)/55;								// Cálculo de la distancia en centímetros del objeto detectado.
	unsigned int angulo = (sonar_echo_width - (servo_0deg))*0.088;				// Cálculo del ángulo en el que se encuentra el servo.
	
	char string_angulo[16] = "Angulo  ";
	char angulo_char[3];
	
	strcat(string_angulo,itoa(angulo,angulo_char,10));
	strcat(string_angulo," deg  ");
	
	lcd_write_string(string_angulo);
	
	write_dist(dist_cm);													// Escritura de la distancia en el display.
}

/*...........................................................................*/

/*
  Nombre:     trigger_pulse
  Propósito:  Hacer que el sensor envíe un pulso y activa el timer.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void trigger_pulse(void){
	PORTL |= (1 << PL1);				// Envio el pulso de 10us al sensor.
	_delay_us(10);
	PORTL &= ~(1 << PL1);
}

/*...........................................................................*/

/*
  Nombre:     write_dist
  Propósito:  Manipular a la distancia calculada en un string para escribirlo en el display LCD.
  Inputs:     dist_cm, distancia calculada en centímetros.
  Outputs:    Ninguno.
*/

void write_dist(unsigned int dist_cm){
	char string_dist1[16] = "Dist. ";										// Define el string que contiene la palabra 'Dist.:'.
	char string_dist2[16] = "DDist. ";
	
	lcd_write_instr(lcd_set_cursor | lcd_line_two);							// Mueve el cursor a la segunda línea.
	
	if(dist_cm < 80) {														// Si el objeto se encuentra a una distancia aceptable...
		char dist_char[3];
		strcat(string_dist1,"  ");
		strcat(string_dist1,itoa(dist_cm,dist_char,10));
		strcat(string_dist1," cm  ");
		lcd_write_string(string_dist1);
	} else {															// Si el objeto no se encuentra a una distancia aceptable...
		strcat(string_dist2,"> 80 cm ");										// Escribe una cota en el display.
		lcd_write_string(string_dist2);
	}
	
	lcd_write_instr(lcd_home);												// Mueve el cursor al principio de la primera linea.
}