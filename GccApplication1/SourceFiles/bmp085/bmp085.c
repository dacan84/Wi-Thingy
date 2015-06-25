/*
* bmp085.c
*
* Created: 17/06/2015 10:57:27
*  Author: Diego
*/
#include "bsp.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/iom1281.h>
#include <util/delay.h>
#include <avr/power.h>
#include <ioport.h>
#include "bmp085.h"
#include "timer0.h"

bool BMP085Error = false;
int8_t coef_BMP085[11];

BmpData bmpData;

static void sendbit_BMP085 (bool estado);
static bool getbit_BMP085(void);
static void BMP085_Stop_Condition(void);
static void mandar_ACK_BMP085(bool value);
static bool recibir_ACK_BMP085(void);
static void BMP085_Start_Condition(void);
static void BMP085_ReStart(void);
static void mandar_byte_BMP085(uint8_t value);
static uint8_t recibir_byte_BMP085(void);
static void waitUntilConversionFinish(void);
static void waitWhilePG5isHigh(void);
static void waitWhilePG5isLow(void);
static uint8_t ossSelector(uint8_t control_register, uint8_t oss);

/*******************************************************************************
Sensor digital BMP085 -- Presión/Temperatura/Altitud
*******************************************************************************/
void Init_BMP085(void){
	//Los ponemos de salida o entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BMP085_XCLR,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BMP085_EOC,IOPORT_DIR_INPUT);

	//Dejamos SDA en alto para evitar Start's indeseados
	ioport_set_pin_high(BMP085_SDA);
	ioport_set_pin_high(BMP085_XCLR);
	//Tiempo de inicialización
	_delay_ms(20);
}

static void BMP085_Start_Condition(void){
	//Enable Supply of timer0
	power_timer0_enable();

	//Ponemos el SCL en alto por precaución	  ???
	PORTG|=_BV(PORTG5);
	
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Start del SDA -- 0
	sendbit_BMP085(0);

	//Activamos el SCL -- empieza en alto
	Timer0_50KHzInit();
	waitWhilePG5isHigh();
	
}

static void BMP085_ReStart(void) {
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición Inicial de Start del SDA -- 1
	sendbit_BMP085(1);

	//Mientras este en 0
	waitWhilePG5isLow();

	//Condición de Start del SDA -- 0
	sendbit_BMP085(0);

	//Condición de Start del SCL -- 0
	//Esperar mientras este en 1
	waitWhilePG5isHigh();
}

static void mandar_byte_BMP085(uint8_t value) {
	uint8_t i;

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2) {
		if(value & i) {
			sendbit_BMP085(1);
			} else {
			sendbit_BMP085(0);
		}
		//Mientras este en 0
		waitWhilePG5isLow();
		//Mientras este en 1
		waitWhilePG5isHigh();
	}
}

static bool recibir_ACK_BMP085(void) {
	bool ACK_BMP085;
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	sendbit_BMP085(1);

	//Ponemos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	//Mientras este en 0
	waitWhilePG5isLow();
	
	//Leemos el ACK
	ACK_BMP085=getbit_BMP085();

	//Condición de Start del SCL -- 0
	waitWhilePG5isHigh();
	return ACK_BMP085;
}

static uint8_t recibir_byte_BMP085(void) {
	uint8_t i,dato =0;
	
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	sendbit_BMP085(1);

	//Aseguramos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2)	{
		//Mientras este en 0
		waitWhilePG5isLow();

		if(getbit_BMP085()) {
			dato|=i;
		}

		//Mientras este en 1
		waitWhilePG5isHigh();
	}
	return dato;
}

static void mandar_ACK_BMP085(bool value) {
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//ACK  -- 0 // noACK --1
	sendbit_BMP085(value);

	//Mientras este en 0
	waitWhilePG5isLow();
	//Mientras este en 1
	waitWhilePG5isHigh();
}

static void BMP085_Stop_Condition(void) {
	//Mientras este en 0
	waitWhilePG5isLow();
	//Paramos el reloj
	Timer0Stop();

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Stop del SDA -- 1
	sendbit_BMP085(1);

	//Disable supply of timer0
	power_timer0_disable();
	
	//Ponemos el SCL en alto por precaución
	PORTG|=_BV(PORTG5);
}

void BMP085_Coef_Calib(void) {
	uint8_t i;
	//Los coeficientes van desde AA (170) hasta BF(191)
	for(i=0;i<11;i++) {
		if(i==3 || i==4 || i==5) {
			coef_BMP085[i]=(uint8_t)BMP085_Read_Mesure(170+i*2);
			} else {
			coef_BMP085[i]=BMP085_Read_Mesure(170+i*2);
		}
	}
}

void BMP085_Start_Mesure(uint8_t control_data) {
	bool ACK_BMP085;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}
	
	mandar_byte_BMP085(0xF4);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1){
		BMP085Error=true;
	}
	mandar_byte_BMP085(control_data);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}
	
	BMP085_Stop_Condition();
	waitUntilConversionFinish();
}

int8_t BMP085_Read_Mesure(uint8_t registro)	  {
	bool ACK_BMP085;
	int8_t data_BMP085;
	uint8_t data_msb,data_lsb;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1){
		BMP085Error=true;
	}
	
	mandar_byte_BMP085(registro);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}
	
	BMP085_ReStart();
	mandar_byte_BMP085(0xEF);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}
	
	data_msb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_lsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(1);

	BMP085_Stop_Condition();

	data_BMP085 = (int8_t)((data_msb<<8) + data_lsb);
	return data_BMP085;
}

