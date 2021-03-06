/*
* h25k5a.c
*
* Created: 17/06/2015 10:56:49
*  Author: Diego
*/

#include <adc.h>
#include <ioport.h>
#include "h25k5a.h"

static void H25k5aInitIO(void);

void H25k5aInit(void){
	H25k5aInitIO();
}

void H25k5aMeasure(uint16_t* data){
	AdcConvert(H25K5A_AN_CH, data);
}

static void H25k5aInitIO(void){
	ioport_set_pin_dir(ADC_1_PIN,IOPORT_DIR_INPUT);
}
