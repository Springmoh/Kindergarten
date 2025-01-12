/*
 * imu.h
 *
 *  Created on: Jan 12, 2025
 *      Author: mohnc
 */

#ifndef INC_IMU_H_
#define INC_IMU_H_

#include "imu.h"

// Global variables for the interrupt-driven receive mechanism
uint8_t* rxBuffer = NULL;
volatile uint16_t rxSize = 0;
volatile uint16_t rxIndex = 0;
uint8_t firstread = 0;

typedef enum {
    PENDING_SYNC,
    CONFIRMING_SYNC,
    IN_SYNC
} IMU_State_t;

typedef struct {

	USART_TypeDef* usartx; // USART instance
	uint8_t Buffer[20];
	IMU_State_t State;
	volatile float yaw;
	volatile float yaw_rate;
	volatile float x_acc;
	volatile float y_acc;
	volatile float z_acc;
	volatile int8_t turn_no;
	volatile uint8_t checksum;
	float prev_yaw;
	float yaw_constant;
	float real_z;
	float real_zrad;
	float offset;
}R6091U_t;

R6091U_t imu;


void UART_Receive_IT(USART_TypeDef* USARTx, uint16_t Size) {
    rxSize = Size;     // Set the size
    rxIndex = 0;       // Reset the index

    // Enable RXNE interrupt
    USARTx->CR1 |= (1 << 5); // RXNEIE: RX Not Empty Interrupt Enable
}

void IMU_Handler(R6091U_t* IMU) {

    switch (IMU->State) {
    case PENDING_SYNC:
        if (IMU->Buffer[0] == 0xAA) {
            IMU->State = CONFIRMING_SYNC;
            UART_Receive_IT(USART2, 1);
        } else {
            IMU->State = PENDING_SYNC;
            UART_Receive_IT(IMU->usartx, 1);
        }
        break;

    case CONFIRMING_SYNC:
        if (IMU->Buffer[0] == 0x00) {
            IMU->State = IN_SYNC;
            UART_Receive_IT(IMU->usartx, 13);
        } else {
            IMU->State = PENDING_SYNC;
            UART_Receive_IT(IMU->usartx, 1);
        }
        break;

    case IN_SYNC:
        IMU->checksum = 0;
        for (uint8_t i = 0; i < 12; i++) {
            IMU->checksum += IMU->Buffer[i];
        }

        if (IMU->checksum == IMU->Buffer[12]) {
            IMU->yaw = *((int16_t*)&IMU->Buffer[1]) / 100.0;
            IMU->yaw_rate = *((int16_t*)&IMU->Buffer[3]) / 100.0;
            IMU->x_acc = *((int16_t*)&IMU->Buffer[5]) / 1000.0 * 9.8067;
            IMU->y_acc = *((int16_t*)&IMU->Buffer[7]) / 1000.0 * 9.8067;
            IMU->z_acc = *((int16_t*)&IMU->Buffer[9]) / 1000.0 * 9.8067;
            IMU->turn_no = IMU->Buffer[11];
        }

        if(firstread == 0){
        	imu.offset = 180.0 - IMU->yaw;
        	firstread = 1;
        }

        IMU->State = PENDING_SYNC;
        UART_Receive_IT(IMU->usartx, 1);
        break;
    }
}


#endif /* INC_IMU_H_ */
