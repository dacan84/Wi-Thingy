/*
* digi_serial.c
*
* Created: 15/06/2015 13:03:59
*  Author: Diego
*/

#include "digi_serial.h"

static void EnableUart1Txd(void);
static void DisableUart1Txd(void);
static void EnableUart1Rxd(void);
static void DisableUart1Rxd(void); 
static void EnableUart1TxInterrupt(void); 
static void DisableUart1TxInterrupt(void); 
static void EnableUart1RxInterrupt(void);
static void DisableUart1RxInterrupt(void); 

/*************************************************************************************
Comunicación con el XBee
**************************************************************************************/
void XBeeSerialInit(uint8_t baudrate) {
	ioport_set_pin_dir(RXD, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(TXD, IOPORT_DIR_OUTPUT);
	
	//Ponemos a 0 los flags y errores
	UCSR1A = 0x00;
	//BaudRate para 4 MHz y 9600 bps (UBRR0L = 25)
	UBRR1H = 0x00;
	UBRR1L = baudrate;
	//Asincrono (UMSEL = 0),sin paridad (UPM01:0 por defecto 0)
	//8 bits (UCSZ02:0 = 011) y 1 bit de parada (USBS0 = 0)
	//Enable R y T.
	EnableUart1Txd();
	EnableUart1Rxd();
}

void XBeeSerialSend(uint8_t value) {
	while(!(UCSR1A & (1<<UDRE1))){} //con un 1 esta preparado para escribir
	UDR1 = value;
}

void XBeeSerialSendArray(uint8_t* values, uint8_t size) {
	uint8_t i = 0;
	while (i < size) {
		XBeeSerialSend(values[i++]);
	}
	XBeeSerialSend(NULL);
}

uint8_t XBeeSerialRead(void) {
	return UDR1;
}

bool XBeeSerialAvailable(void) {
	if ((UCSR1A & (1<<UDRE1))){
		return TRUE; // Data is available, return TRUE
		}	else {
		return FALSE;
	}	
}

void XBeeSerialClose(void) {
	DisableUart1Txd();
	DisableUart1Rxd();
	DisableUart1TxInterrupt();
	DisableUart1RxInterrupt();
}

static void EnableUart1Txd(void) {
	UCSR1B |= _BV(TXEN1);
}

static void DisableUart1Txd(void) {
	UCSR1B &= ~(_BV(TXEN1));
}

static void EnableUart1Rxd(void) {
	UCSR1B |= _BV(RXEN1);
}

static void DisableUart1Rxd(void) {
	UCSR1B &= ~(_BV(RXEN1));
}

static void EnableUart1TxInterrupt(void) {
	UCSR1B |= _BV(RXCIE1);
}

static void DisableUart1TxInterrupt(void) {
	UCSR1B &= ~(_BV(TXCIE1));
}

static void EnableUart1RxInterrupt(void) {
	UCSR1B |= _BV(RXCIE1);
}

static void DisableUart1RxInterrupt(void) {
	UCSR1B &= ~(_BV(RXCIE1));
}

/** Interrupt handler functions */
bool XBeeSerialCheckInterrupt(void) {
	return XBeeSerialAvailable();
}

void XBeeSerialAckInterrupt(void) {
	UCSR1A&=_BV(RXC1);
	cli();
}
