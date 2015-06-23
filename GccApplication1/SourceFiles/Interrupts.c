/*
* Interrupts.c
*
* Created: 15/06/2015 13:47:56
*  Author: Diego
*/

#include "bsp.h"
#include "interrupts.h"
#include "sensor_proxy.h"
#include "network_proxy.h"
#include "digi_api.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

#define XBEE_INT_LEVEL    IOPORT_CREATE_PIN(PORTB, 0)

static bool NetworkAwake(void);

void EnableInterrupts(void) {
	sei();
}

void ClearInterrupt(void) {
	cli();
}


/********************************************************************************
END DEVICE
Interrupción de ciclo de sueño del XBee
********************************************************************************/
void EnableExternalInterruptPCINT0(void)  {
	//Permitimos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT0);
	return;
}

void DisableExternalInterruptPCINT0(void)  {
	//Eliminamos interrupcion externa en PCINT0 (CTS) por wake/sleep del xbee
	PCICR &= ~(_BV(PCIE0));
	PCMSK0 &= ~(_BV(PCINT0));
	return;
}

static bool NetworkAwake(void)	{
	
	if (ioport_get_pin_level(XBEE_INT_LEVEL)==IOPORT_PIN_LEVEL_HIGH) {
		return FALSE;
	}
	else   {
		return TRUE;
	}
}

/******************************************************************
INTERRUPCIÓN PRINCIPAL
******************************************************************/
ISR (PCINT0_vect) {
	
	ClearInterrupt();
	DisableExternalInterruptPCINT0();
	if (NetworkAwake()){

		//Metodo de sleep para las medidas de sensores
		set_sleep_mode(SLEEP_MODE_IDLE);
		//Habilitamos interrupciones para los sensores
		EnableInterrupts();
		// Gather measurements
		MeasureSensors();
		
		power_usart1_enable();
		
		XBeeInit();
		_delay_ms(1);
		// Send measurements
		SendMeasures();
		power_usart1_disable() ;		
	}
	
	EnableExternalInterruptPCINT0();

	//Prueba sin ack's
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	EnableInterrupts();
}

// TODO: ESTA ISR no se si la usare ni como. De momento la dejo ahi.

///*******************************************************************************
  	//COORDINADOR	& END DEVICE
  //Recepción de datos desde el Xbee
//**************************************************************************************/
//#ifdef MOTE_ENDDEVICE 
//ISR (USART1_RX_vect) 
//{
  ///******************************************************************
  //COMUNICACIÓN DEL ENDDEVICE -- ACK'S & STATUS
  //******************************************************************/
//
  ////Hasta que no llega la cabecera 7E no empezará a guardar
  //if (indice_ack == 0) 
  //{
		//vector_datos_xbee[indice_ack]=UDR1;
		//if (vector_datos_xbee[indice_ack]==0x7E) ++indice_ack;
  //}	
  //else if (indice_ack == 1) 
  //{
		//vector_datos_xbee[indice_ack]=UDR1;
		//if (vector_datos_xbee[indice_ack]==0x00) ++indice_ack;
  //}	
  //else if (indice_ack == 2) 
  //{
		//vector_datos_xbee[indice_ack]=UDR1;
		////Mas 3 porque empieza en 0
		//longitud_datos_ack=vector_datos_xbee[indice_ack]+3;
		////if(longitud_datos_ack>10 || longitud_datos_ack<5)
		//++indice_ack;
  //}
  //else if (indice_ack>2 && indice_ack<=longitud_datos_ack) 
  //{
		//vector_datos_xbee[indice_ack]=UDR1;
		//++indice_ack;	
  //}		
  ////Final de recepción del mensaje de Status
  //if ((longitud_datos_ack>3) && (indice_ack>longitud_datos_ack))
  //{
  		//longitud_datos_ack=0;
  		//indice_ack=0;
//
		////Transmit Status
		//if(vector_datos_xbee[3]==0x8B)
		//{
		  ////Delivery Status OK
		  //if (vector_datos_xbee[8]!=0x00) 
		    //++contador_fallos;
		  ////Discovery Status OK
		  //if (vector_datos_xbee[9]!=0x02)
		    //++contador_fallos;
		//}
//
		////Si ya ha llegado la primera trama a su destino
		//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		//sei();
  //}
  //_delay_ms(1);
//}  
//#endif

