#define F_CPU 16000000UL
#define t_0grados 350
#define t_180grados 2400
#define t_paso 5
#define ms_servo 25

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Headers de funciones
void sonar_setup(void);
void trigger_pulse(void);
void dist_calc(unsigned int, unsigned int);