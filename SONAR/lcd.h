#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "hardware_const.h"

// Headers de las funciones.
void lcd_write(uint8_t);
void lcd_write_instr(uint8_t);
void lcd_write_char(uint8_t);
void lcd_write_string(uint8_t *);
void lcd_setup(void);

/*============================== FUNCIONES ======================*/

/*
  Nombre:     lcd_setup
  Propósito:  Inicializar el display LCD para una interfaz de 8 bit.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void lcd_setup(void)
{
	DDRA = 0xFF;						// Seteo el PortA de D0:D7 como salida
	DDRB |= (1 << PB0) | (1 << PB1);	// Seteo Enable y RS como salida
	
// Delay de encendido.
    _delay_ms(40);                                 // 40ms.

// Reset del controlador del display LCD.
    lcd_write_instr(lcd_reset);    // Primera parte de la secuencia de reset.
    _delay_ms(4);                                  // Delay de 4.1ms.

    lcd_write_instr(lcd_reset);    // Segunda parte de la secuencia de reset.
    _delay_us(100);                                 // Delay de 100us.

    lcd_write_instr(lcd_reset);    // Tercera parte de la secuencia de reset.
    _delay_us(100);                                

// Configuración del display LED.
    lcd_write_instr(lcd_set);    // Seteo el modo, la cantidad de líneas y font.
    _delay_us(40);                                  // Delay de 40us.

// Rutina de inicialización (chequear después).

    lcd_write_instr(lcd_off);       // Apago el display.
    _delay_us(40);                                  // Delay de 40us.

    lcd_write_instr(lcd_clear);            // Limpio la RAM del display.
    _delay_ms(2);                                   // Delay de 1.64ms.

    lcd_write_instr(lcd_entrymode);        // Seteo el comportamiento del cursor.
    _delay_us(40);                                  // Delay de 40us.

    lcd_write_instr(lcd_on);        // Prendo el display.
    _delay_us(40);                                  // Delay de 40us.
}

/*...........................................................................*/

/*
  Nombre:     lcd_write_string
  Propósito:  Mostrar un string en el display LCD.
  Inputs:     "texto" es el string que se representará en el display.
  Outputs:    Ninguno.
*/

void lcd_write_string(uint8_t texto[])
{
    volatile int i = 0;                             
    while (texto[i] != 0)
    {
        lcd_write_char(texto[i]);
        i++;
        _delay_us(40);                              // Delay de 40us.
    }
}

/*...........................................................................*/

/*
  Nombre:     lcd_write_char
  Propósito:  Envía un byte de información al registro de datos del display LCD.
  Inputs:     "datos" es la información a enviar al registro.
  Outputs:    Ninguno.
*/

void lcd_write_char(uint8_t datos)
{
    lcd_RS_port |= (1<<lcd_RS_bit);                 // Selecciono el registro de datos.
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Me aseguro que E esté en bajo.
    lcd_write(datos);                             // Escribo los datos.
}

/*...........................................................................*/

/*
  Nombre:     lcd_write_instr
  Propósito:  Envía un byte de información al registro de instrucciones del display LCD.
  Inputs:     "instrucciones" es la información a enviar al registro.
  Outputs:    Ninguno.
*/

void lcd_write_instr(uint8_t instrucciones)
{
    lcd_RS_port &= ~(1<<lcd_RS_bit);                // Selecciono el registro de instrucciones.
    lcd_E_port &= ~(1<<lcd_E_bit);                  // Me aseguro que E esté en bajo.
    lcd_write(instrucciones);                     // Escribo las instrucciones.
}

/*...........................................................................
  Nombre:     lcd_write
  Propósito:  Envío un byte de información al módulo del LCD.
  Inputs:     "info" es la información a enviar al registro del LCD.
  Outputs:    Ninguno.
*/
void lcd_write(uint8_t info)
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
