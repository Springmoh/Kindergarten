#include "stm32f4xx.h"  // Include the device header file for register definitions
#include "string.h"
#include "math.h"

#include "adapter.h"
#include "Colour_Sensor.h"
#include "Delay.h"
#include "i2c.h"
#include "RCC_Config.h"
#include "system.h"
#include "usart.h"
#include "imu.h"
#include "pid.h"

#define shifted

uint32_t tick, delay, linedetected_delay;
char rxBuf[50];
uint8_t redcnt;
uint8_t runflag = 0;
float currentAngle, previousAngle, AngleNow, AngleTarget;
int turn = 0;
PID_t imupid;
float imupidout;
float leftspeed, rightspeed;
float Lfinalspeed, Rfinalspeed;
uint8_t state, startup, segenable, reportflag, seg2buf;
int cnt;
uint8_t receivedData;

unsigned square[6];

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


	PID_Init(&imupid, 300, 0, 1);

	Seg1Value = 0;
	Seg2Value = 0;
	linedetected_delay = tick;

	UART_Receive_IT(USART2, 1);

	while (1) {
		if (tick - timer > 50) {
			getRGB(&red, &green, &blue);
			timer = tick;
		}

		currentAngle = 180.0 - imu.yaw;

		if (currentAngle - previousAngle < -300) {
			turn++;
		}else if(currentAngle - previousAngle > 300){
			turn--;
		}

		AngleNow = (float) turn * 360.0 + currentAngle - imu.offset;

		previousAngle = currentAngle;

		if (PB1) {
			runflag = !runflag;
			while (PB1);
			AngleTarget = AngleNow;
			delay = tick;
			linedetected_delay = tick;
			state = 0;
			segenable = 1;

			Seg1Value = 0;
			Seg2Value = 0;
			reportflag = 0;
		}


		imupidout = PID_Compute(&imupid, AngleTarget, AngleNow);


		switch(state){
		case 0:
			leftspeed =  10000;
			rightspeed = 10000;
			if(tick - delay > 2000){
				state = 1;
				delay = tick;
				leftspeed =  0;
				rightspeed = 0;
#ifdef shifted
				AngleTarget -= 90;
#else
				AngleTarget += 90;
#endif
				imupid.Kp = 75;
				imupid.Ki = 0.001;
				imupid.Kd = 3;
			}
			break;

		case 1:
			leftspeed = 0;
			rightspeed = 0;
			if (tick - delay > 1000) {
				state = 2;
				delay = tick;
			}

			break;

		case 2:
			leftspeed = 10000;
			rightspeed = 10000;
			imupid.Kp = 100;
			imupid.Ki = 0;
			imupid.Kd = 2;
			if (tick - delay > 650) {
				state = 3;
#ifdef shifted
				AngleTarget -= 90;
#else
				AngleTarget += 90;
#endif
				delay = tick;
			}
			break;

		case 3:
			leftspeed = 0;
			rightspeed = 0;
			imupid.Kp = 60;
			imupid.Ki = 0.0005;
			imupid.Kd = 2;
			if (tick - delay > 1000) {
				state = 4;
				delay = tick;
			}
			break;

		case 4:
			leftspeed = 10000;
			rightspeed = 10000;
			imupid.Kp = 300;
			imupid.Ki = 0;
			imupid.Kd = 1;
			if (tick - delay > 2100) {
				state = 5;
				delay = tick;
			}
			break;

		default:
			leftspeed = 0;
			rightspeed = 0;
			runflag = 0;
			break;
		}
#ifdef shifted
		Lfinalspeed = leftspeed + imupidout;
		Rfinalspeed = rightspeed - imupidout;
#else
			Lfinalspeed = leftspeed - imupidout;
			Rfinalspeed = rightspeed + imupidout;
#endif

		if (runflag) {
			Motor(LEFT_MOTOR, Lfinalspeed);
			Motor(RIGHT_MOTOR, Rfinalspeed);

		} else {
			Motor(LEFT_MOTOR, 0);
			Motor(RIGHT_MOTOR, 0);
		}


		if (segenable) {
			if (tick - linedetected_delay < 1000) {
				Seg1Value = 1;
			} else if (tick - linedetected_delay < 1800) {
				Seg1Value = 2;
			} else if (tick - linedetected_delay < 3500) {
				Seg1Value = 3;
			} else if (tick - linedetected_delay < 5000) {
				Seg1Value = 4;
			} else if (tick - linedetected_delay < 6000) {
				Seg1Value = 5;
			} else if (tick - linedetected_delay < 7000) {
				Seg1Value = 6;
			} else if (tick - linedetected_delay < 8000) {
				Seg1Value = 10;
				seg2buf = Seg2Value;
				Seg2Value = 10;
				segenable = 0;
			}
		}

		if(PB2){
			reportflag = 1;
			segenable = 1;
			Seg2Value = seg2buf;
		}

	}
}

void TIM2_IRQHandler(void) { // Timer Interrupt
	if (TIM2->SR & TIM_SR_UIF) { // Check update interrupt flag
		TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt flag


		if (AngleNow != 180 && AngleNow !=0) {
			led1 = !led1;
		}else{
			led1 = 0;
		}
		if (red > 120) {
			redcnt++;
			if (redcnt == 1) {
				if (segenable && !reportflag) {
					Seg2Value++;
					switch (Seg1Value) {
					case 1:
						square[0] = 1;
						break;
					case 2:
						square[1] = 1;
						break;
					case 3:
						square[2] = 1;
						break;
					case 4:
						square[3] = 1;
						break;
					case 5:
						square[4] = 1;
						break;
					case 6:
						square[5] = 1;
						break;
					default:
						break;
					}
				}
			}
		} else {
			redcnt = 0;
		}

		SegUpdate();

	}
}



void USART2_IRQHandler(void) {
    // Handle transmit interrupt


	if (USART2->SR & (1 << 7)) { // Check if TXE is set
		if (txIndex < txSize) {
			USART2->DR = txBuffer[txIndex++]; // Send next byte
		} else {
			USART2->CR1 &= ~(1 << 7); // Disable TXE interrupt
		}
	}

	if (USART2->SR & (1 << 5)) { // RXNE: Read Data Register Not Empty
//		led1 = !led1;
		if (rxIndex < rxSize) {
			imu.Buffer[rxIndex] = USART2->DR;
			rxIndex++;
		} else {
			IMU_Handler(&imu);
		}
	}
}

uint8_t reportcnt;

void TIM3_IRQHandler(void) {
	if (TIM3->SR & TIM_SR_UIF) { // Check for Update Interrupt Flag (UIF)
		TIM3->SR &= ~TIM_SR_UIF; // Clear interrupt flag
		static uint16_t cnt = 0;
		if (reportflag) {
			if (++cnt > 500) {
				reportcnt++;
				cnt = 0;
			}

			if (square[reportcnt] == 1) {
				Seg1Value = reportcnt + 1;
			}

			if (reportcnt == 6) {
				reportcnt = 0;
			}
		}





		tick++;
	}
}
