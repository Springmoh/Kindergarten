#include "stm32f4xx.h"  // Include the device header file for register definitions
#include "adapter.h"
#include "Colour_Sensor.h"
#include "Delay.h"
#include "i2c.h"
#include "RCC_Config.h"
#include "system.h"
#include "usart.h"
#include "string.h"


uint32_t tick;
char rxBuf[50];
int x;

int main(void) {
	uint32_t timer = 0;

	SysClockConfig();
	Pin_config();
	Configure_TIM2();
	Configure_TIM3();
	Configure_TIM1_PWM();
	TIM9Config();
	I2C1_Config();
	tcs3272_init();
	Configure_USART2();

	while (1) {
		SegUpdate();
		if (tick - timer > 50) {
			getRGB(&red, &green, &blue);
			timer = tick;
		}

		if (PB1) {
			x += 1000;
		}

		if (PB2) {
			x -= 1000;
		}

		sprintf(rxBuf, "Red: %d  Green:%d  Blue:%d\n", red, green, blue);
		UART_Transmit_IT((uint8_t*) rxBuf, strlen(rxBuf));
//		UART_Transmit_Blocking((uint8_t*)"Hello World\n", strlen("Hello World\n"));

		Motor(LEFT_MOTOR, x);
		Motor(RIGHT_MOTOR, -x);

	}
}

void TIM2_IRQHandler(void) { // Timer Interrupt
    if (TIM2->SR & TIM_SR_UIF) { // Check update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt flag

        led1 = !led1;
        static int cnt;
		if (++cnt > 50) {
			if (Seg1Value < 9) {
				Seg1Value++;
				Seg2Value++;
			} else {
				Seg1Value = 0;
				Seg2Value = 0;
			}
			cnt = 0;
		}
    }
}

void USART2_IRQHandler(void) {
    if (USART2->SR & (1 << 7)) { // Check if TXE is set
        if (txIndex < txSize) {
            USART2->DR = txBuffer[txIndex++]; // Send next byte
        } else {
            USART2->CR1 &= ~(1 << 7); // Disable TXE interrupt
        }
    }
}


void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) { // Check for Update Interrupt Flag (UIF)
        TIM3->SR &= ~TIM_SR_UIF; // Clear interrupt flag

        tick++;
    }
}
