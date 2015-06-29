/*
* ntc.c
*
* Created: 17/06/2015 10:57:01
*  Author: Diego
*/

#include "averaged_adc.h"
#include "..\ntc/ntc.h"

void NtcInit(void) {
	AdcReInit();
}

void NtcMeasure(uint16_t* data) {
	AdcConvert(NTC_AN_CH, data);
}
