/*
 * meb5540b.c
 *
 * Created: 17/06/2015 10:57:17
 *  Author: Diego
 */ 

  

  #include <avr/io.h>
  #include <avr/sleep.h>
  #include <avr/interrupt.h>
  #include <math.h>

  #include <util/delay.h>

  #ifdef MOTE_ENDDEVICE

  #include "INCLUDE/digitales.h"
  #include "INCLUDE/timer0.h"

   unsigned HumiError = FALSE;
   unsigned BaroError = FALSE;
   unsigned BMP085Error = FALSE;

   unsigned int d1,d2;

   unsigned int w[4];
   unsigned int fc[6];
   signed short coef_BMP085[11];

   unsigned int Pressure, BaroTemp;
   unsigned int Humidity;

/******************************************************************************
Sensor digital presión-temperatura  MS5540b
******************************************************************************/
void BarometerInit(void)
{
	unsigned short i;
	//DIN, SCLK, MCLK -- salida, DOUT -- entrada
	PDIGITAL_DDR |= BAROMETER_DIN + BAROMETER_SCLK;
	PDIGITAL_DDR &= ~BAROMETER_DOUT;
	BaroError = FALSE;
	//Un reset antes de cada tirada confirma el buen protocolo
	BarometerReset();
	for (i=0; i<4; i++)
	{
		//Obtenemos las 4 words
		w[i] = BarometerGetW(i);
	}
	for (i=0; i<6; i++)
	{
		//Convertimos las words en coeficientes
		fc[i] = BarometerConvertWtoC5534(i, w[0], w[1], w[2], w[3]);
	}
	return;
}

void BarometerUpdate(void)
{
	//Timer 0 -- 31,25Hz
	timer_32768Hz_init();
	BarometerInit();
	d1 = BarometerGetD1();
	d2 = BarometerGetD2();
	BarometerCalcPT5534(Pressure,BaroTemp, d1, d2);
	timer_32768Hz_stop();
	return;
}

