/*
* ntc.c
*
* Created: 17/06/2015 10:57:01
*  Author: Diego
*/

#include "averaged_adc.h"
#include "ntc.h"
#include <ioport.h>

static void NtcInitIO(void);

void NtcInit(void) {
	NtcInitIO();
}

void NtcMeasure(uint16_t* data) {
	AdcConvert(NTC_AN_CH, data);
}

static void NtcInitIO(void){
	ioport_set_pin_dir(ADC_0_PIN,IOPORT_DIR_INPUT);
}

