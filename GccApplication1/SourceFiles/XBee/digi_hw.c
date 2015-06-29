/*
 * digi_hw.c
 *
 * Created: 15/06/2015 13:03:24
 *  Author: Diego
 */ 

#include "digi_api.h"
#include "digi_serial.h"
#include "interrupts.h"

static uint8_t EUSART_9600 = 25;
static void XBeeInterruptInit(void);

void XBeeInit(void) {
	XBeeSerialInit(EUSART_9600);
	XBeeInterruptInit();
	XBeeJoin();
}

//Esta función configura la interrupción que genera el XBEE en el uC.
static void XBeeInterruptInit(void) {	
	ioport_set_pin_dir(INT0,IOPORT_DIR_INPUT);
	//// INT0 rising edge mode
	//INTCON2bits.INTEDG0 = 1;
	//// Enable INT0
	EnableExternalInterruptPCINT0();
	ClearInterrupt();
	ClearExternalInterruptFlag(INTF0);
}
