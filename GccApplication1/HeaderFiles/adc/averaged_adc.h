/*
 * averaged_adc.h
 *
 * Created: 15/06/2015 12:09:43
 *  Author: Diego
 */ 


#ifndef AVERAGED_ADC_H_
#define AVERAGED_ADC_H_

#include <stdint.h>

//Defino los canales de cada uno de los ADC.
#define	ADC_0 	0
#define	ADC_1 	1
#define	ADC_2 	2
#define	ADC_3 	3
#define	ADC_4 	4
#define	ADC_5  	5
#define	ADC_6  	6
#define	Bateria 7
#define ADC_reset 8

void AdcInit(void);

void AdcReInit(void);

void AdcConvert(uint8_t channel, uint16_t* result);

 /*************************************************************************************
 Deshabilitar los input buffer de los analogicos
 **************************************************************************************/
 void EnableAllAnalogInputBuffer (void);

 void DisableAllAnalogInputBuffer (void);

 void EnableUsedAnalogInputBuffer(void); 
 
 void DisableUsedAnalogInputBuffer(void); 

#endif /* AVERAGED_ADC_H_ */



