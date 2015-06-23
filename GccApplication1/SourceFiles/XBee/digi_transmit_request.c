/*
 * digi_transmit_request.c
 *
 * Created: 15/06/2015 13:04:14
 *  Author: Diego
 */ 


#include "digi_api.h"

/*..........................................................................*/

void XBeeCreateTransmitRequestPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* destinationAddress, uint8_t radius, uint8_t options,
uint8_t* payload, uint8_t dataLength) {
	uint8_t* p = packet->frame.transmitRequest.destinationAddress;
	uint8_t i = MAC_ADDRESS_BYTES_LENGTH;
	while (i--) {
		*p++ = *destinationAddress++;
	}
	p = packet->frame.transmitRequest.payload;
	i = dataLength;
	while (i--) {
		*p++ = *payload++;
	}
	p = packet->frame.transmitRequest.reserved;
	*p++ = TRANSMIT_REQUEST_RESERVED_H;
	*p++ = TRANSMIT_REQUEST_RESERVED_L;
	packet->frame.transmitRequest.bcastRadius = radius;
	packet->frame.transmitRequest.options = options;
	packet->apiId = TRANSMIT_REQUEST;
	packet->frame.transmitRequest.frameId = frameId;
	packet->length = TRANSMIT_REQUEST_COMMAND_MINLENGTH + dataLength;
}