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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

void EnableInterrupts(void) {
	sei();
	ClearInterrupt();
}

void ClearInterrupt(void) {
	cli();
}

/********************************************************************************
END DEVICE
Interrupción de ciclo de sueño del XBee
********************************************************************************/
void EnableExternalInterruptPCINT0(void)  {
	//Permitimos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT0);
	return;
}

void DisableExternalInterruptPCINT0(void)  {
	//Eliminamos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR &= ~(_BV(PCIE0));
	PCMSK0 &= ~(_BV(PCINT0));
	return;
}

//FOR CLEAR GENERIC FLAG USE THIS STRUCTURE
//void ClearInterruptFlag (uint8_t bit) {
	//EIFR&=_BV(bit);
//}

void ClearExternalInterruptFlag (uint8_t bit) {
	EIFR &= ~(_BV(bit));
}

/******************************************************************
INTERRUPCIÓN PRINCIPAL
******************************************************************/
ISR (PCINT0_vect) {
	
	if (NetworkAwake()){
		ClearNetworkInterrupt();
		//Metodo de sleep para las medidas de sensores
		set_sleep_mode(SLEEP_MODE_IDLE);
		//Habilitamos interrupciones para los sensores
		// Gather measurements
		MeasureSensors();
		power_usart1_enable();
		_delay_ms(1);
		// Send measurements
		SendMeasures();
		power_usart1_disable();		
	}
	//Prueba sin ack's
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	EnableInterrupts();
}