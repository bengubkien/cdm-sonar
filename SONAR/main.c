/*
 * Proyecto SONAR
 *
 * Created: 12/7/2020 7:20:46 PM
 * Author : Tomas Tavella y Benjamin Gubkien
 *
 * Outputs: 	PortL 1 (Pin 48) para mandar el pulso al sensor
 * 	    	PortB 5 (Pin 11, OC1A) para mandar la PWM que maneja el servo
 *
 * Inputs:  	PortL 0 (Pin 49, ICP4) para recibir el pulso de echo del sensor
 *
 */ 

// Definiciones de constantes usadas en el programa
#define F_CPU 16000000UL
#define t_0grados 350
#define t_180grados 2400
#define t_paso 10	
#define ms_servo 50

// LCD module information
#define lcd_LineOne     0x00                    // start of line 1
#define lcd_LineTwo     0x40                    // start of line 2

// LCD instructions
#define lcd_Clear           0b00000001          // replace all characters with ASCII 'space'
#define lcd_Home            0b00000010          // return cursor to first position on first line
#define lcd_EntryMode       0b00000110          // shift cursor from left to right on read/write
#define lcd_DisplayOff      0b00001000          // turn display off
#define lcd_DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define lcd_FunctionReset   0b00110000          // reset the LCD
#define lcd_FunctionSet8bit 0b00111000          // 8-bit data, 2-line display, 5 x 7 font
#define lcd_SetCursor       0b10000000          // set cursor position

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

// Headers de funciones
void sonar_setup(void);
void servo_rotation(void);
void trigger_pulse(void);

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

/*============================== FUNCIONES ======================*/

/*
  Nombre:     setup
  Propósito:  Setea los timers necesarios, los pines de I/O usados, y los modos de bajo consumo
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void sonar_setup(void){
	// Timer 1 para la onda PWM PFC del servo
	TCCR1A |= (1 << COM1A1);						// Limpio OC1A para upcounting en compare match y seteo 0C1A para downcounting en compare match
	TCCR1B |= (1 << WGM13) | (1 << CS11);					// WGM1 3:0 (bits 3 y 2 en TCCR1B y 1 y 0 en TCCR1A) en 0b1000 para modo PFCPWM con TOP = ICR1, y prescaler en 8
	DDRB |= (1 << DDB5);							// Port B5 (Pin 11, OC1A) como salida
	ICR1 = 20000;								// 20 ms de periodo PWM
	
	// Timer 3 para contar tiempo entre pulsos
	TCCR3A |= (1 << WGM31);								// Modo fast PWM con overflow en el valor de ICR3
	TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30);		// 64 de prescaler
	TIMSK3 |= (1 << TOIE3);								// Activo el interrupt por overflow
	ICR3 = 50000;									// Seteo el TOP para que el overflow se de a los 200 ms
	
	// Timer 4 para medir el tiempo entre el pulso de salida y el que devuelve el sensor
	TCCR4B |= (1 << ICES4);								// Seteo que la interrupción se dé en flanco de subida y un prescaler de 8.
	TIMSK4 |= (1 << ICIE4);
	DDRL |= (1 << PL1);								// Seteo el PortL 1 como salida para el pulso del sensor (Pin 48) y el 0 como entrada para el echo (Pin 49)
	
	// Modos de bajo consumo (PRR0 y PRR1)
	PRR0 |= (1 << PRTWI) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);		// Desactivo TWI (Two wire interface), SPI, el ADC y los USART
	PRR1 |= (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1);
}	

/*
  Nombre:     rotacion_servo
  Propósito:  Se encarga de la rotacion del servo sobre el que se monta el sensor
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void servo_rotation(void){
	for( OCR1A = t_0grados; OCR1A <= t_180grados; OCR1A = OCR1A + t_paso){
		_delay_ms(ms_servo);
	}
	for( OCR1A = t_180grados; OCR1A >= t_0grados; OCR1A = OCR1A - t_paso){
		_delay_ms(ms_servo);
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
	
	PORTL |= (1 << PL1);				// Envio el pulso de 10us al sensor.
	_delay_us(10);
	PORTL |= (0 << PL1);
}

/*
  Nombre:     lcd_init_8d
  Propósito:  Inicializar el display LCD para una interfaz de 8 bit.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void lcd_init_8d(void)
{
// Delay de encendido.
    _delay_ms(100);                                 // 40ms.

// Reset del controlador del display LCD.
    lcd_write_instruction_8d(lcd_functionreset);    // Primera parte de la secuencia de reset.
    _delay_ms(10);                                  // Delay de 4.1ms.

    lcd_write_instruction_8d(lcd_functionreset);    // Segunda parte de la secuencia de reset.
    _delay_us(200);                                 // Delay de 100us.

    lcd_write_instruction_8d(lcd_functionreset);    // Tercera parte de la secuencia de reset.
    _delay_us(200);                                

// Configuración del display LED.
    lcd_write_instruction_8d(lcd_functionreset);    // Seteo el modo, la cantidad de líneas y font.
    _delay_us(80);                                  // Delay de 40us.

// Rutina de inicialización (chequear después).

    lcd_write_instruction_8d(lcd_displayoff);       // Apago el display.
    _delay_us(80);                                  // Delay de 40us.

    lcd_write_instruction_8d(lcd_clear);            // Limpio la RAM del display.
    _delay_ms(4);                                   // Delay de 1.64ms.

    lcd_write_instruction_8d(lcd_entrymode);        // Seteo el comportamiento del cursor.
    _delay_us(80);                                  // Delay de 40us.

    lcd_write_instruction_8d(lcd_displayon);        // Prendo el display.
    _delay_us(80);                                  // Delay de 40us.
}

/*...........................................................................*/

