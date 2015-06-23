/*
 * bmp085.c
 *
 * Created: 17/06/2015 10:57:27
 *  Author: Diego
 */ 


#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/iom1281.h>
#include <util/delay.h>
#include "bmp085.h"

#ifdef MOTE_ENDDEVICE

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

/*******************************************************************************
Sensor digital BMP085 -- Presión/Temperatura/Altitud
*******************************************************************************/
void sendbit_BMP085 (bool estado)  {
	if (estado) {
		ioport_set_pin_high(BMP085_SDA);
	}
	else  { ioport_set_pin_low(BMP085_SDA);
	}
}

bool getbit_BMP085(void) {
	if (ioport_get_pin_level(BMP085_SDA)) {
		return 1;
		}
	else {
		return 0;
	}
}

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
	return;
}

void BMP085_Start_Condition(void){
	//Desconectar reducción consumo del timer0
	pr_tc0_off();

	//Ponemos el SCL en alto por precaución	  ???
	PORTG|=_BV(PORTG5);
	
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Start del SDA -- 0
	sendbit_BMP085(0);

	//Activamos el SCL -- empieza en alto
	timer_50KHz_init();

	tiempo=0;
	//Condición de Start del SCL -- 0
	//Esperar mientras este en 1
	while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}
}

void BMP085_ReStart(void)	{
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición Inicial de Start del SDA -- 1
	sendbit_BMP085(1);

	tiempo=0;
	//Mientras este en 0
	while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}

	//TIEMPO¿?¿?¿
	//Condición de Start del SDA -- 0
	sendbit_BMP085(0);

	tiempo=0;
	//Condición de Start del SCL -- 0
	//Esperar mientras este en 1
	while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}
	return;
}

void mandar_byte_BMP085(unsigned char value) {
	unsigned char i;

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2)
	{
		if(value & i) sendbit_BMP085(1);
		else sendbit_BMP085(0);

		tiempo=0;
		//Mientras este en 0
		while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}

		tiempo=0;
		//Mientras este en 1
		while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}
	}
}

bool recibir_ACK_BMP085(void) {
	bool ACK_BMP085;
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	sendbit_BMP085(1);

	//Ponemos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	tiempo=0;
	//Mientras este en 0
	while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}
	
	//Leemos el ACK
	ACK_BMP085=getbit_BMP085();

	tiempo=0;
	//Mientras este en 1
	while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}

	return ACK_BMP085;
}

unsigned char recibir_byte_BMP085(void)	{
	unsigned char i,dato;
	dato=0;
	
	//Soltamos SDA (paso intermedio) -- Ponemos el SDA a 1
	sendbit_BMP085(1);

	//Aseguramos SDA de entrada
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_INPUT);

	//Escribimos en bajo, leemos en alto
	for(i=0x80;i>0;i/=2)
	{
		tiempo=0;
		//Mientras este en 0
		while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}

		if(getbit_BMP085()) dato|=i;

		tiempo=0;
		//Mientras este en 1
		while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}
	}
	return dato;
}

void mandar_ACK_BMP085(bool value) {
	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//ACK  -- 0 // noACK --1
	sendbit_BMP085(value);

	tiempo=0;
	//Mientras este en 0
	while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}
	
	tiempo=0;
	//Mientras este en 1
	while(PING & _BV(PING5)){if(tiempo>50){tiempo=0; break;} ++tiempo;}
}

void BMP085_Stop_Condition(void) {
	tiempo=0;
	//Mientras este en 0
	while(!(PING & _BV(PING5))){if(tiempo>50){tiempo=0; break;} ++tiempo;}

	//Paramos el reloj
	timer_stop();

	//Aseguramos SDA de salida
	ioport_set_pin_dir(BMP085_SDA,IOPORT_DIR_OUTPUT);

	//Condición de Stop del SDA -- 1
	sendbit_BMP085(1);

	//Conectar reducción consumo del timer0
	pr_tc0_on();

	//Ponemos el SCL en alto por precaución
	PORTG|=_BV(PORTG5);
}

void BMP085_Coef_Calib(void) {
	unsigned char i;
	//Los coeficientes van desde AA (170) hasta BF(191)
	for(i=0;i<11;i++)
	{
		if(i==3 || i==4 || i==5)
		coef_BMP085[i]=(unsigned short)BMP085_Read_Mesure(170+i*2);
		else
		coef_BMP085[i]=BMP085_Read_Mesure(170+i*2);
	}
}

