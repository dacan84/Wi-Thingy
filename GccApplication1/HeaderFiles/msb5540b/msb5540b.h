/*
* msb5540b.h
*
* Created: 17/06/2015 10:31:26
*  Author: Diego
*/


#ifndef MSB5540B_H_
#define MSB5540B_H_

#define ACK         1
#define noACK       0
/******************************************************************************
Sensor digital presión-temperatura  MS5540b
******************************************************************************/
void BarometerInit(void);

void BarometerUpdate(void);

void BarometerCalcPT5534( unsigned int pressure, unsigned int temperature,   unsigned int d1_arg, unsigned int d2_arg);

unsigned int BarometerConvertWtoC5534(unsigned short ix, unsigned int W1, unsigned int W2, unsigned int W3,	unsigned int W4);

unsigned int BarometerGetD1(void);

unsigned int BarometerGetD2(void);

unsigned int BarometerGetW(unsigned short index);

void BarometerReset(void);

void BarometerSendLsbFirst(char pattern, char nbr_clock);

unsigned short BarometerWaitOnDoutFall(void);

void BarometerSetSCLK(unsigned short state);

unsigned short BarometerGetSCLK(void);

void BarometerSetDIN(unsigned short state);

unsigned short BarometerGetDIN(void);

unsigned short BarometerGetDOUT(void);

void BarometerWaitOnePulse(void);

#endif /* MSB5540B_H_ */