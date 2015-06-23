/*
* bmp085.h
*
* Created: 17/06/2015 10:30:36
*  Author: Diego
*/


#ifndef BMP085_H_
#define BMP085_H_
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

void sendbit_BMP085 (bool estado);
bool getbit_BMP085(void);
void Init_BMP085(void);
void BMP085_Start_Condition(void);
void BMP085_ReStart(void);
void mandar_byte_BMP085(unsigned char value);
bool recibir_ACK_BMP085(void);
unsigned char recibir_byte_BMP085(void);
void mandar_ACK_BMP085(bool value);
void BMP085_Stop_Condition(void);
void BMP085_Coef_Calib(void);
void BMP085_Start_Mesure(unsigned char control_data);
signed short BMP085_Read_Mesure(unsigned char registro);
unsigned long BMP085_Read_Mesure_XLSB(unsigned char registro,unsigned char oss);
void BMP085_Calc(unsigned short temperatura_BMP085, unsigned long presion_BMP085,unsigned char oss);
void BMP085Update(unsigned char control_register);


#endif /* BMP085_H_ */