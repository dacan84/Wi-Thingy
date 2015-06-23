/*
 * h25k5a.h
 *
 * Created: 17/06/2015 10:56:11
 *  Author: Diego
 */ 


#ifndef H25K5A_H_
#define H25K5A_H_

#include "bsp.h"
#include  "adc/averaged_adc.h"

#define H25K5A_AN_CH   ADC_1

void H25k5aInit(void);

void H25k5aMeasure(uint16_t* data);

#endif /* H25K5A_H_ */