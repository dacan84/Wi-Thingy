/*
 * digi_hw.c
 *
 * Created: 15/06/2015 13:03:24
 *  Author: Diego
 */ 

#include "digi_api.h"
#include "digi_serial.h"

static uint8_t EUSART_9600 = 25;

static void XBeeInterruptInit(void);

void XBeeInit(void) {
	XBeeSerialInit(EUSART_9600);
	XBeeInterruptInit();
	XBeeJoin();
}

static void XBeeInterruptInit(void) {
	//// INT0 rising edge mode
	//INTCON2bits.INTEDG0 = 1;
	//// Disable pullup
	//INTCON2bits.RBPU = 0;
	//// Enable INT0
	//INTCONbits.INT0IE = 1;
	//// Clear INT0 flag
	//INTCONbits.INT0IF = 0;
	//// Configure RB0 as input
	//TRISBbits.TRISB0 = 1;
}