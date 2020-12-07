/*============================== DISPLAY LCD ======================*/

// Puertos a utilizar.
#define lcd_D7_port     PORTD                   // D7.
#define lcd_D7_bit      PORTD7
#define lcd_D7_ddr      DDRD

#define lcd_D6_port     PORTD                   // D6.
#define lcd_D6_bit      PORTD6
#define lcd_D6_ddr      DDRD

#define lcd_D5_port     PORTD                   // D5.
#define lcd_D5_bit      PORTD5
#define lcd_D5_ddr      DDRD

#define lcd_D4_port     PORTD                   // D4.
#define lcd_D4_bit      PORTD4
#define lcd_D4_ddr      DDRD

#define lcd_D3_port     PORTD                   // D3.
#define lcd_D3_bit      PORTD3
#define lcd_D3_ddr      DDRD

#define lcd_D2_port     PORTD                   // D2.
#define lcd_D2_bit      PORTD2
#define lcd_D2_ddr      DDRD

#define lcd_D1_port     PORTD                   // D1.
#define lcd_D1_bit      PORTD1
#define lcd_D1_ddr      DDRD

#define lcd_D0_port     PORTD                   // D0.
#define lcd_D0_bit      PORTD0
#define lcd_D0_ddr      DDRD

#define lcd_E_port      PORTB                   // ENABLE.
#define lcd_E_bit       PORTB1
#define lcd_E_ddr       DDRB

#define lcd_RS_port     PORTB                   // REGISTER SELECT.
#define lcd_RS_bit      PORTB0
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
