/*
 * sht11.h
 *
 * Created: 17/06/2015 9:57:22
 *  Author: Diego
 */ 


#ifndef SHT11_H_
#define SHT11_H_

	#include "bsp.h"
	
	#define SHT_DATA    IOPORT_CREATE_PIN(PORTA, PINA2)	   /* Data tx/rx pin */
	#define SHT_SCK		IOPORT_CREATE_PIN(PORTA, PINA4)	   /* Clk pin */
	
	#define SHT_MEASURE_TEMP    0x03    		/* Measure temperature command */
	#define SHT_MEASURE_HUMI    0x05    		/* Measure humidity command */
	#define SHT_STAT_REG_R      0x07    		/* Read status register command */
	#define SHT_STAT_REG_W      0x06    		/* Write status register command */
	#define SHT_RESET           0x1E    		/* Reset */

	#define SHT_LOW_POWER       0x01                /* 8bit RH / 12 bit TMP */

	#define SHT_ACK             1			/* Send ACK */
	#define SHT_NACK            0                   /* Not send ACK */

	/* Sht data class */
	typedef struct {

		union {
			uint16_t i;
		} temperature;

		union {
			uint16_t i;
		} humidity;
		uint8_t tempChk;
		uint8_t humiChk;
	} ShtData;

	/* Init pins and registers*/
	void Sht11Init(void);

	/* Measures temperature and humidity */
	uint8_t Sht11Measure(ShtData* data);

#endif /* SHT11_H_ */


#define ACK         1
#define noACK       0

/*******************************************************************************
Sensor digital SHT1x humedad - temperatura
*******************************************************************************/
#define HUMIDITY_STATUS_REG_W	0x06    //000	00110
#define HUMIDITY_STATUS_REG_R	0x07    //000	00111
#define HUMIDITY_MEASURE_TEMP	0x03    //000	00011
#define HUMIDITY_MEASURE_HUMI	0x05    //000	00101
#define HUMIDITY_RESET			0x1e    //000	11110

void HumidityWriteByte(unsigned short value);
unsigned short HumidityReadByte(unsigned short ack);
void HumidityTransStart(void);
void HumidityConnectionReset(void);
unsigned short HumidityReadStatus(void);
void HumidityWriteStatus(unsigned short value);
unsigned int HumidityMeasure(unsigned short mode);
void HumidityCalc(unsigned int p_humidity, unsigned int Humi_val_arg);
unsigned int HumidityGet(void);
void HumiditySoftReset(void);
void HumidityInit(void);
void HumidityUpdate(void);

