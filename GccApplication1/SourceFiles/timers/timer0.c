/*
 * timer0.c
 *
 * Created: 24/06/2015 10:49:14
 *  Author: Diego
 */ 

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/iom1281.h>
#include <avr/power.h>
#include "timer0.h"

/***********************************************************************
TIMER DE 50 KHz -- Protocolos de comunicación
***********************************************************************/
void Timer0_50KHzInit(void) {
	//Inicio el contador del "reloj"
	TCNT0 = 0x00;
	//La salida de CLKIN/SCLK esta en PG5 (OC0B) y hay que ponerlo de salida
	DDRG |= _BV(DDG5);
	//Comparación en 39 -- f=50 KHz (4MHz/(40*2))
	OCR0A = 49;
	//Contador en toggle y modo CTC
	TCCR0A |= _BV(WGM01) | _BV(COM0B0);
	//Sin preescalado (CS02:0 == 001) -- Contador On
	TCCR0B |= _BV(CS00);
}

/******************************************************************************
TIMER DE 32,768 KHz -- MS5540B
******************************************************************************/
void Timer0_32768HzInit(void) {
	//Conexión consumo
	power_timer0_enable();

	//Inicio el contador del "reloj"
	TCNT0 = 0x00;
	//La salida de MCLK esta en PB7 (OC0A) y hay que ponerlo de salida
	DDRB |= _BV(DDB7);
	//Comparación en 60 -- f=32,786 KHz (4MHz/(61*2))
	OCR0A = 60;
	//Contador en toggle y modo CTC
	TCCR0A |= _BV(WGM01) | _BV(COM0A0);
	//Sin preescalado (CS02:0 == 001) -- Contador On
	TCCR0B |= _BV(CS00);
	}

void Timer0_32768HzStop(void) {
	//Contador Off
	TCCR0B &= ~(_BV(CS00));
	TCCR0A &= ~(_BV(WGM01) | _BV(COM0A0));
	
	//Desconectar Timer0
	power_timer0_disable();
}

/***********************************************************************
TIMER DE 50 KHz Especial -- RTC-CALENDARIO
***********************************************************************/
void Timer0ClockInit(void) {
	//Inicio el contador del "reloj"
	TCNT0 = 0x00;
	//La salida de CLKIN/SCLK esta en PG5 (OC0B) y hay que ponerlo de salida
	DDRG |= _BV(DDG5);
	PORTG &= ~_BV(PG5);
	//Comparación en  -- f= MHz (4MHz/(*2))
	OCR0A = 39;
	//Contador en toggle y modo CTC
	TCCR0A |= _BV(WGM01) | _BV(COM0B0);
	//Sin preescalado (CS02:0 == 001) -- Contador On
	TCCR0B |= _BV(CS00);
}

/***********************************************************************
TIMER STOP GENERAL
***********************************************************************/
void Timer0Stop(void) {
	//Contador Off
	TCCR0B &= ~(_BV(CS00));
	TCCR0A &= ~(_BV(WGM01) | _BV(COM0B0));
}
