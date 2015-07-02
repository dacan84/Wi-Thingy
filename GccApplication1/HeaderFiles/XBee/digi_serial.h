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

#define RXD				IOPORT_CREATE_PIN(PORTD, PIND2)
#define TXD				IOPORT_CREATE_PIN(PORTD, PIND3)
#define	XBEE_PCINT0		IOPORT_CREATE_PIN(PORTB, PINB0)
#define XBEE_INT_LEVEL  IOPORT_CREATE_PIN(PORTB, PINB0)

void XBeeSerialInit(uint8_t baudrate);

void XBeeSerialSend(uint8_t value);

void XBeeSerialSendArray(uint8_t* values, uint8_t size);

uint8_t XBeeSerialRead(void);

bool XBeeSerialAvailable(void);

void XBeeSerialClose(void);

bool XBeeSerialCheckInterrupt(void);

void XBeeSerialAckInterrupt(void);

void Usart1PowerONandEnable (void);

void Usart1PowerOFFandDisable (void);

#endif /* DIGI_SERIAL_H_ */