/*
  Nombre:     lcd_write_string_8d
  Propósito:  Mostrar un string en el display LCD.
  Inputs:     "texto" es el string que se representará en el display.
  Outputs:    Ninguno.
*/

void lcd_write_string_8d(uint8_t texto[])
{
    volatile int i = 0;                             
    while (texto[i] != 0)
    {
        lcd_write_character_8d(texto[i]);
        i++;
        _delay_us(80);                              // Delay de 40us.
    }
}

/*...........................................................................*/

/*
  Nombre:     lcd_write_character_8d
  Propósito:  Envía un byte de información al registro de datos del display LCD.
  Inputs:     "datos" es la información a enviar al registro.
  Outputs:    Ninguno.
*/

void lcd_write_character_8d(uint8_t datos)
{
    lcd_RS_port |= (1<<lcd_RS_bit);                 // Selecciono el registro de datos.
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Me aseguro que E esté en bajo.
    lcd_write_8(datos);                             // Escribo los datos.
}

/*...........................................................................*/

/*
  Nombre:       lcd_write_instruction_8d
  Propósito:  Envía un byte de información al registro de instrucciones del display LCD.
  Inputs:     "instrucciones" es la información a enviar al registro.
  Outputs:    Ninguno.
*/

void lcd_write_instruction_8d(uint8_t instrucciones)
{
    lcd_RS_port &= ~(1<<lcd_RS_bit);                // Selecciono el registro de instrucciones.
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Me aseguro que E esté en bajo.
    lcd_write_8(instrucciones);                     // Escribo las instrucciones.
}

/*...........................................................................
  Nombre:     lcd_write_8
  Propósito:  Envío un byte de información al módulo del LCD.
  Inputs:     "info" es la información a enviar al registro del LCD.
  Outputs:    Ninguno.
*/
void lcd_write_8(uint8_t info)
{
    lcd_D7_port &= ~(1<<lcd_D7_bit);                        // Asumo que el dato es '0'.
    if (info & 1<<7) lcd_D7_port |= (1<<lcd_D7_bit);        // Fuerzo que el dato sea '1' si es necesario.

    lcd_D6_port &= ~(1<<lcd_D6_bit);                        // Repito para todos los otros bits.
    if (info & 1<<6) lcd_D6_port |= (1<<lcd_D6_bit);

    lcd_D5_port &= ~(1<<lcd_D5_bit);
    if (info & 1<<5) lcd_D5_port |= (1<<lcd_D5_bit);

    lcd_D4_port &= ~(1<<lcd_D4_bit);
    if (info & 1<<4) lcd_D4_port |= (1<<lcd_D4_bit);

    lcd_D3_port &= ~(1<<lcd_D3_bit);
    if (info & 1<<3) lcd_D3_port |= (1<<lcd_D3_bit);

    lcd_D2_port &= ~(1<<lcd_D2_bit);
    if (info & 1<<2) lcd_D2_port |= (1<<lcd_D2_bit);

    lcd_D1_port &= ~(1<<lcd_D1_bit);
    if (info & 1<<1) lcd_D1_port |= (1<<lcd_D1_bit);

    lcd_D0_port &= ~(1<<lcd_D0_bit);
    if (info & 1<<0) lcd_D0_port |= (1<<lcd_D0_bit);

// Escribo los datos.
                                                    // 'Address set-up time' (40 nS)
    lcd_E_port |= (1<<lcd_E_bit);                   // Enable pin high
    _delay_us(1);                                   // implement 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS)
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Enable pin low
    _delay_us(1);                                   // implement 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS)
}

/*============================== INTERRUPCIONES ======================*/


ISR(TIMER3_OVF_vect){					// Vector de interrupcion del overflow del timer 3
	trigger_pulse();
}

ISR(TIMER4_CAPT_vect)					// Vector de interrupción de input capture para el Timer 4.
{
	TCCR4B |= (0<<CS41);				// Freno el timer.
	int dist_cm = ICR4 / (2*58);			// Una cuenta de 2 equivale a 1 us con 8 de prescaler. La cuenta para la distancia en cm es t_us/58 = dist_cm  ==>  count/(2*58) = dist_cm.
	ICR4 = 0;					// Limpio los registros contadores.
	TCNT4 = 0;
	int angulo = floor((OCR1A - t_0grados)*0.088);
	char string_angulo[9] = "Angulo: ";	
	//strcat(string_angulo, str(angulo))							// Concateno el string y el ángulo.
	lcd_write_string_8d(string_angulo)						// Escribo el angulo en el display.
	
	if(dist_cm<150) {											// Si el objeto se encuentra a una distancia aceptable...	
		char string_dist[12] = "Distancia: ";				    // Defino el string para el display.
		lcd_write_instruction_8d(lcd_SetCursor | lcd_LineTwo);	// Muevo el cursor a la segunda línea.
		//strcat(string_dist, dist_cm)						
		lcd_write_string_8d(string_dist, dist_cm)				// Escribo la distancia.
		lcd_write_instruction_8d(lcd_SetCursor | lcd_LineOne);  // Muevo el cursor de vuelta a la primer línea.
	}
	
}
