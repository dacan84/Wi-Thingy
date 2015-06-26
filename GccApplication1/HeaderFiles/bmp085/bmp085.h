/*
* bmp085.h
*
* Created: 17/06/2015 10:30:36
*  Author: Diego
*/


#ifndef BMP085_H_
#define BMP085_H_
#include <stdint.h>
#include <stdbool.h>

#define BMP085_EOC		IOPORT_CREATE_PIN(PORTE, PINE5)
#define BMP085_SDA		IOPORT_CREATE_PIN(PORTE, PINE6)
#define BMP085_XCLR		IOPORT_CREATE_PIN(PORTE, PINE7)

/*******************************************************************************
Sensor digital BMP085 -- Presión/Temperatura/Altitud
*******************************************************************************/
#define ACK         1
#define noACK       0

#define BMP085_REG_W			0xF4    //11110100
#define BMP085_REG_R_MSB		0xF6    //11110110
#define BMP085_REG_R_LSB		0xF7    //11110111
#define BMP085_REG_R_XLSB		0xF8    //11111000

typedef struct {
	uint16_t  pressure;
	uint8_t  temperature;
} BmpData;

typedef struct {
	int8_t		AC1;
	int8_t		AC2;
	int8_t		AC3;
	uint8_t  	AC4;
	uint8_t		AC5;
	uint8_t		AC6;
	int8_t		B1;
	int8_t		B2;
	int8_t		MB;
	int8_t		MC;
	int8_t		MD;
} BmpCoef;


void Bmp085Init(void);
void Bmp085StartMeasure(uint8_t control_data);
int8_t Bmp085ReadMeasure(uint8_t registro);
uint16_t Bmp085ReadMeasureXLSB(uint8_t registro,uint8_t oss);
void Bmp085Measure(uint8_t control_register, BmpData* data);
void Bmp085Calculate(uint8_t temperatura_BMP085, uint16_t presion_BMP085,uint8_t oss);

#endif /* BMP085_H_ */