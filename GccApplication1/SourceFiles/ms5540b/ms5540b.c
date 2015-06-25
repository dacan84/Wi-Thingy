/*
* meb5540b.c
*
* Created: 17/06/2015 10:57:17
*  Author: Diego
*/

#include <avr/io.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>
#include <ioport.h>
#include "timer0.h"

bool msbError = FALSE;

uint16_t w[4];
uint16_t fc[6];

MsData msData;

/******************************************************************************
Sensor digital presi�n-temperatura  MS5540b
******************************************************************************/
void  Ms5540Init(void) {
	uint8_t i;
	//DIN, SCLK, MCLK -- salida, DOUT -- entrada
	ioport_set_pin_dir(MSB_DIN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(MSB_SCLK,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(MSB_DOUT,IOPORT_DIR_INPUT);
	
	msbError = FALSE;
	//Un reset antes de cada tirada confirma el buen protocolo
	Ms5540Reset();
	for (i=0; i<4; i++) {
		//Obtenemos las 4 words
		w[i] = Ms5540GetWCoefficients(i);
	}
	for (i=0; i<6; i++) {
		//Convertimos las words en coeficientes
		fc[i] = Ms5540ConvertWCoefficients(i, w[0], w[1], w[2], w[3]);
	}
}

void Ms5540Measure(MsData* data) {	
	//Timer 0 -- 31,25Hz
	Timer0_32768HzInit();
	 Ms5540Init();
	data->pressureD1 = Ms5540GetD1Pressure();
	data->temperatureD2 = Ms5540GetD2Temperature();
	//Ms5540Calculate(Pressure,BaroTemp, d1, d2);
	// en principio no se calcula.
	Timer0_32768HzStop();
}

void Ms5540Calculate(MsData* data) {//ShtData* data
	MsCalculateData msCalculateData;
	float fd1, fd2, x,dt, off, sens;

	fd1 = (float) msData.pressureD1;
	fd2 = (float) msData.temperatureD2;

	dt   =   fd2 - ((8.0 * fc[4]) + 20224.0);
	off  =   fc[1] * 4.0 + (((fc[3]-512.0)*dt)/4096.0);
	sens =   24576.0 +  fc[0] + ((fc[2]*dt)/1024.0);
	x    =   (( sens * (fd1- 7168.0)) / 16384.0) - off;
	
	
	//TODO: retocar este codigo asquroso.
	msCalculateData.pressure = (250.0 + (x / 32.0))*10;
	//M�ximo de presi�n 1100 mbar
	if(msCalculateData.pressure > 11000.0) msCalculateData.pressure = 0;
	vector_datos_micro[posicion_sensor] = (pressure>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (pressure);
	++posicion_sensor;
	
	msCalculateData.temperature	 =  (200.0 +((dt*(fc[5]+50.0))/1024.0));
	//M�ximo de temperatura 85 �C
	if(msCalculateData.temperature > 850.0) msCalculateData.temperature = 0;
	vector_datos_micro[posicion_sensor] = (temperature>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (temperature);
	++posicion_sensor;
	longitud_datos_sensores+=4;
}

uint16_t Ms5540ConvertWCoefficients(uint8_t ix, uint16_t W1, uint16_t W2, uint16_t W3,	uint16_t	 W4) {
	unsigned int c;
	unsigned int x, y;

	c = 0;
	switch (ix) {
		case 0:
		c =  (W1 >> 1) & 0x7FFF;
		break;
		case 1:
		x = (W3 << 6) & 0x0FC0;
		y =  W4       & 0x003F;
		c = x | y;
		break;
		case 2:
		c = (W4 >> 6) & 0x03FF;
		break;
		case 3:
		c = (W3 >> 6) & 0x03FF;
		break;
		case 4:
		x = (W1 << 10)& 0x0400;
		y = (W2 >> 6 )& 0x03FF;
		c = x | y;
		break;
		case 5:
		c =  W2       & 0x003F;
		break;
	}
	return(c);
}

uint16_t Ms5540GetD1Pressure(void) {
	uint16_t d1;

	Ms5540SendLsbFirst(0x2F, 8);
	Ms5540SendLsbFirst(0x00, 2);

	if (Ms5540GetDOUT()==FALSE) msbError = 1;   // line should be at 1 now
	Ms5540SendLsbFirst(0x00, 2);

	if (!msbError) msbError = Ms5540WaitOnDoutFall();
	if (!msbError) d1 = Ms5540Get16();
	else d1 = 0;
	return(d1);
}

uint16_t Ms5540GetD2Temperature(void) {
	uint16_t d2;

	Ms5540SendLsbFirst(0x4F, 8);
	Ms5540SendLsbFirst(0x00, 3);                 // Note the difference
	// with BarometerGetD1

	if (Ms5540GetDOUT()==FALSE) msbError = 1;   // line should be at 1 now
	Ms5540SendLsbFirst(0x00, 1);

	if (!msbError) msbError = Ms5540WaitOnDoutFall();
	if (!msbError) d2 = Ms5540Get16();
	else d2 = 0;
	return(d2);
}

uint16_t Ms5540GetWCoefficients(uint8_t index) {
	uint16_t data;

	data = 0;
	switch (index)
	{
		case 0:
		Ms5540SendLsbFirst((char) 0x57, (char) 8);
		Ms5540SendLsbFirst((char) 0x01, (char) 5);
		data = Ms5540Get16();
		break;

		case 1:
		Ms5540SendLsbFirst((char) 0xD7, (char) 8);
		Ms5540SendLsbFirst((char) 0x00, (char) 5);
		data = Ms5540Get16();
		break;

		case 2:
		Ms5540SendLsbFirst((char) 0x37, (char) 8);
		Ms5540SendLsbFirst((char) 0x01, (char) 5);
		data = Ms5540Get16();
		break;

		case 3:
		Ms5540SendLsbFirst((char) 0xB7, (char) 8);
		Ms5540SendLsbFirst((char) 0x00, (char) 5);
		data = Ms5540Get16();
		break;
	}
	Ms5540SendLsbFirst(0x00, 1);  // to be compliant with the data sheet
	return(data);
}

void Ms5540Reset(void) {
	Ms5540SendLsbFirst(0x55, 8);
	Ms5540SendLsbFirst(0x55, 8);
	Ms5540SendLsbFirst(0x00, 5);
}

uint16_t Ms5540Get16(void) {
	char i;
	uint16_t v;

	v = 0;
	Ms5540SetSCLK(FALSE);
	Ms5540WaitOnePulse();

	for (i=0; i<16; i++) {
		Ms5540SetSCLK(TRUE);
		Ms5540WaitOnePulse();
		Ms5540SetSCLK(FALSE);
		v = v << 1;
		if (Ms5540GetDOUT()) v = v | 1;
		Ms5540WaitOnePulse();
	}
	return(v);
}

void Ms5540SendLsbFirst(char pattern, char nbr_clock) {
	char i;
	char c;

	Ms5540SetSCLK(FALSE);
	Ms5540WaitOnePulse();
	for (i=0; i<nbr_clock; i++) {
		c = (char) (pattern & 1);
		if (c==1) Ms5540SetDIN(TRUE);
		else Ms5540SetDIN(FALSE);
		Ms5540WaitOnePulse();
		Ms5540SetSCLK(TRUE);
		Ms5540WaitOnePulse();
		Ms5540SetSCLK(FALSE);
		pattern = (char) (pattern >> 1);
	}
}

uint8_t Ms5540WaitOnDoutFall(void) {
	bool working;
	uint16_t cnt;
	uint8_t error;

	working = TRUE;
	error   = 0;

	Ms5540WaitOnePulse();
	cnt = 0;
	while (working){
		working = Ms5540GetDOUT();
		cnt++;
		Ms5540WaitOnePulse();
		if (cnt>=20000) {
			working = FALSE;
			error   = 1;
		}
	}
	return(error);
}

void Ms5540SetSCLK(uint8_t state) {
	cli();
	if (state) PDIGITAL_PORT |= BAROMETER_SCLK;
	else PDIGITAL_PORT &= ~BAROMETER_SCLK;
	sei();
}

uint8_t Ms5540GetSCLK(void) {
	cli();
	if ((PDIGITAL_PIN & BAROMETER_SCLK)	{
		sei();
		return 1;
	}
	else {
		sei();
		return 0;
	}
}

void Ms5540SetDIN(uint8_t state) {
	cli();
	if (state) PDIGITAL_PORT |= BAROMETER_DIN;
	else PDIGITAL_PORT &= ~BAROMETER_DIN;
	sei();
}

uint8_t Ms5540GetDIN(void) {
	cli();
	if ((PDIGITAL_PIN & BAROMETER_DIN))	{
		sei();
		return 1;
	}
	else {
		sei();
		return 0;
	}
}

uint8_t Ms5540GetDOUT(void) {
	cli();
	if ((PDIGITAL_PIN & BAROMETER_DOUT)){
		sei();
		return 1;
	}
	else{
		sei();
		return 0;
	}
}

void Ms5540WaitOnePulse(void) {
	uint8_t i;
	for (i=0;i<3;i++);
}