#define F_CPU 16000000UL
#define servo_0deg 350
#define servo_180deg 2400
#define servo_paso 5
#define delay_servo_ms 25

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Headers de las funciones.

void sonar_setup(void);
void trigger_pulse(void);
void process_param(unsigned int, unsigned int);
void write_dist(unsigned int dist_cm);