void BarometerCalcPT5534( unsigned int pressure,
unsigned int temperature,
unsigned int d1_arg,
unsigned int d2_arg)
{
	float fd1, fd2, x,dt, off, sens;

	fd1 = (float) d1_arg;
	fd2 = (float) d2_arg;

	dt   =   fd2 - ((8.0 * fc[4]) + 20224.0);
	off  =   fc[1] * 4.0 + (((fc[3]-512.0)*dt)/4096.0);
	sens =   24576.0 +  fc[0] + ((fc[2]*dt)/1024.0);
	x    =   (( sens * (fd1- 7168.0)) / 16384.0) - off;
	
	pressure = (250.0 + (x / 32.0))*10;
	//Máximo de presión 1100 mbar
	if(pressure > 11000.0) pressure = 0;
	vector_datos_micro[posicion_sensor] = (pressure>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (pressure);
	++posicion_sensor;
	
	temperature =  (200.0 +((dt*(fc[5]+50.0))/1024.0));
	//Máximo de temperatura 85 ºC
	if(temperature > 850.0) temperature = 0;
	vector_datos_micro[posicion_sensor] = (temperature>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (temperature);
	++posicion_sensor;
	longitud_datos_sensores+=4;
	return;
}

unsigned int BarometerConvertWtoC5534(unsigned short ix,
unsigned int W1,
unsigned int W2,
unsigned int W3,
unsigned int W4)
{
	unsigned int c;
	unsigned int x, y;

	c = 0;
	switch (ix)
	{
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

unsigned int BarometerGetD1(void)
{
	unsigned int d1;

	BarometerSendLsbFirst(0x2F, 8);
	BarometerSendLsbFirst(0x00, 2);

	if (BarometerGetDOUT()==FALSE) BaroError = 1;   // line should be at 1 now
	BarometerSendLsbFirst(0x00, 2);

	if (!BaroError) BaroError = BarometerWaitOnDoutFall();
	if (!BaroError) d1 = BarometerGet16();
	else d1 = 0;
	return(d1);
}

unsigned int BarometerGetD2(void)
{
	unsigned int d2;

	BarometerSendLsbFirst(0x4F, 8);
	BarometerSendLsbFirst(0x00, 3);                 // Note the difference
	// with BarometerGetD1

	if (BarometerGetDOUT()==FALSE) BaroError = 1;   // line should be at 1 now
	BarometerSendLsbFirst(0x00, 1);

	if (!BaroError) BaroError = BarometerWaitOnDoutFall();
	if (!BaroError) d2 = BarometerGet16();
	else d2 = 0;
	return(d2);
}

unsigned int BarometerGetW(unsigned short index)
{
	unsigned int data;

	data = 0;
	switch (index)
	{
		case 0:
		BarometerSendLsbFirst((char) 0x57, (char) 8);
		BarometerSendLsbFirst((char) 0x01, (char) 5);
		data = BarometerGet16();
		break;

		case 1:
		BarometerSendLsbFirst((char) 0xD7, (char) 8);
		BarometerSendLsbFirst((char) 0x00, (char) 5);
		data = BarometerGet16();
		break;

		case 2:
		BarometerSendLsbFirst((char) 0x37, (char) 8);
		BarometerSendLsbFirst((char) 0x01, (char) 5);
		data = BarometerGet16();
		break;

		case 3:
		BarometerSendLsbFirst((char) 0xB7, (char) 8);
		BarometerSendLsbFirst((char) 0x00, (char) 5);
		data = BarometerGet16();
		break;
	}
	BarometerSendLsbFirst(0x00, 1);  // to be compliant with the data sheet
	return(data);
}

void BarometerReset(void)
{
	BarometerSendLsbFirst(0x55, 8);
	BarometerSendLsbFirst(0x55, 8);
	BarometerSendLsbFirst(0x00, 5);
	return;
}

unsigned int BarometerGet16(void)
{
	char i;
	unsigned int v;

	v = 0;
	BarometerSetSCLK(FALSE);
	BarometerWaitOnePulse();

	for (i=0; i<16; i++)
	{
		BarometerSetSCLK(TRUE);
		BarometerWaitOnePulse();
		BarometerSetSCLK(FALSE);
		v = v << 1;
		if (BarometerGetDOUT()) v = v | 1;
		BarometerWaitOnePulse();
	}
	return(v);
}

void BarometerSendLsbFirst(char pattern, char nbr_clock)
{
	char i;
	char c;

	BarometerSetSCLK(FALSE);
	BarometerWaitOnePulse();
	for (i=0; i<nbr_clock; i++)
	{
		c = (char) (pattern & 1);
		if (c==1) BarometerSetDIN(TRUE);
		else BarometerSetDIN(FALSE);
		BarometerWaitOnePulse();
		BarometerSetSCLK(TRUE);
		BarometerWaitOnePulse();
		BarometerSetSCLK(FALSE);
		pattern = (char) (pattern >> 1);
	}
	return;
}

unsigned short BarometerWaitOnDoutFall(void)
{
	unsigned short working;
	long cnt;
	unsigned short error;

	working = TRUE;
	error   = 0;

	BarometerWaitOnePulse();
	cnt = 0;
	while (working)
	{
		working = BarometerGetDOUT();
		cnt++;
		BarometerWaitOnePulse();
		if (cnt>=20000)
		{
			working = FALSE;
			error   = 1;
		}
	}
	return(error);
}

void BarometerSetSCLK(unsigned short state)
{
	cli();
	if (state) PDIGITAL_PORT |= BAROMETER_SCLK;
	else PDIGITAL_PORT &= ~BAROMETER_SCLK;
	sei();
	return;
}

unsigned short BarometerGetSCLK(void)
{
	cli();
	if ((PDIGITAL_PIN & BAROMETER_SCLK))
	{
		sei();
		return 1;
	}
	else
	{
		sei();
		return 0;
	}
}

void BarometerSetDIN(unsigned short state)
{
	cli();
	if (state) PDIGITAL_PORT |= BAROMETER_DIN;
	else PDIGITAL_PORT &= ~BAROMETER_DIN;
	sei();
	return;
}

unsigned short BarometerGetDIN(void)
{
	cli();
	if ((PDIGITAL_PIN & BAROMETER_DIN))
	{
		sei();
		return 1;
	}
	else
	{
		sei();
		return 0;
	}
}

unsigned short BarometerGetDOUT(void)
{
	cli();
	if ((PDIGITAL_PIN & BAROMETER_DOUT))
	{
		sei();
		return 1;
	}
	else
	{
		sei();
		return 0;
	}
}

void BarometerWaitOnePulse(void)
{
	unsigned short i;
	for (i=0;i<3;i++);
	return;
}

#endif