#include <avr/sleep.h>
#include "bsp.h"
#include "interrupts.h"
#include "sensor_proxy.h"
#include "XBee/digi_api.h"

/*************************************************************************************
Programa principal
**************************************************************************************/
int main(void)
{
	BspInit();
	XBeeInit();
	SensorProxyInit();
	EnableInterrupts();

	while(1) {
		sleep_mode();
	}
}
