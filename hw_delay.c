/*
 * hw_delay.c
 *
 * Created: 11-10-2016 12:23:06
 *  Author: Karol
 */ 


#include "hw_delay.h"
#include <util/atomic.h>

volatile static uint32_t time = 0;

uint32_t getCurrentTime(void){
	uint32_t tmp_time = 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON){
		tmp_time = time;	
	}
	return tmp_time;
}

void hw_delay_ms(uint32_t delay_time){
	//Zerowanie preskalera?
	uint32_t endTime = getCurrentTime() + delay_time;
	while (time != endTime);
}

void hw_sleep_ms(uint32_t delay_time){
	
	uint32_t endTime = getCurrentTime() + delay_time;
	MCUCR |= (1<<SE);
	while (getCurrentTime() != endTime){
		asm volatile("SLEEP");
		asm volatile("NOP");
	};
	MCUCR &= ~(1<<SE);
}

void hw_sleep(uint8_t sleep_mode){
	MCUCR |= (sleep_mode << SM0);
	MCUCR |= (1<<SE);
	asm volatile("SLEEP");
	asm volatile("NOP");
	MCUCR &= ~((1<<SE) | (1<<SM0) | (1<<SM1));
}

ISR(INT_VECTOR){
	time++;
}