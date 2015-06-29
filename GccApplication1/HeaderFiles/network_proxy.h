/*
 * network_proxy.h
 *
 * Created: 15/06/2015 12:31:40
 *  Author: Diego
 */ 


#ifndef NETWORK_PROXY_H_
#define NETWORK_PROXY_H_ 
#include <stdbool.h>

#define XBEE_INT_LEVEL    IOPORT_CREATE_PIN(PORTB, PINB0)

void SendMeasures(void);
bool NetworkAwake(void);
void ClearNetworkInterrupt(void);

#endif /* NETWORK_PROXY_H_ */