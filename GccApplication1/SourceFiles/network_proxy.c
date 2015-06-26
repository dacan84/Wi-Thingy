/*
 * network_proxy.c
 *
 * Created: 15/06/2015 13:47:39
 *  Author: Diego
 */ 

#include "network_proxy.h"
#include "payload.h"
#include "digi_api.h"
#include "sht11.h"
#include "bmp085.h"
#include "ms5540b.h"
#include "h25k5a.h"
#include "ntc.h"

const uint8_t COORDINATOR_ADDRESS[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xBB, 0x3D, 0xD3};
const uint8_t SENSORS[] = {H25K5A_ID, NTC_ID, SHT_ID, MS_ID, BMP085_ID};
const uint8_t BROADCAST_RADIUS = 0x00;
const uint8_t TRANSMIT_OPTIONS = 0x00;

extern ShtData shtData;
extern uint16_t h25k5aData;
extern uint16_t ntcData;
extern MsData	msData;
extern BmpData	bmpData;
extern int8_t	bmpCoef[11];

void SendMeasures(void) {
	uint8_t i=0;
	XBeePacket packet;
	Payload payload;
	XBeeCreatePacket(&packet);
	PayloadInit(&payload);
	PayloadPutByte(&payload, MOBILE);
	PayloadPutByte(&payload, sizeof(SENSORS));
	PayloadPutString(&payload, SENSORS);
	PayloadPutWord(&payload, h25k5aData);
	PayloadPutWord(&payload, ntcData);
	PayloadPutWord(&payload, shtData.temperature.i);
	PayloadPutWord(&payload, shtData.humidity.i);
	PayloadPutWord(&payload,msData.pressureD1);
	PayloadPutWord(&payload,msData.temperatureD2);
	for(i=0;i<sizeof(bmpCoef);++i){
		PayloadPutByte(&payload,bmpCoef[i]);
	}
	PayloadPutByte(&payload,bmpData.temperature);
	PayloadPutWord(&payload,bmpData.pressure);
	XBeeCreateTransmitRequestPacket(&packet, 0x06, (uint8_t*) COORDINATOR_ADDRESS,
	BROADCAST_RADIUS, TRANSMIT_OPTIONS, payload.data, payload.size);
	XBeeSendPacket(&packet);
}



