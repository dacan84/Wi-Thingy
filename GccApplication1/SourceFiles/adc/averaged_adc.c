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
static void AdcClose (void); 


/**
* Configura el ADC
*/
void AdcInit(void) {
	//FIXME: necesario aqui el start?	 //start conversion?
	//ADCSRA = _BV(ADPS1) | _BV(ADPS0) | _BV(ADSC);
	adc_set_admux(ADC_MUX_GND | ADC_PRESCALER_DIV8 | ADC_ADJUSTMENT_RIGHT);
	
	//Referencia AVCC porque AREF = 0V.
	adc_set_voltage_reference(ADC_VREF_AVCC);
	//Digital Input Enable
	EnableUsedAnalogInputBuffer();
	//Deshabilita el power reduction del ADC.
	 power_adc_enable();
	//EN ADMUX REFS1 Y REFS0 NO LOS ESCRIBO, INICIALMENTE DEJO LA REFERENCIA INTERNA.
	//PARTE MENOS SIGNIFICATIVA ALINEADA A LA DERECHA, ADLAR = 0.
	//Habilita la conversión A/D.//Habilita interrupciones
	adc_enable();
	//TODO: esto no se si realmente hace falta, y si ponerlo aquí o en el ADC calibrate.
	// aquí o es suficiente con que
	adc_set_mux(ADC_reset);
	adc_start_conversion();	 //creo que este estart si que hace falta aquí
	while (!adc_conversion_is_complete()) {}
	ADC;
}	

void AdcReInit(void) {
	//FIXME: necesario aqui el start?	 //start conversion?
	//ADCSRA = _BV(ADPS1) | _BV(ADPS0) | _BV(ADSC);
	adc_set_admux(ADC_MUX_GND | ADC_PRESCALER_DIV8 | ADC_ADJUSTMENT_RIGHT);
	
	//Referencia AVCC porque AREF = 0V.
	adc_set_voltage_reference(ADC_VREF_AVCC);
	//Digital Input Enable
	EnableUsedAnalogInputBuffer();
	//Deshabilita el power reduction del ADC.
	power_adc_enable();
	//EN ADMUX REFS1 Y REFS0 NO LOS ESCRIBO, INICIALMENTE DEJO LA REFERENCIA INTERNA.
	//PARTE MENOS SIGNIFICATIVA ALINEADA A LA DERECHA, ADLAR = 0.
	//Habilita la conversión A/D.//Habilita interrupciones
	set_sleep_mode(SLEEP_MODE_ADC);
	adc_enable();
	adc_enable_interrupt();
	
	//TODO: esto no se si realmente hace falta, y si ponerlo aquí o en el ADC calibrate.
	// aquí o es suficiente con que
	adc_set_mux(ADC_reset);
	adc_start_conversion();	 //creo que este estart si que hace falta aquí
	sleep_mode();
	ADC;
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
	
	adc_set_mux(channel);	
	AdcCalibrate();
	while (i--) {
		ADC = 0;
		//Referencia con AVCC
		adc_set_voltage_reference(ADC_VREF_AVCC);
		//Elegimos convertidor AD
		adc_start_conversion();
		sleep_mode();
		tmp += AdcReadValue();	  //la funcion de lectura no sirve.
	}
	*result = (uint16_t) tmp >> DIV_AVERAGE;
	AdcClose();
}

/**
* Realiza una conversión dummy para compensar el ruido de las entradas.
*/
static void AdcCalibrate(void) {
		//Referencia con AVCC
		adc_set_voltage_reference(ADC_VREF_AVCC);
		//Start/Single conversion
		adc_start_conversion();
		sleep_mode();
		ADC;
}

/**
* Lee el valor del conversor.
*/
static uint16_t AdcReadValue(void) {
	return ADC;
}

static void AdcClose (void) {
	//Deshabilita la conversión A/D y la interrupción de fin de conversión.
	adc_disable();
	adc_disable_interrupt();
	//Habilita Power Reduction ADC.
	power_adc_disable();
	//Digital Input Disable
	DisableUsedAnalogInputBuffer();
	set_sleep_mode(SLEEP_MODE_IDLE);
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