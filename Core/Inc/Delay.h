/*
 * Delay.h
 *
 *  Created on: Dec 17, 2024
 *      Author: OSCAR
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stdint.h"
#include "stm32f4xx.h"

void TIM9Config(void);
void Delay_us (uint16_t us);
void Delay_ms (uint16_t ms);

void TIM9Config(void)
{
	/* 1) Enable Timer Clock*/
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN; // Enable timer 9 clock

	/* 2) Set the prescalar and the ARR*/
	TIM9->PSC = 84-1; // 84MHz/84 = 1MHz ~ 1us delay
	TIM9->ARR = 0xFFFF;// Max ARR value

	/* 3) Enable the Timer, and wait for the update Flag to set*/
	TIM9->CR1 |= (1<<0); //Enable the Counter
	while(!(TIM9->SR & (1<<0))); //Update interrupt flag
}

void Delay_us (uint16_t us)
{
	/* 1) RESET the Counter */
	TIM9->CNT = 0;
	/* 2) Wait for the Counter to reach the entered value. each count would be 1us */
	while(TIM9->CNT < us);

}

void Delay_ms (uint16_t ms)
{
	for (uint16_t i = 0; i < ms; i++)
	{
		Delay_us(1000); //Delay of 1ms
	}
}


#endif /* INC_DELAY_H_ */
