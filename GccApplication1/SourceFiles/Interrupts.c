/*
* Interrupts.c
*
* Created: 15/06/2015 13:47:56
*  Author: Diego
*/

#include "bsp.h"
#include "interrupts.h"
#include "sensor_proxy.h"
#include "network_proxy.h"
#include "digi_api.h"
#include "digi_serial.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>
bool x = false;
void EnableGeneralInterrupts(void) {
	sei();
}

void DisableGeneralInterrupts(void) {
	cli();
}

/********************************************************************************
END DEVICE
Interrupci�n de ciclo de sue�o del XBee
********************************************************************************/
void EnableExternalInterruptPCINT0(void)  {
	//Permitimos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT0);
}

void DisableExternalInterruptPCINT0(void)  {
	//Eliminamos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR &= ~(_BV(PCIE0));
	PCMSK0 &= ~(_BV(PCINT0));
}


void ClearExternalInterruptFlag (uint8_t bit) {
	EIFR &= ~(_BV(bit));
}

/******************************************************************
INTERRUPCI�N PRINCIPAL
******************************************************************/
ISR (PCINT0_vect) {

	x=NetworkAwake();
	//DisableGeneralInterrupts();
	
	//DisableExternalInterruptPCINT0();
	if (x){
		
		//ClearNetworkInterrupt();
		EnableGeneralInterrupts();
		//set_sleep_mode(SLEEP_MODE_IDLE);
		// Gather measurements
		MeasureSensors();
		Usart1PowerONandEnable();
		_delay_ms(1);
		// Send measurements
		SendMeasures();
		Usart1PowerOFFandDisable();		
	}
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	//EnableExternalInterruptPCINT0();
	EnableGeneralInterrupts();
	//ClearNetworkInterrupt();
}