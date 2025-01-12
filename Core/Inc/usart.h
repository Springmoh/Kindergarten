/*
 * usart.h
 *
 *  Created on: Jan 5, 2025
 *      Author: mohnc
 */

#ifndef INC_USART_H_
#define INC_USART_H_

volatile uint8_t *txBuffer; // Pointer to transmission buffer
volatile uint16_t txSize;   // Size of the transmission buffer
volatile uint16_t txIndex;  // Current index of transmission

void Configure_USART2(void) {
//    // Step 1: Enable USART2
//    USART2->CR1 = 0;  // Reset control register
//    USART2->BRR = 0x02D9;
//    USART2->CR1 |= (1 << 3);  // Enable transmitter (TE)
//    USART2->CR1 |= (1 << 5);  // Enable TXE interrupt (Transmit Data Register Empty)
//    USART2->CR1 |= (1 << 13); // Enable USART2 (UE)
//    USART2->CR1 &= ~(1 << 10); // Disable parity (PCE = 0)
//    USART2->CR2 &= ~(0b11 << 12); // Set 1 stop bit (STOP = 0b00)
//
//    // Enable USART2 interrupt and set its priority
//    NVIC_SetPriority(USART2_IRQn, 1); // Priority level 1
//    NVIC_EnableIRQ(USART2_IRQn);      // Enable interrupt for USART2


	// Step 1: Enable USART2 clock and GPIOA clock
	    RCC->APB1ENR |= (1 << 17); // Enable USART2 clock

	    // Step 2: Configure PA2 (USART2_TX) and PA3 (USART2_RX) as Alternate Function
	    GPIOA->MODER &= ~((0b11 << (2 * 2)) | (0b11 << (3 * 2))); // Clear mode bits for PA2 and PA3
	    GPIOA->MODER |= ((0b10 << (2 * 2)) | (0b10 << (3 * 2)));  // Set to Alternate Function mode

	    GPIOA->AFR[0] &= ~((0b1111 << (2 * 4)) | (0b1111 << (3 * 4))); // Clear AF bits for PA2 and PA3
	    GPIOA->AFR[0] |= ((0b0111 << (2 * 4)) | (0b0111 << (3 * 4)));  // Set AF7 (USART2)

	    GPIOA->OTYPER &= ~((1 << 2) | (1 << 3)); // Push-Pull
	    GPIOA->OSPEEDR |= ((0b10 << (2 * 2)) | (0b10 << (3 * 2))); // High speed
	    GPIOA->PUPDR &= ~((0b11 << (2 * 2)) | (0b11 << (3 * 2)));   // Clear pull-up/pull-down
	    GPIOA->PUPDR |= ((0b01 << (2 * 2)) | (0b01 << (3 * 2)));    // Enable pull-up resistors

	    // Step 3: Configure USART2
	    USART2->CR1 = 0; // Reset control register

	    // Configure baud rate (115200, assuming PCLK1 = 84 MHz)
	    USART2->BRR = 0x016C; // BRR = 729 (115200 baud)

	    // Configure word length: 8 bits
	    USART2->CR1 &= ~(1 << 12); // M = 0 (8 data bits)

	    // Configure stop bits: 1 stop bit
	    USART2->CR2 &= ~(0b11 << 12); // STOP = 0b00 (1 stop bit)

	    // Configure parity: None
	    USART2->CR1 &= ~(1 << 10); // PCE = 0 (Parity Control Enable = disabled)

	    // Enable transmitter and receiver
	    USART2->CR1 |= (1 << 3); // TE = 1 (Transmitter Enable)
	    USART2->CR1 |= (1 << 2); // RE = 1 (Receiver Enable)

	    // Enable USART2
	    USART2->CR1 |= (1 << 13); // UE = 1 (USART Enable)
	    USART2->CR1 |= (1 << 5);

	    NVIC_EnableIRQ(USART2_IRQn);
	    NVIC_SetPriority(USART2_IRQn, 1); // Set priority (lower number = higher priority)

}

void UART_Transmit_IT(uint8_t *data, uint16_t size) {
    if (txIndex < txSize) {
        // Previous transmission in progress
        return;
    }

    txBuffer = data; // Set the data buffer
    txSize = size;   // Set the buffer size
    txIndex = 0;     // Reset the index

    // Enable TXE interrupt to start transmission
    USART2->CR1 |= (1 << 7); // TXEIE: Transmit Data Register Empty Interrupt Enable
}

void UART_Transmit_Blocking(uint8_t *data, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        while (!(USART2->SR & (1 << 7))); // Wait for TXE
        USART2->DR = data[i];            // Send data
    }
}



#endif /* INC_USART_H_ */
