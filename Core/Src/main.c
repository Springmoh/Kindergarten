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
uint8_t redcnt;
uint8_t runflag = 0;

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

	Seg1Value = 0;
	Seg2Value = 0;
	linedetected_delay = tick;

	while (1) {

		if (tick - timer > 50) {
			getRGB(&red, &green, &blue);
			timer = tick;
		}
//
		if (PB1) {
			runflag = !runflag;
			while (PB1);
			linedetected_delay = tick;
		}

		if (runflag) {

			if (tick - linedetected_delay < 2020) {
				Motor(LEFT_MOTOR, 10500);
				Motor(RIGHT_MOTOR, 10000);

			} else if (tick - linedetected_delay < 2520) {
				Motor(LEFT_MOTOR, 0);
				Motor(RIGHT_MOTOR, 0);


			} else if (tick - linedetected_delay < 2840) {

				Motor(LEFT_MOTOR, -10500);
				Motor(RIGHT_MOTOR, 10000);

			} else if (tick - linedetected_delay < 3270) {
				Motor(LEFT_MOTOR, 0);
				Motor(RIGHT_MOTOR, 0);
//
			} else if (tick - linedetected_delay < 4020) {
				Motor(LEFT_MOTOR, 11500);
				Motor(RIGHT_MOTOR, 11000);

			} else if (tick - linedetected_delay < 4520) {
				Motor(LEFT_MOTOR, 0);
				Motor(RIGHT_MOTOR, 0);

			} else if (tick - linedetected_delay < 4840) {
				Motor(LEFT_MOTOR, -10500);
				Motor(RIGHT_MOTOR, 10000);
//
			} else if (tick - linedetected_delay < 5470) {
				Motor(LEFT_MOTOR, 0);
				Motor(RIGHT_MOTOR, 0);
//////
			} else if (tick - linedetected_delay < 7470) {
				Motor(LEFT_MOTOR, 10500);
				Motor(RIGHT_MOTOR, 10000);

			} else {
				Motor(LEFT_MOTOR, 0);
				Motor(RIGHT_MOTOR, 0);
				runflag = 0;
			}
		}




	}
}

void TIM2_IRQHandler(void) { // Timer Interrupt
	if (TIM2->SR & TIM_SR_UIF) { // Check update interrupt flag
		TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt flag

		Seg1Value = Square;
		SegUpdate();
		if (red > 100) {
			redcnt++;
			if (redcnt == 1) {
				Seg2Value++;
			}
		} else {
			redcnt = 0;
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
		static uint8_t ledcnt = 0;
		if(++ledcnt > 20){
			led1 = !led1;
			ledcnt = 0;
		}
		tick++;
	}
}
