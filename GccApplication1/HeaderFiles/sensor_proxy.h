/*
 * sensor_proxy.h
 *
 * Created: 15/06/2015 12:31:53
 *  Author: Diego
 */ 


#ifndef SENSOR_PROXY_H_
#define SENSOR_PROXY_H_


#include "bsp.h"

#define INTERNAL_ANALOG_SENSOR_SUPPLY		IOPORT_CREATE_PIN(PORTE, PINE5)	 //EN EL DISEÑO ESTA EN EL PINA0, pero por un fallo en el Hard esta conectado a este pin.
#define EXTERNAL_ANALOG_SENSOR_SUPPLY		IOPORT_CREATE_PIN(PORTA, PINA1)
#define VFC_SUPPLY							IOPORT_CREATE_PIN(PORTA, PINA7)

void SensorProxyInit(void);

void MeasureSensors(void);

#endif /* SENSOR_PROXY_H_ */