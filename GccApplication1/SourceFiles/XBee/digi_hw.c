/*
 * digi_hw.c
 *
 * Created: 15/06/2015 13:03:24
 *  Author: Diego
 */ 

#include "digi_api.h"
#include "digi_serial.h"
#include "interrupts.h"
#include <avr/power.h>

static uint8_t EUSART_9600 = 25;
static void XBeeInterruptInit(void);

void XBeeInit(void) {
	power_usart1_enable();
	XBeeSerialInit(EUSART_9600);
	
	XBeeJoin();
	XBeeInterruptInit();
	//power_usart1_disable();
}

//Esta funci�n configura la interrupci�n que genera el XBEE en el uC.
static void XBeeInterruptInit(void) {	
	ioport_set_pin_dir(XBEE_PCINT0,IOPORT_DIR_INPUT);
	ioport_set_pin_mode(XBEE_PCINT0,IOPORT_MODE_PULLDOWN);
	EnableExternalInterruptPCINT0();
	ClearExternalInterruptFlag(INTF0);
}
