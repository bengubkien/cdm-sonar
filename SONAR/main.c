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


#include "hardware_const.h"
#include "sonar.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

// Headers de funciones
void lcd_write_8(uint8_t);
void lcd_write_instruction_8d(uint8_t);
void lcd_write_character_8d(uint8_t);
void lcd_write_string_8d(uint8_t *);
void lcd_init_8d(void);

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
