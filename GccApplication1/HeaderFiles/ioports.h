/*
 * ioports.h
 *
 * Created: 17/06/2015 12:09:27
 *  Author: Diego
 */ 


#ifndef IOPORTS_H_
#define IOPORTS_H_

	 /*****************************************************************************************
	 PUERTO A -- COMPLETO
	 *****************************************************************************************/
	 #define PDIGITAL_DDR	DDRA
	 #define PDIGITAL_PIN	PINA
	 #define PDIGITAL_PORT	PORTA

	 #define SWITCH_DDR	DDRA
	 #define SWITCH_PIN	PINA
	 #define SWITCH_PORT	PORTA
		PINA2
	 #define SW_DIG_S3			0x01	//Pin 0 Switch S3 de sensores analógicos internos
	 #define SW_DIG_S4			0x02	//Pin 1 Switch S4 de sensores analógicos externos
	 #define HUMIDITY_DATA		0x04 	//pin 2 Data del SHT11 (Humedad)
	 #define BAROMETER_DIN    	0x08	//pin 3 DIN del MS5540C (Barómetro)
	 #define HUMIDITY_SCK		0x10	//pin 4 SCK del SHT11 (Humedad)
	 #define BAROMETER_DOUT   	0x20	//pin 5 DOUT del MS5540C (Barómetro)
	 #define BAROMETER_SCLK   	0x40    //pin 6 SCLK del MS5540C (Barómetro)
	 #define SW_DIG_S2			0x80	//Pin 7 Switch S2 de alimentación del conector VFC

	 /*****************************************************************************************
	 PUERTO B -- COMPLETO
	 *****************************************************************************************/
	 #define CLOCK_DDR  	DDRB
	 #define CLOCK_PORT 	PORTB
	 #define CLOCK_PIN 	PINB

	 //pin0 es el PCINT0 de despertar del XBEE (AWAKE).
	 //pin1 conectado al /INT del RTC.
	 #define CLOCK_CE	0x04  	//pin2 CE del PCF2123 (RTC)
	 #define CLOCK_CLKOE 0x08    //pin3 CLKOE del PCF2123 (RTC)
	 #define CLOCK_SDI	0x10	//pin4 SDI del PCF2123 (RTC)
	 #define CLOCK_SDO	0x20	//pin5 SDO del PCF2123 (RTC)
	 //pin6 Inutilizado en el diseño
	 //pin7 32SCLK del Barómetro (CLCK0A)

	 /*****************************************************************************************
	 PUERTO C -- COMPLETO
	 *****************************************************************************************/
	 //VFC y DA

	 #define VFC_DDR		DDRC
	 #define VFC_PIN		PINC
	 #define VFC_PORT	PORTC

	 #define DA_DDR		DDRC
	 #define DA_PIN		PINC
	 #define DA_PORT		PORTC

	 #define A0MUX	  0x01	//pin 0
	 #define A1MUX	  0x02	//pin 1
	 #define CSPOT1    0x04 	//pin 2
	 #define CSPOT2    0x08	//pin 3
	 #define DINPOT    0x10  //pin 4
	 #define SDA		  0x20	//pin 5
	 #define A2MUX     0x40  //pin 6
	 //El pin7 es el ENABLE_AINSTRUM del VFC

	 /*****************************************************************************************
	 PUERTO D
	 *****************************************************************************************/
	 //pin 0 SCL del I2C
	 //pin 1 SDA del I2C
	 //pin 2 RxD del Usart1
	 //pin 3 TxD del Usart1
	 //pin 4 LIBRE en el conector del VFC
	 //pin 5 LIBRE en el conector del VFC
	 //pin 6 entrada cuasidigital (en el conector del VFC)
	 //pin 7 LIBRE en el conector del VFC

	 /*****************************************************************************************
	 PUERTO E
	 *****************************************************************************************/
	 /* Los pines 3, 4 y de este puerto se utilizan para la conexión de dos elementos distintos al Mote.
	 Siendo estos el módulo GSM/GPRS Telit GM862, el interface para la conexión de sensores y el sensor de presión.
	 En ningún caso se van a poder utilizar más de uno de estos dispositivos en cada caso, exceptuando el módulo telit y el
	 sensor de presión cuando se controle el consumo del módulo Telit mediante el apagado del mismo. Las definiciones
	 pueden cohexistir sin interferirse de ninguna manera. */

	 // Definiciones para el uso del módulo GSM/GPRS.
	 #define GSM_DDR			DDRE
	 #define GSM_PIN			PINE
	 #define GSM_PORT		PORTE

	 #define GSM_ON_OFF				PINE3		//pin3
	 #define GSM_RESET				PINE4		//pin4
	 #define GSM_POWER_CONTROL		PINE5		//pin5
	 #define GSM_STATUS				PINE6		//pin6 Sirve para monitorizar el estado del GSM, puede o no conectarse al uC.

	 #define COM_DDR			DDRE
	 #define COM_PIN			PINE
	 #define COM_PORT		PORTE

	 #define INTERFAZ_DDR  	DDRE
	 #define INTERFAZ_PORT 	PORTE
	 #define INTERFAZ_PIN 	PINE

	 #define BMP085_DDR		DDRE
	 #define BMP085_PIN		PINE
	 #define BMP085_PORT		PORTE
	 //pin 0 es el RXD0 que viene del PC
	 //pin 1 es el TXD0 que va al PC
	 #define SW_DIG_S5			0x04	//pin 2 Switch S5 de conexión de comunicaciones y digital IO
	 #define INTERFAZ_SDA 		0x08    //pin 3 I2C Manual DATA OUT
	 #define INTERFAZ_SDAI		0x10	//pin 4 I2C Manual DATA IN
	 #define BMP085_EOC	    	0x20 	//pin 5 EOC del BMP085 (Barómetro)
	 #define BMP085_SDA   		0x40	//pin 6 SDA del BMP085 (Barómetro)
	 #define BMP085_XCLR   		0x80	//pin 7 XCLR del BMP085 (Barómetro)

	 /*****************************************************************************************
	 PUERTO F
	 *****************************************************************************************/
	 //Pertenece a los ADC's y la programación por JTAG

	 /*****************************************************************************************
	 PUERTO G
	 *****************************************************************************************/
	 //pin 0 al conector del VFC
	 //pin 1 al conector del VFC
	 //pin 2 Inutilizado en el diseño
	 //pin 3 Inutilizado en el diseño
	 //pin 4 RTC_Signal del PCF2123 (RTC)
	 //pin 5 Clock_Gen (CLCK0B)

#endif /* IOPORTS_H_ */