/*
 * digi_serial.h
 *
 * Created: 15/06/2015 12:12:43
 *  Author: Diego
 */ 


#ifndef DIGI_SERIAL_H_
#define DIGI_SERIAL_H_

#include "bsp.h"
#include <ioport.h>

#define RXD		IOPORT_CREATE_PIN(PORTD, PIND2)
#define TXD		IOPORT_CREATE_PIN(PORTD, PIND3)


void XBeeSerialInit(uint8_t baudrate);

void XBeeSerialSend(uint8_t value);

void XBeeSerialSendArray(uint8_t* values, uint8_t size);

uint8_t XBeeSerialRead(void);

bool XBeeSerialAvailable(void);

void XBeeSerialClose(void);

bool XBeeSerialCheckInterrupt(void);

void XBeeSerialAckInterrupt(void);

#endif /* DIGI_SERIAL_H_ */