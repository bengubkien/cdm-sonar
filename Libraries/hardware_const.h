/*============================== DISPLAY LCD ======================*/

// Puertos a utilizar.
#define lcd_D7_port     PORTA                   // D7.
#define lcd_D7_bit      PA7
#define lcd_D7_ddr      DDRA

#define lcd_D6_port     PORTA                   // D6.
#define lcd_D6_bit      PA6
#define lcd_D6_ddr      DDRA

#define lcd_D5_port     PORTA                   // D5.
#define lcd_D5_bit      PA5
#define lcd_D5_ddr      DDRA

#define lcd_D4_port     PORTA                   // D4.
#define lcd_D4_bit      PA4
#define lcd_D4_ddr      DDRA

#define lcd_D3_port     PORTA                   // D3.
#define lcd_D3_bit      PA3
#define lcd_D3_ddr      DDRA

#define lcd_D2_port     PORTA                   // D2.
#define lcd_D2_bit      PA2
#define lcd_D2_ddr      DDRA

#define lcd_D1_port     PORTA                   // D1.
#define lcd_D1_bit      PA1
#define lcd_D1_ddr      DDRA

#define lcd_D0_port     PORTA                   // D0.
#define lcd_D0_bit      PA0
#define lcd_D0_ddr      DDRA

#define lcd_E_port      PORTB                   // ENABLE.
#define lcd_E_bit       PB1
#define lcd_E_ddr       DDRB

#define lcd_RS_port     PORTB                   // REGISTER SELECT.
#define lcd_RS_bit      PB0
#define lcd_RS_ddr      DDRB

// Información del display LCD.
#define lcd_line_one     0x00                    // Inicio de la primer línea.
#define lcd_line_two     0x40                    // Inicio de la segunda línea.

// Instrucciones del display LCD.
#define lcd_clear           0b00000001          // Limpia el display.
#define lcd_home            0b00000010          // Vuelve el cursor al origen.
#define lcd_entrymode       0b00000110          // Desplaza al curso de izquierda a derecha en R/W.
#define lcd_displayoff      0b00001000          // Apaga el display.
#define lcd_displayon       0b00001100          // Prende el display sin titilación del cursor.
#define lcd_functionreset   0b00110000          // Resetea el LCD.
#define lcd_functionset8bit 0b00111000          // Datos de 8 bit, display de 2 líneas, font de 5x7.
#define lcd_setcursor       0b10000000          // Establece la posición del cursor
