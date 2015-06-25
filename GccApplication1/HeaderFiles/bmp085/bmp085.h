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
	uint16_t  temperature;
} BmpData;

void Init_BMP085(void);
void BMP085_Coef_Calib(void);
void BMP085_Start_Mesure(uint8_t control_data);
int8_t BMP085_Read_Mesure(uint8_t registro);
uint16_t BMP085_Read_Mesure_XLSB(uint8_t registro,uint8_t oss);
void BMP085_Calc(uint8_t temperatura_BMP085, uint16_t presion_BMP085,uint8_t oss);
void BMP085Update(uint8_t control_register);

#endif /* BMP085_H_ */