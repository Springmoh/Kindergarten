/*
 * Colour_Sensor.h
 *
 *  Created on: Dec 20, 2024
 *      Author: OSCAR
 */

#ifndef INC_COLOUR_SENSOR_H_
#define INC_COLOUR_SENSOR_H_

#include "stdint.h"
#include "stm32f4xx.h"
#include "i2c.h"
#include "RCC_Config.h"
#include "Delay.h"
#include "adapter.h"

#define TCS34725_ADDRESS          (0x29 << 1) /* I2C address */
/* Datasheet is at https://cdn-shop.adafruit.com/datasheets/TCS34725.pdf */
#define TCS34725_COMMAND_BIT      (0x80)      /* Command bit */
#define TCS34725_ENABLE           (0x00)      /* Enable register */
#define TCS34725_ENABLE_AEN       (0x02)      /* RGBC Enable */
#define TCS34725_ENABLE_PON       (0x01)      /* Power on */
#define TCS34725_ATIME            (0x01)      /* Integration time */
#define TCS34725_CONTROL          (0x0F)      /* Set the gain level */
#define TCS34725_ID               (0x12)
/* 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 */
#define TCS34725_CDATAL           (0x14)      /* Clear channel data */
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATAL           (0x16)      /* Red channel data */
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATAL           (0x18)      /* Green channel data */
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATAL           (0x1A)      /* Blue channel data */
#define TCS34725_BDATAH           (0x1B)
/* set the correct delay time in void getRawData() for TCS34725_INTEGRATIONTIME */
#define TCS34725_INTEGRATIONTIME_50MS   0xEB  /* 50ms  - 20 cycles */
#define TCS34725_GAIN_4X                0x01  /* 4x gain  */

uint8_t _tcs34725Initialised = 0;
int red, green, blue;
uint8_t readValue;

void write8 (uint8_t reg, uint32_t value);
uint8_t read8(uint8_t reg);
uint16_t read16(uint8_t reg);
void enable(void);
void disable(void);
void setIntegrationTime(uint8_t it);
void setGain(uint8_t gain);
void tcs3272_init( void );
void getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);
void getRGB(int *R, int *G, int *B);


/* Writes a register and an 8 bit value over I2C */
void write8 (uint8_t reg, uint32_t value)
{
    uint8_t txBuffer[2];
    txBuffer[0] = (TCS34725_COMMAND_BIT | reg);
    txBuffer[1] = (value & 0xFF);
	I2C1_Start();
	I2C1_Address(TCS34725_ADDRESS);
	I2C1_Write(txBuffer[0]);
	I2C1_Write(txBuffer[1]);
	I2C1_Stop();
//    HAL_I2C_Master_Transmit(&hi2c1, TCS34725_ADDRESS, txBuffer, 2, 100);
}

/* Reads an 8 bit value over I2C */
uint8_t read8(uint8_t reg) {
    uint8_t txBuffer = (TCS34725_COMMAND_BIT | reg);
    uint8_t rxBuffer[1];

    I2C1_Start();                                // Generate start condition
    I2C1_Address(TCS34725_ADDRESS);             // Send sensor address
    I2C1_Write(txBuffer);                       // Write register address
    I2C1_Stop();                                // Generate stop condition

    I2C1_Start();                                // Restart for read
    I2C1_Read(TCS34725_ADDRESS | 0x01, rxBuffer, 1); // Read 1 byte
    return rxBuffer[0];
}


/* Reads a 16 bit values over I2C */
uint16_t read16(uint8_t reg) {
    uint8_t txBuffer = (TCS34725_COMMAND_BIT | reg);
    uint8_t rxBuffer[2];

    I2C1_Start();                                // Generate start condition
    I2C1_Address(TCS34725_ADDRESS);             // Send sensor address
    I2C1_Write(txBuffer);                       // Write register address
    I2C1_Stop();                                // Generate stop condition

    I2C1_Start();                                // Restart for read
    I2C1_Read(TCS34725_ADDRESS | 0x01, rxBuffer, 2); // Read 2 bytes

    uint16_t ret = (rxBuffer[1] << 8) | rxBuffer[0];
    return ret;
}


void enable(void)
{
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  Delay_ms (3);
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
  Delay_ms (50);
}

void disable(void)
{
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = read8(TCS34725_ENABLE);
  write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

void setIntegrationTime(uint8_t itime)
{
  if (_tcs34725Initialised == 0) tcs3272_init();
  write8(TCS34725_ATIME, itime);
}

void setGain(uint8_t gain)
{
  if (_tcs34725Initialised == 0) tcs3272_init();
  write8(TCS34725_CONTROL, gain);
}

void tcs3272_init(void)
{
  /* Make sure we're actually connected */
  readValue = read8(TCS34725_ID);
  if ((readValue != 0x44) && (readValue != 0x4d))
  {

    return;
  }
  _tcs34725Initialised = 1;
  /* Set default integration time and gain */
  setIntegrationTime(TCS34725_INTEGRATIONTIME_50MS);
  setGain(TCS34725_GAIN_4X);
  /* Note: by default, the device is in power down mode on bootup */
  enable();
}

/* Get raw data */
void getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  if (_tcs34725Initialised == 0) tcs3272_init();

  *c = read16(TCS34725_CDATAL);
  *r = read16(TCS34725_RDATAL);
  *g = read16(TCS34725_GDATAL);
  *b = read16(TCS34725_BDATAL);
  /* Delay time is from page no 16/26 from the datasheet  (256 − ATIME)* 2.4ms */
//  Delay_ms (50); // Set delay for (256 − 0xEB)* 2.4ms = 50ms
}

/* Get Red, Green and Blue color from Raw Data */
void getRGB(int *R, int *G, int *B)
{
    uint16_t rawRed, rawGreen, rawBlue, rawClear;
    getRawData(&rawRed, &rawGreen, &rawBlue, &rawClear);
    if(rawClear == 0)
    {
      *R = 0;
      *G = 0;
      *B = 0;
    }
    else
    {
      *R = (int)rawRed * 255 / rawClear;
      *G = (int)rawGreen * 255 / rawClear;
      *B = (int)rawBlue * 255 / rawClear;
    }
}



#endif /* INC_COLOUR_SENSOR_H_ */
