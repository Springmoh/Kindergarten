#include "stm32f4xx.h"  // Include the device header file for register definitions
#include "adapter.h"
#include "Colour_Sensor.h"
#include "Delay.h"
#include "i2c.h"
#include "RCC_Config.h"
#include "system.h"



uint32_t count;

int main(void) {

	SysClockConfig();
	Pin_config();
	Configure_Timer2();
	Configure_TIM1_PWM();
	TIM9Config();
	I2C1_Config();
	tcs3272_init();

	while (1) {
		SegUpdate();
		getRGB(&red, &green, &blue);
		if(PB1){
			Motor(LEFT_MOTOR, 0);
			Motor(RIGHT_MOTOR, 0);
		}
		if(PB2){
			Motor(LEFT_MOTOR, 10000);
			Motor(RIGHT_MOTOR, 10000);
		}
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
