/*
 * ntc1.h
 *
 * Created: 17/06/2015 10:55:59
 *  Author: Diego
 */ 


#ifndef NTC_H_
#define NTC_H_
   
   #include "bsp.h"
   #include "adc/averaged_adc.h"

   #define NTC_AN_CH   ADC_0

   void NtcInit(void);

   void NtcMeasure(uint16_t* data);

#endif /* NTC_H_ */