/*
 * digi_join.c
 *
 * Created: 15/06/2015 13:03:35
 *  Author: Diego
 */ 

#include "digi_api.h"
#include <avr/delay.h>
#include "bsp.h"
#include <avr/power.h>

static XBeePacket joinPacket;

static void XBeeCommissioning(uint8_t timesButtonPress);
static void XBeeSetSleepMode(uint8_t mode);

/*..........................................................................*/
/* Join xbee */
void XBeeJoin(void) {
	power_usart1_enable();
	_delay_us(10);
	XBeeReset();
	_delay_ms(100);
	//Delay10KTCYx(100);
	// Simulate one press commissioning button
	XBeeCommissioning(COMMISSIONING_ONE_PRESS);
//	Delay10KTCYx(100);
	_delay_ms(100);
	// Set sleep mode 8
	XBeeSetSleepMode(DEEP_SLEEP_MODE);
	power_usart1_disable();
}

/*..........................................................................*/

/* Reset xbee */
void XBeeReset(void) {
	static uint8_t command[3] = {'F', 'R', '\0'};
	XBeeCreatePacket(&joinPacket);
	XBeeCreateATCommandPacket(&joinPacket, 0x01, command);
	XBeeSendPacket(&joinPacket);
}

static void XBeeCommissioning(uint8_t timesButtonPress) {
	static uint8_t command[3] = {'C', 'B', '\0'};
	uint8_t params[2];
	params[0] = timesButtonPress;
	params[1] = '\0';
	XBeeCreatePacket(&joinPacket);
	XBeeCreateCompleteATCommandPacket(&joinPacket, 0x01, command, params, 0x01);
	XBeeSendPacket(&joinPacket);
}

static void XBeeSetSleepMode(uint8_t mode) {
	static uint8_t command[3] = {'S', 'M', '\0'};
	uint8_t params[2];
	params[0] = mode;
	params[1] = '\0';
	XBeeCreatePacket(&joinPacket);
	XBeeCreateCompleteATCommandPacket(&joinPacket, 0x01, command, params, 0x01);
	XBeeSendPacket(&joinPacket);
}
