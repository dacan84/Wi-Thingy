/*
* bsp.c
*
* Created: 15/06/2015 13:46:18
*  Author: Diego
*/

#include "bsp.h"
#include "averaged_adc.h"
#include <avr/sleep.h>
#include <avr/power.h>

void BspInit(void) {
	//Apagado permanente de TWI, SPI, T/C 5, T/C 4, USART 3 y USART 2
	//Apagado temporal de T/C 0, T/C 1, T/C 2, T/C 3, USART 0, USART 1 y ADC
	power_all_disable();
	
	//Digital Input Disable
	DisableAllAnalogInputBuffer();
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}