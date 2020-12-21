#include "lcd.h"

/*============================== FUNCIONES ======================*/

/*
  Nombre:     lcd_setup
  Propósito:  Inicializar el display LCD para una interfaz de 8 bit.
  Inputs:     Ninguno.
  Outputs:    Ninguno.
*/

void lcd_setup(void)
{
	DDRA = 0xFF;							// Setea el PortA de D0:D7 como salida
	DDRB |= (1 << PB0) | (1 << PB1);		// Setea Enable y Register Select como salida.
	
// Delay de encendido.
    _delay_ms(40);   
	
// Reset del controlador del display LCD.

    lcd_write_instr(lcd_reset);	
    _delay_us(40); 

// Configuración del display LED.
    lcd_write_instr(lcd_set);				// Setea el modo, la cantidad de líneas y font.
    _delay_us(40);

// Rutina de inicialización (chequear después).

    lcd_write_instr(lcd_off);				// Apaga el display.
    _delay_us(40);       

    lcd_write_instr(lcd_clear);            // Limpia la RAM del display.
    _delay_ms(2); 

    lcd_write_instr(lcd_entrymode);        // Setea el comportamiento del cursor.
    _delay_us(40);    

    lcd_write_instr(lcd_on);			   // Prende el display.
    _delay_us(40);
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
        _delay_us(40);
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
    lcd_RS_port |= (1<<lcd_RS_bit);                 // Selecciona el registro de datos.
    lcd_E_port &= ~(1<<lcd_E_bit);                  // 'Enable' debe estar en bajo.
    lcd_write(datos);								// Escribe los datos.
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
    lcd_RS_port &= ~(1<<lcd_RS_bit);               // Selecciona al registro de instrucciones.
    lcd_E_port &= ~(1<<lcd_E_bit);                 // 'Enable' debe estar en bajo.
    lcd_write(instrucciones);                      // Escribe las instrucciones.
}

/*...........................................................................
  Nombre:     lcd_write
  Propósito:  Envío un byte de información al módulo del LCD.
  Inputs:     "info" es la información a enviar al registro del LCD.
  Outputs:    Ninguno.
*/
void lcd_write(uint8_t info)
{
    lcd_D7_port &= ~(1<<lcd_D7_bit);                        // Asume que el dato es '0'.
    if (info & 1<<7) lcd_D7_port |= (1<<lcd_D7_bit);        // Fuerza que el dato sea '1' si es necesario.

    lcd_D6_port &= ~(1<<lcd_D6_bit);                        // Repite para todos los otros bits.
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

// Escribe los datos.

    lcd_E_port |= (1<<lcd_E_bit);						  // Pone a E en alto para escribir los datos.
    _delay_us(1);
    lcd_E_port &= ~(1<<lcd_E_bit);						  // Baja a E de vuelta.
    _delay_us(1); 
}