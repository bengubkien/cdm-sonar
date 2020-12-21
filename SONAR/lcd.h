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


