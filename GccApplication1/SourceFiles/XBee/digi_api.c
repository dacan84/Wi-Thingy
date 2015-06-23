/*
 * digi_api.c
 *
 * Created: 15/06/2015 13:02:53
 *  Author: Diego
 */ 

#include "digi_api.h"
#include <string.h>

void XBeeCreatePacket(XBeePacket * const packet) {
	packet->dataPtr = (uint8_t*) packet;
	packet->checksum = 0;
	packet->rxState = XBEE_PACKET_RX_START;
	packet->length = 0;
	packet->index = 0;
	packet->apiId = 0;
	memset(packet->frame.payload, 0, MAX_PAYLOAD);
}

uint8_t XBeeEscape(uint8_t value) {
	if (value == START_DELIMITER || value == XON || value == XOFF || value == ESCAPE) {
		return value ^ 0x20;
	}
	return value;
}