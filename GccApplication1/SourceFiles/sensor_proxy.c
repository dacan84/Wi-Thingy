/*
* sensor_proxy.c
*
* Created: 15/06/2015 13:47:24
*  Author: Diego
*/

#include <ioport.h>
#include "sensor_proxy.h"
#include "averaged_adc.h"
#include "../sht11/sht11.h"
#include "../bmp085/bmp085.h"
#include "../ms5540b/ms5540b.h"
#include "../h25k5a/h25k5a.h"
#include "../ntc/ntc.h"
#include <math.h>

static void InternalAnalogSensorPowerCtrlInit(void);
static void ExternalAnalogSensorPowerCtrlInit(void);
static void	VFCPowerCtrlInit(void);

static void PowerUpSHT11(void);
static void PowerDownSHT11(void);

static void PowerUpMSB5540b(void);
static void PowerDownMSB5540b(void);

static void PowerUpBMP085(void);
static void PowerDownBMP085(void);

static void PowerUpInternalAnalogSensor(void);
static void PowerDownInternalAnalogSensor(void);

static void PowerUpExternalAnalogSensor(void);
static void PowerDownExternalAnalogSensor(void);

static void PowerUpVFC(void);
static void PowerDownVFC(void);

ShtData shtData;
uint16_t h25k5aData;
uint16_t ntcData;
MsData	msData;
//BmpData	bmpData;

void SensorProxyInit(void) {
	Sht11Init();
	InternalAnalogSensorPowerCtrlInit();
	NtcInit();
	H25k5aInit();
	Ms5540Init();
	//	BMP085Init();
}

void MeasureSensors(void) {
	
	// Analog Sensor Sequence
	PowerUpInternalAnalogSensor();
	// h25k5a;
	H25k5aMeasure(&h25k5aData);
	//ntc
	NtcMeasure(&ntcData);
	PowerDownInternalAnalogSensor();
	
	// Digital Sensor Sequence
	// SHT11
	Sht11Measure(&shtData);

	//TODO: implementar estos métodos.
	//MSB5540b sequence
	Ms5540Measure(&msData);
	
	//BMP085 sequence
	
}

static void InternalAnalogSensorPowerCtrlInit(void){
	ioport_set_pin_dir(INTERNAL_ANALOG_SENSOR_SUPPLY,IOPORT_DIR_OUTPUT);
}

static void ExternalAnalogSensorPowerCtrlInit(void){
	ioport_set_pin_dir(EXTERNAL_ANALOG_SENSOR_SUPPLY,IOPORT_DIR_OUTPUT);
}

static void	VFCPowerCtrlInit(void){
	ioport_set_pin_dir(VFC_SUPPLY,IOPORT_DIR_OUTPUT);
}

static void PowerUpInternalAnalogSensor(void){
	ioport_set_pin_high(INTERNAL_ANALOG_SENSOR_SUPPLY);
}
static void PowerDownInternalAnalogSensor(void){
	ioport_set_pin_low(INTERNAL_ANALOG_SENSOR_SUPPLY);
}

static void PowerUpExternalAnalogSensor(void){
	ioport_set_pin_high(EXTERNAL_ANALOG_SENSOR_SUPPLY);
}
static void PowerDownExternalAnalogSensor(void){
	ioport_set_pin_low(EXTERNAL_ANALOG_SENSOR_SUPPLY);
}

static void PowerUpVFC(void){
	ioport_set_pin_high(VFC_SUPPLY);
}

static void PowerDownVFC(void){
	ioport_set_pin_low(VFC_SUPPLY);
}

/* Las funciones no se implementan porque en este caso los sensores
digitales no tienen su alimentación controlada con un interruptor. */

static void PowerUpSHT11(void) {}
static void PowerDownSHT11(void) {}

static void PowerUpMSB5540b(void) {}
static void PowerDownMSB5540b(void) {}

static void PowerUpBMP085(void) {}
static void PowerDownBMP085(void) {}