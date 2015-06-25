/*
* msb5540b.h
*
* Created: 17/06/2015 10:31:26
*  Author: Diego
*/


#ifndef MSB5540B_H_
#define MS5540B_H_


#define ACK         1
#define noACK       0

#define MS_DIN		IOPORT_CREATE_PIN(PORTA, PINA3)	   /* Data in sensor pin */
#define MS_DOUT		IOPORT_CREATE_PIN(PORTA, PINA5)	   /* Data out sensor pin */
#define MS_SCLK		IOPORT_CREATE_PIN(PORTA, PINA6)	   /* Clk pin */

/* Msb data class */
typedef struct {
	float  pressure;
	float  temperature;
} MsCalculateData;

/* Msb data class */
typedef struct {
	uint16_t  pressureD1;
	uint16_t  temperatureD2;
} MsData;

/******************************************************************************
Sensor digital presión-temperatura  MS5540b
******************************************************************************/
void  Ms5540Init(void);

void Ms5540Measure(MsData* data);

void Ms5540Calculate(MsData* data);

uint16_t Ms5540ConvertWCoefficients(uint8_t ix, uint16_t W1, uint16_t W2, uint16_t W3,	uint16_t	 W4);

uint16_t Ms5540GetD1Pressure(void);

uint16_t Ms5540GetD2Temperature(void);

uint16_t Ms5540GetWCoefficients(uint8_t index);

void Ms5540Reset(void);

void Ms5540SendLsbFirst(char pattern, char nbr_clock);

uint8_t Ms5540WaitOnDoutFall(void);

void Ms5540SetSCLK(uint8_t state);

uint8_t Ms5540GetSCLK(void);

void Ms5540SetDIN(uint8_t state);

uint8_t Ms5540GetDIN(void);

uint8_t BarometerGetDOUT(void);

void BarometerWaitOnePulse(void);

#endif /* MS5540B_H_ */