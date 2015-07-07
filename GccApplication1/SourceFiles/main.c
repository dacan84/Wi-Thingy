#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "bsp.h"
#include "interrupts.h"
#include "sensor_proxy.h"
#include "network_proxy.h"
#include "XBee/digi_api.h"
#include <util/delay.h>
   bool papara = 0;
/*************************************************************************************
Programa principal
**************************************************************************************/
int main(void) {
 	DisableGeneralInterrupts();
	BspInit();
	XBeeInit();
	SensorProxyInit();
	EnableGeneralInterrupts();
	
	while(1) {		  		
		//sleep_mode();
	}
}
