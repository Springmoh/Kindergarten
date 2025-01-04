/*
 * i2c.h
 *
 *  Created on: Dec 17, 2024
 *      Author: OSCAR
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f4xx.h"
#include "RCC_Config.h"

void I2C1_Config(void);
void I2C1_Start(void);
void I2C1_Write(uint8_t data);
void I2C1_Address(uint8_t Address);
void I2C1_Stop(void);
void I2C1_Read(uint8_t Address, uint8_t *buffer, uint8_t size);

void I2C1_Config(void) {

	/* 1. Enable the I2C CLOCK and GPIO CLOCK */
	RCC->APB1ENR |= (1 << 21); // ENABLE I2C1 Clock
	RCC->AHB1ENR |= (1 << 1); // ENABLE GPIOB Clock (SCL: PB6 SDA: PB7)

	/* 2. Configure the I2C PINs for ALternate Functions */
	GPIOB->MODER |= (2 << 12) | (2 << 14);// Alternate Function of PB6 & PB7 in MODER Register
	GPIOB->OTYPER |= (1 << 6) | (1 << 7);	//Open Drain Output of PB6 & PB7
	GPIOB->OSPEEDR |= (3 << 12) | (3 << 14);//High SPEED for the Pins PB6 & PB7
	GPIOB->PUPDR |= (1 << 12) | (1 << 14);//Pull-up for both the Pins PB6 & PB7
	GPIOB->AFR[0] |= (4 << 24) | (4 << 28);	//Configure the Alternate Function in AFR Register

	/* 3. Reset the I2C */
	I2C1->CR1 |= (1 << 15);
	I2C1->CR1 &= ~(1 << 15);

	/* 4. Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings */
	I2C1->CR2 |= (42 << 0); // PCLK1 FREQUENCY in MHz

	/* 5. Configure the clock control registers */
	I2C1->CCR = 210 << 0;

	/* 6. Configure the rise time register */
	I2C1->TRISE = 43;

	/* 7. Program the I2C_CR1 register to enable the peripheral */
	I2C1->CR1 |= (1 << 0); // Enable I2C
}

void I2C1_Start(void) {
	/* 1. Enable the ACK */
	I2C1->CR1 |= (1 << 10);  // Enable the ACK

	/* 2. Send the START condition */
	I2C1->CR1 |= (1 << 8);  // Generate START

	/* 3. Wait for the SB ( Bit 0 in SR1) to set means the start condition is generated */
	while (!(I2C1->SR1 & (1 << 0)))
		;
}

void I2C1_Write(uint8_t data) {
	/* 1. Wait for the TXE (bit 7 in SR1) to set. This indicates that the DR is empty */
	while (!(I2C1->SR1 & (1 << 7)))
		;
	/* 2. Send the DATA to the DR Register */
	I2C1->DR = data;
	/* 3. Wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission */
	while (!(I2C1->SR1 & (1 << 2)))
		;
}

void I2C1_Address(uint8_t Address) {
	/* 1. Send the Slave Address to the DR Register */
	I2C1->DR = Address;
	/* 2. Wait for the ADDR (bit 1 in SR1) to set. This indicates the end of address transmission */
	while (!(I2C1->SR1 & (1 << 1)));
	/* 3. clear the ADDR by reading the SR1 and SR2 */
	uint8_t temp = I2C1->SR1 | I2C1->SR2;
}

void I2C1_Stop(void) {
	I2C1->CR1 |= (1 << 9); // Stop I2C
}

void I2C1_Read(uint8_t Address, uint8_t *buffer, uint8_t size) {
	/**** STEPS FOLLOWED  ************
	 1. If only 1 BYTE needs to be Read
	 a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	 b) the Acknowledge disable is made during EV6 (before ADDR flag is cleared) and the STOP condition generation is made after EV6
	 c) Wait for the RXNE (Receive Buffer not Empty) bit to set
	 d) Read the data from the DR

	 2. If Multiple BYTES needs to be read
	 a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	 b) Clear the ADDR bit by reading the SR1 and SR2 Registers
	 c) Wait for the RXNE (Receive buffer not empty) bit to set
	 d) Read the data from the DR
	 e) Generate the Acknowlegment by settint the ACK (bit 10 in SR1)
	 f) To generate the nonacknowledge pulse after the last received data byte, the ACK bit must be cleared just after reading the
	 second last data byte (after second last RxNE event)
	 g) In order to generate the Stop/Restart condition, software must set the STOP/START bit
	 after reading the second last data byte (after the second last RxNE event)
	 */
	int remaining = size;

	/**** STEP 1 ****/
	if (size == 1) {
		/**** STEP 1-a ****/
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1 << 1)))
			;  // wait for ADDR bit to set

		/**** STEP 1-b ****/
		I2C1->CR1 &= ~(1 << 10);  // clear the ACK bit
		uint8_t temp = I2C1->SR1 | I2C1->SR2; // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1->CR1 |= (1 << 9);  // Stop I2C

		/**** STEP 1-c ****/
		while (!(I2C1->SR1 & (1 << 6)))
			;  // wait for RxNE to set

		/**** STEP 1-d ****/
		buffer[size - remaining] = I2C1->DR; // Read the data from the DATA REGISTER

	}

	/**** STEP 2 ****/
	else {
		/**** STEP 2-a ****/
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1 << 1)))
			;  // wait for ADDR bit to set

		/**** STEP 2-b ****/
		uint8_t temp = I2C1->SR1 | I2C1->SR2; // read SR1 and SR2 to clear the ADDR bit

		while (remaining > 2) {
			/**** STEP 2-c ****/
			while (!(I2C1->SR1 & (1 << 6)))
				;  // wait for RxNE to set

			/**** STEP 2-d ****/
			buffer[size - remaining] = I2C1->DR; // copy the data into the buffer

			/**** STEP 2-e ****/
			I2C1->CR1 |= 1 << 10; // Set the ACK bit to Acknowledge the data received

			remaining--;
		}

		// Read the SECOND LAST BYTE
		while (!(I2C1->SR1 & (1 << 6)))
			;  // wait for RxNE to set
		buffer[size - remaining] = I2C1->DR;

		/**** STEP 2-f ****/
		I2C1->CR1 &= ~(1 << 10);  // clear the ACK bit

		/**** STEP 2-g ****/
		I2C1->CR1 |= (1 << 9);  // Stop I2C

		remaining--;

		// Read the Last BYTE
		while (!(I2C1->SR1 & (1 << 6)))
			;  // wait for RxNE to set
		buffer[size - remaining] = I2C1->DR;  // copy the data into the buffer
	}

}

#endif /* INC_I2C_H_ */
