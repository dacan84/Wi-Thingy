/*
 * digi_at_request.c
 *
 * Created: 15/06/2015 13:03:09
 *  Author: Diego
 */ 

#include "digi_api.h"

void XBeeCreateCompleteATCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command, uint8_t* params, uint8_t paramsLength) {
	uint8_t* p = packet->frame.atCommand.command;
	uint8_t i = paramsLength;
	*p++ = *command++;
	*p++ = *command;
	p = packet->frame.atCommand.value;
	while (i--) {
		*p++ = *params++;
	}
	packet->apiId = AT_COMMAND;
	packet->frame.atCommand.frameId = frameId;
	packet->length = AT_COMMAND_MINLENGTH + paramsLength;
}

/*..........................................................................*/

void XBeeCreateATCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command) {
	uint8_t* p = packet->frame.atCommand.command;
	*p++ = *command++;
	*p++ = *command;
	packet->apiId = AT_COMMAND;
	packet->frame.atCommand.frameId = frameId;
	packet->length = AT_COMMAND_MINLENGTH;
}

/*..........................................................................*/

void XBeeCreateRemoteAtCommandPacket(XBeePacket* packet, uint8_t frameId,
uint8_t* command, uint8_t param, uint8_t* destinationAddress,
uint8_t options) {
	uint8_t* p = packet->frame.remoteAtCommand.command;
	uint8_t i = MAC_ADDRESS_BYTES_LENGTH;
	*p++ = *command++;
	*p++ = *command;
	p = packet->frame.remoteAtCommand.destinationAddress;
	while (i--) {
		*p++ = *destinationAddress++;
	}
	p = packet->frame.explicitAddressing.reserved;
	*p++ = TRANSMIT_REQUEST_RESERVED_H;
	*p++ = TRANSMIT_REQUEST_RESERVED_L;
	packet->frame.remoteAtCommand.parameter = param;
	packet->frame.remoteAtCommand.options = options;
	packet->apiId = AT_COMMAND;
	packet->frame.remoteAtCommand.frameId = frameId;
	packet->length = REMOTE_AT_COMMAND_MINLENGTH;
}