void BMP085_Start_Mesure(unsigned char control_data) {
	bool ACK_BMP085;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	mandar_byte_BMP085(0xF4);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	mandar_byte_BMP085(control_data);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;
	
	BMP085_Stop_Condition();
	//Esperar final de conversion

	tiempo=0;
	while(!(ioport_get_pin_level(BMP085_EOC))) {
		if(tiempo>15000){tiempo=0; break;}
		++tiempo;
	}
}

signed short BMP085_Read_Mesure(unsigned char registro)	  {
	bool ACK_BMP085;
	unsigned char data_msb,data_lsb;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	mandar_byte_BMP085(registro);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	BMP085_ReStart();
	mandar_byte_BMP085(0xEF);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	data_msb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_lsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(1);

	BMP085_Stop_Condition();

	signed short data_BMP085;
	data_BMP085 = (signed short)((data_msb<<8) + data_lsb);
	return data_BMP085;
}

unsigned long BMP085_Read_Mesure_XLSB(unsigned char registro,unsigned char oss)	 {
	bool ACK_BMP085;
	unsigned char data_msb,data_lsb,data_xlsb;

	BMP085_Start_Condition();
	mandar_byte_BMP085(0xEE);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	mandar_byte_BMP085(registro);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	BMP085_ReStart();
	mandar_byte_BMP085(0xEF);
	ACK_BMP085=recibir_ACK_BMP085();
	if(ACK_BMP085==1) BMP085Error=TRUE;

	data_msb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_lsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(0);

	data_xlsb=recibir_byte_BMP085();
	mandar_ACK_BMP085(1);

	BMP085_Stop_Condition();

	unsigned long data_BMP085;
	data_BMP085 = (unsigned long)(((data_msb*65536) + (data_lsb<<8) + (data_xlsb))>>(8-oss));
	return data_BMP085;
}

void BMP085_Calc(unsigned short temperatura_BMP085,unsigned long presion_BMP085,unsigned char oss) {
	unsigned long pres_calibrated;
	short temp_calibrated;

	temp_calibrated=0;
	pres_calibrated=0;

	signed long x1,x2,x3,b3,b5,b6,p;
	unsigned long b4,b7;
	
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
	b4=coef_BMP085[3]*(unsigned long)(x3+32768)/pow(2,15);
	b7=((unsigned long)presion_BMP085-b3)*(50000>>oss);
	if(b7<0x80000000){p=(b7*2)/b4;}
	else{p=(b7/b4)*2;}
	x1=(p/pow(2,8))*(p/pow(2,8));
	x1=(x1*3038)/pow(2,16);
	x2=(-7357*p)/pow(2,16);
	pres_calibrated=p+(x1+x2+3791)/pow(2,4);
	
	//Tenemos que poner esta cifra para que sean 2 bytes
	pres_calibrated=pres_calibrated/10;

	
	vector_datos_micro[posicion_sensor] = (temp_calibrated>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (temp_calibrated);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (pres_calibrated>>8);
	++posicion_sensor;
	vector_datos_micro[posicion_sensor] = (pres_calibrated);
	++posicion_sensor;
	longitud_datos_sensores+=4;
}

void BMP085Update(unsigned char control_register)  {
	unsigned short temperatura_BMP085;
	unsigned long presion_BMP085;
	
	unsigned char oss;
	switch(control_register)
	{
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
	}

	Init_BMP085();
	//El primer paso por sensores tomamos los coeficientes de calibración
	if(comienzo==1)
	{
		BMP085_Coef_Calib();
		++comienzo;
	}
	
	//Medimos temperatura
	BMP085_Start_Mesure(0x2E);
	temperatura_BMP085=(unsigned short)BMP085_Read_Mesure(0xF6);

	//Medimos presión
	BMP085_Start_Mesure(control_register);
	presion_BMP085=(unsigned long)BMP085_Read_Mesure_XLSB(0xF6,oss);
	
	BMP085_Calc(temperatura_BMP085,presion_BMP085,oss);

	//Apagamos el sensor
	ioport_set_pin_low(BMP085_XCLR);
}
#endif