uint16_t BMP085_Read_Mesure_XLSB(uint8_t registro,uint8_t oss)	 {
	bool ACK_BMP085;
	uint16_t data_BMP085;
	uint8_t data_msb,data_lsb,data_xlsb;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}

	mandar_byte_BMP085(registro);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}

	BMP085_ReStart();
	mandar_byte_BMP085(0xEF);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) {
		BMP085Error=true;
	}

	data_msb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_lsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_xlsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(1);

	BMP085_Stop_Condition();

	data_BMP085 = (uint16_t)(((data_msb*65536) + (data_lsb<<8) + (data_xlsb))>>(8-oss));
	return data_BMP085;
}

void BMP085_Calc(uint8_t temperatura_BMP085, uint16_t presion_BMP085, uint8_t oss) {
	uint16_t	pres_calibrated = 0;
	int8_t		temp_calibrated = 0;
	int16_t x1,x2,x3,b3,b5,b6,p;
	uint16_t b4,b7;
	
	x1=(temperatura_BMP085-coef_BMP085[5])*(coef_BMP085[4]/pow(2,15));
	x2=(coef_BMP085[9]*pow(2,11))/(x1+coef_BMP085[10]);
	b5=x1+x2;
	temp_calibrated=(b5+8)/pow(2,4);

	b6=b5-4000;
	x1=(coef_BMP085[7]*(b6*b6/pow(2,12)))/pow(2,11);
	x2=coef_BMP085[1]*b6/pow(2,11);
	x3=x1+x2;
	b3=(((coef_BMP085[0]*4+x3)<<oss) + 2)/4;
	x1=coef_BMP085[2]*b6/pow(2,13);
	x2=(coef_BMP085[6]*(b6*b6/pow(2,12)))/pow(2,16);
	x3=((x1+x2)+2)/pow(2,2);
	b4=coef_BMP085[3]*(uint16_t)(x3+32768)/pow(2,15);
	b7=((uint16_t)presion_BMP085-b3)*(50000>>oss);
	if(b7<0x80000000){p=(b7*2)/b4;}
	else{p=(b7/b4)*2;}
	x1=(p/pow(2,8))*(p/pow(2,8));
	x1=(x1*3038)/pow(2,16);
	x2=(-7357*p)/pow(2,16);
	pres_calibrated=p+(x1+x2+3791)/pow(2,4);
	
	//Tenemos que poner esta cifra para que sean 2 bytes
	pres_calibrated=pres_calibrated/10;

	//vector_datos_micro[posicion_sensor] = (temp_calibrated>>8);
	//++posicion_sensor;
	//vector_datos_micro[posicion_sensor] = (temp_calibrated);
	//++posicion_sensor;
	//vector_datos_micro[posicion_sensor] = (pres_calibrated>>8);
	//++posicion_sensor;
	//vector_datos_micro[posicion_sensor] = (pres_calibrated);
	//++posicion_sensor;
	//longitud_datos_sensores+=4;
}

static uint8_t ossSelector(uint8_t control_register, uint8_t oss) {
	switch(control_register) {
		case 0x34:
		oss=0;
		break;

		case 0x74:
		oss=1;
		break;

		case 0xB4:
		oss=2;
		break;

		case 0xF4:
		oss=3;
		break;

		default:
		oss=0;
		break;
	}	return oss;
}

//Start_Measure Options: Resistro de control.
//Pressure 		-- 0x34 -- Ultra Low Power
//			 	-- 0x74 -- Standard
//			 	-- 0xB4 -- High Resolution
//			 	-- 0xF4 -- Ultra High Resolution
void BMP085Update(uint8_t control_register)  {
	uint8_t temperatura_BMP085;
	uint16_t presion_BMP085;
	uint8_t oss;
	
	oss = ossSelector(control_register, oss);

	Init_BMP085();

	//El primer paso por sensores tomamos los coeficientes de calibración
		BMP085_Coef_Calib();
	
	//Medimos temperatura
	BMP085_Start_Mesure(0x2E);
	temperatura_BMP085=(uint8_t)BMP085_Read_Mesure(0xF6);
	//Medimos presión
	BMP085_Start_Mesure(control_register);
	presion_BMP085=(uint16_t)BMP085_Read_Mesure_XLSB(0xF6,oss);
	// En principio los calculos se hacen fuera
	//BMP085_Calc(temperatura_BMP085,presion_BMP085,oss);

	//Apagamos el sensor
	ioport_set_pin_low(BMP085_XCLR);
}

static void sendbit_BMP085 (bool estado)  {
	if (estado) {
		ioport_set_pin_high(BMP085_SDA);
		} else {
		ioport_set_pin_low(BMP085_SDA);
	}
}

static bool getbit_BMP085(void) {
	if (ioport_get_pin_level(BMP085_SDA)) {
		return 1;
		} else {
		return 0;
	}
}

static void waitWhilePG5isLow(void) {
	uint8_t t=0;
	while(!(PING & _BV(PING5))){
		if(t>50){
			t=0;
			break;
		}
		++t;
	}
}

static void waitWhilePG5isHigh(void) {
	uint8_t t=0;
	
	//Esperar mientras este en 1
	while(PING & _BV(PING5)){
		if(t>50){
			t=0;
			break;
		}
		++t;
	}
}

static void waitUntilConversionFinish(void) {
	uint16_t t;
	while(!(ioport_get_pin_level(BMP085_EOC))) {
		if(t>15000){
			t=0;
			break;
		}
		++t;
	}
}
