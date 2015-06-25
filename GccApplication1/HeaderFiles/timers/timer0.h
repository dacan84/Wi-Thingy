/*
 * timer0.h
 *
 * Created: 24/06/2015 10:48:48
 *  Author: Diego
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

void Timer0_50KHzInit(void);
void Timer0Change1MHz(void);
void Timer0_32768HzInit(void);
void Timer0_32768HzStop(void);
void Timer0ClockInit(void);
void Timer0Stop(void);

#endif /* TIMER0_H_ */