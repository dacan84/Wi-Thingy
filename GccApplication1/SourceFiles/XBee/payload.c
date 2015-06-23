/*
* payload.c
*
* Created: 15/06/2015 13:04:29
*  Author: Diego
*/

#include "payload.h"
#include <stdio.h>
#include <string.h>

/*..........................................................................*/

/* Init list, set size equals zero */
void PayloadInit(Payload* payload) {
	uint8_t i = MAX_PAYLOAD - 1;
	payload->size = 0;
	while (i--) {
		payload->data[i] = NULL;
	}
}

/*..........................................................................*/

/* Add one byte to the list */
void PayloadPutByte(Payload* payload, const uint8_t element) {
	if (payload->size < MAX_PAYLOAD - 1) {
		payload->data[payload->size] = element;
		payload->size++;
	}
}

/*..........................................................................*/

/* Add one word to the list(big endian) */
void PayloadPutWord(Payload* payload, const uint16_t element) {
	uint8_t lsb = (uint8_t)element;
	uint8_t msb = (uint8_t)(element >> 8);
	if (payload->size < MAX_PAYLOAD - 2) {
		payload->data[payload->size++] = msb;
		payload->data[payload->size++] = lsb;
	}
}

/*..........................................................................*/

/*..........................................................................*/

/* Append all elements to the list */
void PayloadAppend(Payload* to, const Payload* from) {
	uint8_t i = 0;
	while (i < from->size) {
		PayloadPutByte(to, from->data[i++]);
	}
}

/*..........................................................................*/
/* */
void PayloadPutString(Payload* payload, const uint8_t* string) {
	char* buffer = (char*) &payload->data[payload->size];
	sprintf(buffer, (const char*) string);
	payload->size += strlen((char*)string);
}

void PayloadPutRAMString(Payload* payload, const uint8_t* string) {
	char* buffer = (char*) payload->data;
	strcpy(&(buffer[payload->size]), (char*) string);
	payload->size += strlen((char*)string);
}
