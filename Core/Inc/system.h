
/*
 * system.h
 *
 *  Created on: Jan 4, 2025
 *      Author: mohnc
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

void Pin_config(void) {
	RCC->AHB1ENR |= 0b111; // Enable GPIOA, GPIOB and GPIOC clocks
	GPIOC->MODER &= ~(0b11 << 26); // Clear mode bits for PC13
	GPIOC->MODER |= (0b01 << 26);  // Set PC13 as output (0b01)
	GPIOC->OTYPER &= ~(1 << 13); // Clear bit 13 for push-pull
	GPIOC->OSPEEDR &= ~(0b11 << 26); // Clear speed bits (low speed)
	GPIOC->PUPDR &= ~(0b11 << 26); // No pull-up, no pull-down

	GPIOB->MODER &= ~((0b11 << (3 * 2)) | (0b11 << (4 * 2)) | (0b11 << (5 * 2))); // Clear mode bits
	GPIOB->MODER |= ((0b01 << (3 * 2)) | (0b01 << (4 * 2)) | (0b01 << (5 * 2)));  // Set mode to output (0b01)
	GPIOB->OTYPER &= ~((1 << 3) | (1 << 4) | (1 << 5)); // Clear bits for Push-Pull
	GPIOB->OSPEEDR &= ~((0b11 << (3 * 2)) | (0b11 << (4 * 2)) | (0b11 << (5 * 2))); // Clear speed bits
	GPIOB->PUPDR &= ~((0b11 << (3 * 2)) | (0b11 << (4 * 2)) | (0b11 << (5 * 2))); // No pull-up, no pull-down

    // Configure PA10 as Alternate Function (TIM1_CH3)
    GPIOA->MODER &= ~(0b11 << (10 * 2)); // Clear mode bits for PA10
    GPIOA->MODER |= (0b10 << (10 * 2));  // Set PA10 to Alternate Function mode
    GPIOA->AFR[1] &= ~(0b1111 << ((10 - 8) * 4)); // Clear AF bits for PA10
    GPIOA->AFR[1] |= (0b0001 << ((10 - 8) * 4));  // Set AF1 (TIM1_CH3)

    // Configure PA11 as Alternate Function (TIM1_CH4)
    GPIOA->MODER &= ~(0b11 << (11 * 2)); // Clear mode bits for PA11
    GPIOA->MODER |= (0b10 << (11 * 2));  // Set PA11 to Alternate Function mode
    GPIOA->AFR[1] &= ~(0b1111 << ((11 - 8) * 4)); // Clear AF bits for PA11
    GPIOA->AFR[1] |= (0b0001 << ((11 - 8) * 4));  // Set AF1 (TIM1_CH4)

    // Configure PA8 and PA9 as outputs
	GPIOA->MODER &= ~((0b11 << (8 * 2)) | (0b11 << (9 * 2))); // Clear mode bits for PA8, PA9
	GPIOA->MODER |= ((0b01 << (8 * 2)) | (0b01 << (9 * 2))); // Set PA8, PA9 as General-purpose output mode (0b01)
	GPIOA->OTYPER &= ~((1 << 8) | (1 << 9)); // Set Push-Pull mode for PA8, PA9
	GPIOA->OSPEEDR &= ~((0b11 << (8 * 2)) | (0b11 << (9 * 2))); // Low speed for PA8, PA9
	GPIOA->PUPDR &= ~((0b11 << (8 * 2)) | (0b11 << (9 * 2))); // No pull-up, no pull-down for PA8, PA9

	// Configure PB14 and PB15 as outputs
	GPIOB->MODER &= ~((0b11 << (14 * 2)) | (0b11 << (15 * 2))); // Clear mode bits for PB14, PB15
	GPIOB->MODER |= ((0b01 << (14 * 2)) | (0b01 << (15 * 2))); // Set PB14, PB15 as General-purpose output mode (0b01)
	GPIOB->OTYPER &= ~((1 << 14) | (1 << 15)); // Set Push-Pull mode for PB14, PB15
	GPIOB->OSPEEDR &= ~((0b11 << (14 * 2)) | (0b11 << (15 * 2))); // Low speed for PB14, PB15
	GPIOB->PUPDR &= ~((0b11 << (14 * 2)) | (0b11 << (15 * 2)));   // No pull-up, no pull-down for PB14, PB15

	// Configure PB2 and PB10 as inputs
	GPIOB->MODER &= ~((0b11 << (2 * 2)) | (0b11 << (10 * 2))); // Clear mode bits for PB2 and PB10
	GPIOB->PUPDR &= ~((0b11 << (2 * 2)) | (0b11 << (10 * 2))); // No pull-up, no pull-down (default)

    // Configure PC14 and PC15 as inputs
    GPIOC->MODER &= ~((0b11 << (14 * 2)) | (0b11 << (15 * 2))); // Clear mode bits for PC14 and PC15
    GPIOC->PUPDR &= ~((0b11 << (14 * 2)) | (0b11 << (15 * 2))); // No pull-up, no pull-down (default)
}

void Configure_TIM1_PWM(void) {
    // Enable TIM1 clock
    RCC->APB2ENR |= (1 << 0); // Enable TIM1 clock (bit 0)

    // Configure TIM1 for PWM mode
    TIM1->PSC = 4 - 1;       // Prescaler: 16 MHz / 16 = 1 MHz timer clock
    TIM1->ARR = 20000 - 1;    // Auto-reload: 1 MHz / 20000 = 50 Hz PWM frequency

    // Configure PWM mode 1 and enable preload
    TIM1->CCMR2 &= ~((0b111 << 4) | (0b111 << 12)); // Clear OC3M and OC4M bits
    TIM1->CCMR2 |= (0b110 << 4);  // PWM mode 1 on Channel 3 (PA10)
    TIM1->CCMR2 |= (0b110 << 12); // PWM mode 1 on Channel 4 (PA11)
    TIM1->CCMR2 |= (1 << 3) | (1 << 11); // Enable preload for CCR3 and CCR4 (OC3PE and OC4PE)

    // Enable channels
    TIM1->CCER |= (1 << 8);  // Enable Channel 3 output (PA10)
    TIM1->CCER |= (1 << 12); // Enable Channel 4 output (PA11)

    // Enable main output (MOE)
    TIM1->BDTR |= (1 << 15);

    // Start TIM1
    TIM1->CR1 |= (1 << 0); // Enable TIM1 counter
}

void Configure_TIM2(void) {
    // Enable TIM2 clock
    RCC->APB1ENR |= (1 << 0); // TIM2 clock enable

    // Set TIM2 for 1 Hz (1-second period)
    TIM2->PSC = 144 - 1;        // Prescaler: 84 MHz / 84 = 1 MHz
    TIM2->ARR = 20000 - 1;   // Auto-reload: 1 MHz / 1,000,000 = 1 Hz (1-second period)

    // Enable Update Interrupt
    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt

    // Enable TIM2
    TIM2->CR1 |= TIM_CR1_CEN; // Enable counter

    // Enable TIM2 interrupt in NVIC
    NVIC_SetPriority(TIM2_IRQn, 9); // Set interrupt priority
    NVIC_EnableIRQ(TIM2_IRQn);
}

void Configure_TIM3(void) {
    // Enable TIM3 clock
    RCC->APB1ENR |= (1 << 1); // TIM3 clock enable

    // Set TIM3 for 1 Hz (1-second period)
    TIM3->PSC = 84 - 1;        // Prescaler: 84 MHz / 84 = 1 MHz
    TIM3->ARR = 1000 - 1;   // Auto-reload: 1 MHz / 1,000,000 = 1 Hz (1-second period)

    // Enable Update Interrupt
    TIM3->DIER |= TIM_DIER_UIE; // Enable update interrupt

    // Enable TIM3
    TIM3->CR1 |= TIM_CR1_CEN; // Enable counter

    // Enable TIM3 interrupt in NVIC
    NVIC_SetPriority(TIM3_IRQn, 9); // Set interrupt priority
    NVIC_EnableIRQ(TIM3_IRQn);
}




#endif /* INC_SYSTEM_H_ */
