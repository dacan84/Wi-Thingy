/*
 * interrupts.h
 *
 * Created: 15/06/2015 12:10:29
 *  Author: Diego
 */ 


#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

void EnableInterrupts (void);

void ClearInterrupt (void);

void ClearExternalInterruptFlag (uint8_t bit);

void EnableExternalInterruptPCINT0 (void);

void DisableExternalInterruptPCINT0 (void);

#endif /* INTERRUPTS_H_ */