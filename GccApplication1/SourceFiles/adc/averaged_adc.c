/*
* averaged_adc.c
*
* Created: 15/06/2015 12:37:48
*  Author: Diego
*/

#include	<avr/interrupt.h>
#include	<avr/sleep.h>
#include	<avr/power.h>
#include	<adc.h>
#include	"bsp.h"
#include	"averaged_adc.h"

#define AVERAGE_FACTOR  16
#define DIV_AVERAGE     4

static void AdcCalibrate(void);
static uint16_t AdcReadValue(void);
static void AdcOpen(void);
static void AdcClose (void); 


/**
* ADC config & Reset conversión to neglect noise
*/
void AdcInit(void) {
	
	//Deshabilita el power reduction del ADC.
	power_adc_enable();
	//Digital Input Enable
	EnableUsedAnalogInputBuffer();
	adc_enable();
	
	adc_init(ADC_PRESCALER_DIV8);
	adc_set_admux(ADC_ADJUSTMENT_RIGHT | ADC_reset |ADC_VREF_AVCC);
	//TODO, Dejo la función de abajo por si la aqui puesta no funciona. 
	adc_set_mux(ADC_reset);
	
	adc_start_conversion();	
	while (!adc_conversion_is_complete()) {}
	ADC;
	//DisableUsedAnalogInputBuffer();	
	//adc_disable();
	//Habilitabilita el power reduction del ADC.
	//power_adc_disable();
}	

/**
* Devuelve una muestra promedio de AVERAGE_FACTOR muestras.
* Inicia el proceso tomando una muestra dummy para compensar el efecto del ruido
* de las entradas.
* A continuación, promedia tantas muestras como indica el factor de conversión.
*/
void AdcConvert(uint8_t channel, uint16_t* result) {
	uint32_t tmp = 0;
	uint8_t i = AVERAGE_FACTOR;
	AdcOpen();
	
	adc_init(ADC_PRESCALER_DIV8);
	adc_set_admux(ADC_ADJUSTMENT_RIGHT | ADC_reset |ADC_VREF_AVCC);
	//TODO, Dejo la función de abajo por si la aqui puesta no funciona. 
	//adc_set_mux(ADC_reset);
	
	//AdcCalibrate();
	//Referencia con AVCC
	adc_set_voltage_reference(ADC_VREF_AVCC);
	//Elegimos convertidor AD
	adc_set_mux(channel);
	while (i--) {
		ADC = 0;

		adc_start_conversion();
		while (!adc_conversion_is_complete()) {}
		//sleep_mode();
		tmp += AdcReadValue();
		//ClearAdcInterrupt();
	}
	*result = (uint16_t) tmp >> DIV_AVERAGE;
	AdcClose();
}

/**
* Realiza una conversión dummy para compensar el ruido de las entradas.
*/
static void AdcCalibrate(void) {
		adc_set_mux(ADC_reset);
		//Referencia con AVCC
		adc_set_voltage_reference(ADC_VREF_AVCC);
		//Start/Single conversion
		adc_start_conversion();
		sleep_mode();
		ADC;
		ClearAdcInterrupt();
}

static void AdcOpen(void) {
	//Digital Input Enable
	power_adc_enable();
	EnableUsedAnalogInputBuffer();
	adc_enable();
	set_sleep_mode(SLEEP_MODE_ADC);
	//adc_enable_interrupt();
	//ClearAdcInterrupt();
}

static void AdcClose (void) {
	//Deshabilita la conversión A/D y la interrupción de fin de conversión.
	adc_disable_interrupt();
	//Digital Input Disable
	DisableUsedAnalogInputBuffer();
	adc_disable();
	//Habilita Power Reduction ADC.
	power_adc_disable();
	set_sleep_mode(SLEEP_MODE_IDLE);
}

void ClearAdcInterrupt(void) {
	ADCSRA &= ~(_BV(ADIF));
}

/**
* Lee el valor del conversor.
*/
static uint16_t AdcReadValue(void) {
	return ADC;
}

/*************************************************************************************
Deshabilitar los input buffer de los analogicos
**************************************************************************************/
void EnableAllAnalogInputBuffer (void) {
	DIDR0=0x00;
	DIDR1=0x00;
	DIDR2=0x00;
}

void DisableAllAnalogInputBuffer (void) {
	DIDR0=0xFF;
	DIDR1=0xFF;
	DIDR2=0xFF;
}

void EnableUsedAnalogInputBuffer(void) {
	DIDR0=0b11111100;
}

void DisableUsedAnalogInputBuffer(void) {
	DIDR0=0x00;
}
  
//Esta interrupción la utilizo unicamente para
//despertar al micro después de la conversión del ADC.
ISR (ADC_vect) {
}