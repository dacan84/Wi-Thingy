/*
 * digi_send.c
 *
 * Created: 15/06/2015 13:03:48
 *  Author: Diego
 */ 


#include "digi_api.h"
#include "digi_serial.h"
//TODO: aqui hay cositas que corregir

/* Send XBee packet */
bool XBeeSendPacket(XBeePacket * const packet) {
	uint8_t* p = (uint8_t*) packet;
	XBeeSerialSend(START_DELIMITER);
	// send the most significant bit
	XBeeSerialSend((packet->length >> 8) & 0xFF);
	// then the LSB
	XBeeSerialSend(packet->length & 0xFF);
	// just in case it hasn't been initialized.
	packet->checksum = 0;
	//Generalizando para cualquier paquete
	while (packet->length--) {
		XBeeSerialSend(*p);
		packet->checksum += *p++;
	}
	XBeeSerialSend((0xFF - packet->checksum));
	// Bugfix: Flush buffer
	//XBeeSerialSend(NULL);			 //TODO: definir el null
	//return TRUE;		    //TODO: definir el true
}