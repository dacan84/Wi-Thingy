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

static void Bmp085SendBit (bool estado);
static bool Bmp085GetBit(void);
static void Bmp085StopCondition(void);
static void Bmp085SendAck(bool value);
static bool Bmp085RecieveAck(void);
static void Bmp085StartCondition(void);
static void Bmp085Restart(void);
static void Bmp085SendByte(uint8_t value);
static uint8_t Bmp085ReceiveByte(void);
static void waitUntilConversionFinish(void);
static void Bmp085GetCoefCal(void);
static void waitWhilePG5isHigh(void);
static void waitWhilePG5isLow(void);
static uint8_t ossSelector(uint8_t control_register, uint8_t oss);


bool BMP085Error = false;
int8_t bmpCoef[11];	   //TODO: meditar sonbre como hacer esto porque tenemos datos con diferentes tipos de datos declarados con el mismo tipo.

/*******************************************************************************
Sensor digital BMP085 -- Presión/Temperatura/Altitud
*******************************************************************************/
void Bmp085Init(void) {
	//Los ponemos de salida o entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BMP085_XCLR,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BMP085_EOC,IOPORT_DIR_INPUT);

	//Dejamos SDA en alto para evitar Start's indeseados
	ioport_set_pin_high(BMP085_SDA);
	ioport_set_pin_high(BMP085_XCLR);
}

static void Bmp085StartCondition(void){
	//Enable Supply of timer0
	power_timer0_enable();

	//Ponemos el SCL en alto por precaución	  ???
	PORTG|=_BV(PORTG5);
	
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Start del SDA -- 0
	Bmp085SendBit(0);

	//Activamos el SCL -- empieza en alto
	Timer0_50KHzInit();
	waitWhilePG5isHigh();
	
}

static void Bmp085Restart(void) {
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición Inicial de Start del SDA -- 1
	Bmp085SendBit(1);

	//Mientras este en 0
	waitWhilePG5isLow();

	//Condición de Start del SDA -- 0
	Bmp085SendBit(0);

	//Condición de Start del SCL -- 0
	//Esperar mientras este en 1
	waitWhilePG5isHigh();
}

static void Bmp085SendByte(uint8_t value) {
	uint8_t i;

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2) {
		if(value & i) {
			Bmp085SendBit(1);
			} else {
			Bmp085SendBit(0);
		}
		//Mientras este en 0
		waitWhilePG5isLow();
		//Mientras este en 1
		waitWhilePG5isHigh();
	}
}

static bool Bmp085RecieveAck(void) {
	bool bmp085Ack;
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	Bmp085SendBit(1);

	//Ponemos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	//Mientras este en 0
	waitWhilePG5isLow();
	
	//Leemos el ACK
	bmp085Ack=Bmp085GetBit();

	//Condición de Start del SCL -- 0
	waitWhilePG5isHigh();
	return bmp085Ack;
}

static uint8_t Bmp085ReceiveByte(void) {
	uint8_t i,dato =0;
	
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	Bmp085SendBit(1);

	//Aseguramos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2)	{
		//Mientras este en 0
		waitWhilePG5isLow();

		if(Bmp085GetBit()) {
			dato|=i;
		}

		//Mientras este en 1
		waitWhilePG5isHigh();
	}
	return dato;
}

static void Bmp085SendAck(bool value) {
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//ACK  -- 0 // noACK --1
	Bmp085SendBit(value);

	//Mientras este en 0
	waitWhilePG5isLow();
	//Mientras este en 1
	waitWhilePG5isHigh();
}

static void Bmp085StopCondition(void) {
	//Mientras este en 0
	waitWhilePG5isLow();
	//Paramos el reloj
	Timer0Stop();

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Stop del SDA -- 1
	Bmp085SendBit(1);

	//Disable supply of timer0
	power_timer0_disable();
	
	//Ponemos el SCL en alto por precaución
	PORTG|=_BV(PORTG5);
}

static void Bmp085GetCoefCal(void) {
	uint8_t i;
	//Los coeficientes van desde AA (170) hasta BF(191)
	for(i=0;i<11;i++) {
		if(i==3 || i==4 || i==5) {
			bmpCoef[i] = (uint8_t) Bmp085ReadMeasure(170+i*2);
			} else {
			bmpCoef[i] = Bmp085ReadMeasure(170+i*2);
		}
	}
}

void Bmp085StartMeasure(uint8_t control_data) {
	bool bmp085Ack;

	Bmp085StartCondition();
	Bmp085SendByte(0xEE);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}
	
	Bmp085SendByte(0xF4);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1){
		BMP085Error=true;
	}
	Bmp085SendByte(control_data);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}
	
	Bmp085StopCondition();
	waitUntilConversionFinish();
}

