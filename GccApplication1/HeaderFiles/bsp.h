/*
* BSP_H_.h
*
* Created: 15/06/2015 12:08:19
*  Author: Diego
*/


#ifndef BSP_H_
#define BSP_H_

#include <avr/io.h>
#include <ioport.h>	  
#include <stdint.h>
#include <stdbool.h>
#include <avr/common.h>

//Frecuencia para delays
#define F_CPU 4000000U

#define TRUE  1
#define FALSE 0

#define NO_MOBILE   0
#define MOBILE      1

#define SHT_ID      'S'
#define IRCA_ID     'I'
#define CO2D1_ID    'C'
#define BMP085_ID   'B'
#define H25K5A_ID	'H'
#define NTC_ID		'N'
#define MS_ID		'M'

void BspInit(void);

#endif /* BSP_H_ */