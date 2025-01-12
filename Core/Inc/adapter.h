/*
 * adapter.h
 *
 *  Created on: Nov 19, 2024
 *      Author: mohnc
 */

#ifndef INC_ADAPTER_H_
#define INC_ADAPTER_H_

#include<stdio.h>
#include<stdlib.h>

typedef struct {
	union {
		uint32_t Word;
		struct {
			unsigned bit0 :1;
			unsigned bit1 :1;
			unsigned bit2 :1;
			unsigned bit3 :1;
			unsigned bit4 :1;
			unsigned bit5 :1;
			unsigned bit6 :1;
			unsigned bit7 :1;
			unsigned bit8 :1;
			unsigned bit9 :1;
			unsigned bit10 :1;
			unsigned bit11 :1;
			unsigned bit12 :1;
			unsigned bit13 :1;
			unsigned bit14 :1;
			unsigned bit15 :1;
			unsigned bit16 :1;
			unsigned bit17 :1;
			unsigned bit18 :1;
			unsigned bit19 :1;
			unsigned bit20 :1;
			unsigned bit21 :1;
			unsigned bit22 :1;
			unsigned bit23 :1;
			unsigned bit24 :1;
			unsigned bit25 :1;
			unsigned bit26 :1;
			unsigned bit27 :1;
			unsigned bit28 :1;
			unsigned bit29 :1;
			unsigned bit30 :1;
			unsigned bit31 :1;
		};
	};
} word_t;

union SegmentData {
	uint16_t bit;
	uint8_t Seg[2];
};

union SegmentData SegData;

typedef enum{
	LEFT_MOTOR,
	RIGHT_MOTOR,
}motor_e;

typedef enum{
	SQUARE_1 = 1,
	SQUARE_2,
	SQUARE_3,
	SQUARE_4,
	SQUARE_5,
	SQUARE_6,
}number_e;

number_e Square;

#define GPIOA_OUT			((word_t*) &GPIOA->ODR)
#define GPIOB_OUT			((word_t*) &GPIOB->ODR)
#define GPIOC_OUT			((word_t*) &GPIOC->ODR)
#define GPIOD_OUT			((word_t*) &GPIOD->ODR)
#define GPIOE_OUT			((word_t*) &GPIOE->ODR)
#define GPIOF_OUT			((word_t*) &GPIOF->ODR)
#define GPIOG_OUT			((word_t*) &GPIOG->ODR)

#define GPIOA_IN			((word_t*) &GPIOA->IDR)
#define GPIOB_IN			((word_t*) &GPIOB->IDR)
#define GPIOC_IN			((word_t*) &GPIOC->IDR)
#define GPIOD_IN			((word_t*) &GPIOD->IDR)
#define GPIOE_IN			((word_t*) &GPIOE->IDR)
#define GPIOF_IN			((word_t*) &GPIOF->IDR)
#define GPIOG_IN			((word_t*) &GPIOG->IDR)

#define led1		GPIOC_OUT->bit13
#define SRlatch		GPIOB_OUT->bit4
#define SRclock		GPIOB_OUT->bit5
#define SRdata		GPIOB_OUT->bit3
#define M_IN1		GPIOB_OUT->bit15
#define M_IN2		GPIOA_OUT->bit8
#define M_IN3		GPIOA_OUT->bit9
#define M_IN4		GPIOB_OUT->bit14
#define M_PWM12		TIM1->CCR3
#define M_PWM34		TIM1->CCR4
#define PB1			GPIOB_IN->bit2
#define PB2			GPIOB_IN->bit10
#define sensorL 	GPIOC_IN->bit15
#define sensorR 	GPIOC_IN->bit14

#define RED_THRESHOLD	90
#define LEFT_F_SPEED	11000
#define RIGHT_F_SPEED	10000
uint8_t Seg1Value, Seg2Value;


void SegUpdate() {
	switch (Seg1Value) {
	case 0:
		SegData.Seg[0] = 0b01111110;
		break;
	case 1:
		SegData.Seg[0] = 0b01100000;
		break;
	case 2:
		SegData.Seg[0] = 0b10110110;
		break;
	case 3:
		SegData.Seg[0] = 0b11110100;
		break;
	case 4:
		SegData.Seg[0] = 0b11101000;
		break;
	case 5:
		SegData.Seg[0] = 0b11011100;
		break;
	case 6:
		SegData.Seg[0] = 0b11011110;
		break;
	case 7:
		SegData.Seg[0] = 0b01110000;
		break;
	case 8:
		SegData.Seg[0] = 0b11111110;
		break;
	case 9:
		SegData.Seg[0] = 0b11111100;
		break;
	default:
		SegData.Seg[0] = 0b00000000;
		break;
	}

	switch (Seg2Value) {
	case 0:
		SegData.Seg[1] = 0b11101110;
		break;
	case 1:
		SegData.Seg[1] = 0b00101000;
		break;
	case 2:
		SegData.Seg[1] = 0b11011100;
		break;
	case 3:
		SegData.Seg[1] = 0b01111100;
		break;
	case 4:
		SegData.Seg[1] = 0b00111010;
		break;
	case 5:
		SegData.Seg[1] = 0b01110110;
		break;
	case 6:
		SegData.Seg[1] = 0b11110110;
		break;
	case 7:
		SegData.Seg[1] = 0b00101100;
		break;
	case 8:
		SegData.Seg[1] = 0b11111110;
		break;
	case 9:
		SegData.Seg[1] = 0b01111110;
		break;
	default:
		SegData.Seg[1] = 0b00000000;
		break;
	}

	for (int i = 15; i >= 0; i--) {
		uint16_t bit = (SegData.bit >> i) & 0x01;
		if (bit) {
			SRdata = 1;
		} else {
			SRdata = 0;
		}
		SRclock = 1;
//			Delay_ms(2);
		SRclock = 0;
	}
	SRlatch = 1;
//		Delay_ms(2);
	SRlatch = 0;
}

void Motor(uint8_t number, int pwm){
	switch(number){
	case RIGHT_MOTOR:
		if (pwm >= 0) {
			M_IN1 = 1;
			M_IN2 = 0;
		} else {
			M_IN1 = 0;
			M_IN2 = 1;
		}
		M_PWM12 = abs(pwm);
		break;

	case LEFT_MOTOR:
		if (pwm >= 0) {
			M_IN3 = 0;
			M_IN4 = 1;
		} else {
			M_IN3 = 1;
			M_IN4 = 0;
		}
		M_PWM34 = abs(pwm);
		break;
	}
}

#endif /* INC_ADAPTER_H_ */
