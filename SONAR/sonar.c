#include "sonar.h"
#include "lcd.h"

/*============================== FUNCIONES ======================*/

/*
  Nombre:     sonar_setup
  Propósito:  Setea los timers necesarios, los pines de I/O usados, y los modos de bajo consumo
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void sonar_setup(void){
	
	// Timer 0 para contar tiempo entre pulsos (Fast PWM) 						// Seteo el timer 0 de 8 bits para contar intervalos de 5 us
	TCCR0A |= (1 << WGM01) | (1 << WGM00);										// para medir el tiempo del sensor
	TCCR0B |= (1 << WGM02);														// WGM0 2:0 para modo Fast PWM con ovf en OCR0A
	TIMSK0 |= (1 << TOIE3);														// Activo el interrupt por ovf	
	OCR0A = 80;																	// Si OCR0A = 80  ==>  T_timer0 = OCR0A/16MHz = 80/16MHz = 5 us
	
	// Timer 1 para la onda PWM PFC del servo
	TCCR1A |= (1 << COM1A1);						// Limpio OC1A para upcounting en compare match y seteo 0C1A para downcounting en compare match
	TCCR1B |= (1 << WGM13) | (1 << CS11);			// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler en 8
	DDRB |= (1 << DDB5);							// Port B5 (Pin 11, OC1A) como salida
	ICR1 = 40000;									// Si ICR1 = 40000  ==>  T_timer1 = ICR1/(16/8)MHz = 40000/2MHz = 20 ms de periodo
	
	// Timer 3 para contar tiempo entre pulsos
	TCCR3A |= (1 << WGM31);													// Modo fast PWM con overflow en TOP = ICR3
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);		// 64 de prescaler
	TIMSK3 |= (1 << TOIE3);													// Activo el interrupt por overflow
	ICR3 = 25000;															// Si ICR3 = 25000  ==>  T_timer3 = ICR3/(16/64)MHz = 25000/0.25MHz = 100 ms de periodo
	
	//Seteo el interrupt de INT0 (Port D0, Pin 21)											
	EICRA |= (1 << ISC00);														// Interrupt en cualquier flanco (Para que detecte el flanco de subida y luego el de bajada de Echo)
	EIMSK |= (1 << INT0);														// Activo el interrupt externo
	
	// Modos de bajo consumo (PRR0 y PRR1)
	PRR0 |= (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);		// Desactivo TWI (Two wire interface), SPI, el ADC y los USART
	PRR1 |= (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1);
	
	DDRL |= (1 << PL1);															// Seteo el PortL 1 como salida para el pulso del sensor (Pin 48)
}	

/*...........................................................................*/

/*
  Nombre:     dist_calc
  Propósito:  Calcula la distancia que mide el sensor y el angulo en el que se encuentra, 
			  ademas de escribirlo en pantalla
  Inputs:     count_5us, el valor del contador del timer 4 entre que se manda el pulso y el sensor devuelve
			  pulse_width, el valor de OCR1A que determina el ancho de pulso para el angulo del servo
  Outputs:    Ninguno.
*/

void dist_calc(unsigned int count_5us, unsigned int pulse_width){
	unsigned int dist_cm;												// Una cuenta de 2 equivale a 1 us con 8 de prescaler. La cuenta para la distancia en cm es t_us/58 = dist_cm  ==>  count/(2*58) = dist_cm.
	dist_cm = (5*count_5us)/55;
	unsigned int angulo = (pulse_width - (t_0grados))*0.088;			// Obtengo el angulo (lo paso a int es vez de usar floor(), para no usar math.h)
	char string_angulo[16] = "Angulo  ";
	char angulo_char[3];
	strcat(string_angulo,itoa(angulo,angulo_char,10));
	strcat(string_angulo," deg  ");										// Se agregan 2 espacios para que no quede escrita una g al final una vez que se achica la contidad de cifras (100 grados a 99 grados)
	lcd_write_string(string_angulo);									// Escribo el angulo en el display
	char string_dist1[16] = "Dist. ";									// Defino el string que contiene la palabra Dist.
	char string_dist2[16] = "DDist. ";									// Se define otro con DDist, porque por alguna razon, cuando dist_cm > 50, se escribe ist. en vez de Dist.
	lcd_write_instr(lcd_set_cursor | lcd_line_two);						// Muevo el cursor a la segunda línea.
	
	if(dist_cm < 80) {													// Si el objeto se encuentra a una distancia aceptable...
		char dist_char[3];
		strcat(string_dist1,"  ");
		strcat(string_dist1,itoa(dist_cm,dist_char,10));
		strcat(string_dist1," cm  ");
		lcd_write_string(string_dist1);									// Escribo la distancia.
	}else{
		strcat(string_dist2,"> 80 cm");
		lcd_write_string(string_dist2);									// Escribo la distancia.
	}
	lcd_write_instr(lcd_home);							// Vuelvo al principio de la primera linea
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
