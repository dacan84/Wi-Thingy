/*
 * payload.h
 *
 * Created: 15/06/2015 12:12:59
 *  Author: Diego
 */ 


#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include "bsp.h"

#define MAX_PAYLOAD     100

/* Static List struct */
typedef struct Payload Payload;

struct Payload {
    uint8_t data[MAX_PAYLOAD];
    uint8_t size;
};

/* Init list */
void PayloadInit(Payload* payload);

/* Add one byte to the list */
void Payload_putByte(Payload* payload, const uint8_t element);

/* Add one word to the list */
void PayloadPutWord(Payload* payload, const uint16_t element);

/* Append */
void Payload_append(Payload* to, const Payload* from);

/* Put a formatted string in ROM */
void Payload_putString(Payload* payload, const uint8_t* string);

/* Put a formatted string in RAM */
void Payload_putRAMString(Payload* payload, const uint8_t* string);

#endif /* PAYLOAD_H_ */