int8_t Bmp085ReadMeasure(uint8_t registro)	  {
	bool bmp085Ack;
	int8_t data_BMP085;
	uint8_t data_msb,data_lsb;

	Bmp085StartCondition();
	Bmp085SendByte(0xEE);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1){
		BMP085Error=true;
	}
	
	Bmp085SendByte(registro);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}
	
	Bmp085Restart();
	Bmp085SendByte(0xEF);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}
	
	data_msb=Bmp085ReceiveByte();
	Bmp085SendAck(0);

	data_lsb=Bmp085ReceiveByte();
	Bmp085SendAck(1);

	Bmp085StopCondition();

	data_BMP085 = (int8_t)((data_msb<<8) + data_lsb);
	return data_BMP085;
}

uint16_t Bmp085ReadMeasureXLSB(uint8_t registro,uint8_t oss)	 {
	bool bmp085Ack;
	uint16_t data_BMP085;
	uint8_t data_msb,data_lsb,data_xlsb;

	Bmp085StartCondition();
	Bmp085SendByte(0xEE);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}

	Bmp085SendByte(registro);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}

	Bmp085Restart();
	Bmp085SendByte(0xEF);
	bmp085Ack=Bmp085RecieveAck();
	if(bmp085Ack==1) {
		BMP085Error=true;
	}

	data_msb=Bmp085ReceiveByte();
	Bmp085SendAck(0);

	data_lsb=Bmp085ReceiveByte();
	Bmp085SendAck(0);

	data_xlsb=Bmp085ReceiveByte();
	Bmp085SendAck(1);

	Bmp085StopCondition();

	data_BMP085 = (uint16_t)(((data_msb*65536) + (data_lsb<<8) + (data_xlsb))>>(8-oss));
	return data_BMP085;
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
void Bmp085Measure(uint8_t control_register, BmpData* data)  {
	uint8_t oss;
	
	oss = ossSelector(control_register, oss);

	//Dejamos SDA en alto para evitar Start's indeseados
	ioport_set_pin_high(BMP085_SDA);
	ioport_set_pin_high(BMP085_XCLR);

	//El primer paso por sensores tomamos los coeficientes de calibración
	Bmp085GetCoefCal();
	
	//Medimos temperatura
	Bmp085StartMeasure(0x2E);
	data->temperature=(uint8_t)Bmp085ReadMeasure(0xF6);
	//Medimos presión
	Bmp085StartMeasure(control_register);
	data->pressure = (uint16_t)Bmp085ReadMeasureXLSB(0xF6,oss);
	// En principio los calculos se hacen fuera
	//BMP085_Calc(temperatura_BMP085,presion_BMP085,oss);

	//Apagamos el sensor
	ioport_set_pin_low(BMP085_XCLR);
}

static void Bmp085SendBit (bool estado)  {
	if (estado) {
		ioport_set_pin_high(BMP085_SDA);
		} else {
		ioport_set_pin_low(BMP085_SDA);
	}
}

static bool Bmp085GetBit(void) {
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

void Bmp085Calculate(uint8_t temperatura_BMP085, uint16_t presion_BMP085, uint8_t oss) {
	uint16_t	pres_calibrated = 0;
	int8_t		temp_calibrated = 0;
	int16_t		x1,x2,x3,b3,b5,b6,p;
	uint16_t	b4,b7;
	
	x1=(temperatura_BMP085-bmpCoef[5])*(bmpCoef[4]/pow(2,15));
	x2=(bmpCoef[9]*pow(2,11))/(x1+bmpCoef[10]);
	b5=x1+x2;
	temp_calibrated=(b5+8)/pow(2,4);

	b6=b5-4000;
	x1=(bmpCoef[7]*(b6*b6/pow(2,12)))/pow(2,11);
	x2=bmpCoef[1]*b6/pow(2,11);
	x3=x1+x2;
	b3=(((bmpCoef[0]*4+x3)<<oss) + 2)/4;
	x1=bmpCoef[2]*b6/pow(2,13);
	x2=(bmpCoef[6]*(b6*b6/pow(2,12)))/pow(2,16);
	x3=((x1+x2)+2)/pow(2,2);
	b4=bmpCoef[3]*(uint16_t)(x3+32768)/pow(2,15);
	b7=((uint16_t)presion_BMP085-b3)*(50000>>oss);
	if(b7<0x80000000){p=(b7*2)/b4;}
	else{p=(b7/b4)*2;}
	x1=(p/pow(2,8))*(p/pow(2,8));
	x1=(x1*3038)/pow(2,16);
	x2=(-7357*p)/pow(2,16);
	pres_calibrated=p+(x1+x2+3791)/pow(2,4);
	
	//Tenemos que poner esta cifra para que sean 2 bytes
	pres_calibrated=pres_calibrated/10;
}
