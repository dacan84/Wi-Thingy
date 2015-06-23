/*
 * network_proxy.c
 *
 * Created: 15/06/2015 13:47:39
 *  Author: Diego
 */ 

#include "network_proxy.h"
#include "..\sht11/sht11.h"
#include "payload.h"
#include "digi_api.h"

const uint8_t COORDINATOR_ADDRESS[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xBB, 0x3D, 0xD3};
const uint8_t SENSORS[2] = {H25K5A_ID, NTC_ID, SHT_ID, MSB_ID};
const uint8_t BROADCAST_RADIUS = 0x00;
const uint8_t TRANSMIT_OPTIONS = 0x00;

extern ShtData shtData;
extern uint16_t co2d1Data;  //TODO: eliminar porque en estos nodos no va.
extern uint16_t h25k5aData;
extern uint16_t ntcData;
/* sensores que declaro porque los voy a usar, pero aun no están implementados.*/
//extern MsbData	msbData;
//extern BmpData	bmpData;

void SendMeasures(void) {
	XBeePacket packet;
	Payload payload;
	XBeeCreatePacket(&packet);
	PayloadInit(&payload);
	PayloadPutByte(&payload, MOBILE);
	PayloadPutString(&payload, SENSORS);
	PayloadPutWord(&payload, h25k5aData);
	PayloadPutWord(&payload, ntcData);
	PayloadPutWord(&payload, shtData.temperature.i);
	PayloadPutWord(&payload, shtData.humidity.i);
	//PayloadPutWord(&payload, co2d1Data);		//TODO: quitar la carga de el sensor de CO2 e introducir la de los nuevos sensores.
	XBeeCreateTransmitRequestPacket(&packet, 0x06, (uint8_t*) COORDINATOR_ADDRESS,
	BROADCAST_RADIUS, TRANSMIT_OPTIONS, payload.data, payload.size);
	XBeeSendPacket(&packet);
}



