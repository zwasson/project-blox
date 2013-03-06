/**
 * @file    blox_accel.h
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/20/2010
 * @brief   Driver for Blox Accelerometer
 * Copyright (C) 2010 by Project Blox
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __BLOX_ACCEL_H
#define __BLOX_ACCEL_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

/**
 * @ingroup driver_accel
 * @{
 */

#define ACCEL_GPIO 		        GPIOC
#define ACCEL_GPIO_CLK 	      RCC_APB2Periph_GPIOC

#define ACCEL_ADC1_CLK        RCC_APB2Periph_ADC1
#define ACCEL_DMA_CLK         RCC_AHBPeriph_DMA1

#define ACCEL_XOUT_PIN			  GPIO_Pin_0
#define ACCEL_XOUT_PIN_NUM	  0

#define ACCEL_YOUT_PIN			  GPIO_Pin_1
#define ACCEL_YOUT_PIN_NUM	  1

#define ACCEL_ZOUT_PIN			  GPIO_Pin_2
#define ACCEL_ZOUT_PIN_NUM	  2

#define ACCEL_SLEEP_GPIO 		  GPIOA
#define ACCEL_SLEEP_GPIO_CLK 	RCC_APB2Periph_GPIOA
#define ACCEL_SLEEP_PIN			  GPIO_Pin_11
#define ACCEL_SLEEP_PIN_NUM		11

void Blox_Accel_Init(void);
uint16_t Blox_Accel_GetX(void);
uint16_t Blox_Accel_GetY(void);
uint16_t Blox_Accel_GetZ(void);

uint8_t Blox_Accel_GetXTilt(void);
uint8_t Blox_Accel_GetYTilt(void);
uint8_t Blox_Accel_GetZTilt(void);
/** @} */
#endif
