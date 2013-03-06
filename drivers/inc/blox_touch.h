/**
 * @file    blox_touch.h
 * @author  Ankita Kaul & Jesse Tannahill
 * @version V0.1
 * @date    11/01/10
 * @brief   Contains function prototypes for the Touchpanel interface.
 *
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

#ifndef __BLOX_TOUCH_H
#define __BLOX_TOUCH_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "blox_filesystem.h"
#include "blox_system.h"
#include "blox_counter.h"
#include "blox_exti.h"

/**
 * @ingroup driver_exti
 * @{
 *
 * @brief A mapping from touch ids to cardinal directions
 */
typedef enum {
  TOUCH_NORTH_ID = 1,
  TOUCH_SOUTH_ID,
  TOUCH_EAST_ID,
  TOUCH_WEST_ID
} TOUCH_DIR;

#define TOUCH_SPI		        SPI1
#define TOUCH_SPI_CLK 		  RCC_APB2Periph_SPI1
#define TOUCH_SPI_GPIO 		  GPIOA
#define TOUCH_SPI_GPIO_CLK	RCC_APB2Periph_GPIOA
//#define TOUCH_SPI_NSS_PIN	  GPIO_Pin_12
#define TOUCH_SPI_SCK_PIN	  GPIO_Pin_5
#define TOUCH_SPI_MISO_PIN	GPIO_Pin_6
#define TOUCH_SPI_MOSI_PIN	GPIO_Pin_7
//#define TOUCH_BUSY_GPIO     GPIOB
//#define TOUCH_BUSY_PIN		  GPIO_Pin_10

#define TOUCH_CS_GPIO        GPIOE
#define TOUCH_CS_GPIO_CLK    RCC_APB2Periph_GPIOE
#define TOUCH3_CS_PIN        GPIO_Pin_12             //TOUCH3_CS on schematic
#define TOUCH1_CS_PIN        GPIO_Pin_8
#define TOUCH2_CS_PIN        GPIO_Pin_13
#define TOUCH4_CS_PIN        GPIO_Pin_9

#define TOUCH_PENIRQ_GPIO    GPIOE
#define TOUCH1_PENIRQ_PIN    GPIO_Pin_10
#define TOUCH2_PENIRQ_PIN    GPIO_Pin_11
#define TOUCH3_PENIRQ_PIN    GPIO_Pin_14
#define TOUCH4_PENIRQ_PIN    GPIO_Pin_7

/**
 * Touchpanel Controller Control Byte
 *   7   - Start Bit
 * [6:4] - Channel Select Bits
 *   3   - 12-Bit/8-Bit Conversion Mode
 *   2   - Single-Ended/Differential
 * [1:0] - Power-Down Select Bits
 */
/// 1101 1011
//#define TOUCH_CTL_X   0xDB
//#define TOUCH_CTL_Y   0x9B
//#define TOUCH_CTL_Z1  0xBB
//#define TOUCH_CTL_Z2  0xCB

//1101 1010
#define TOUCH_CTL_X   0xDA
#define TOUCH_CTL_Y   0x9A
#define TOUCH_CTL_Z1  0xBA
#define TOUCH_CTL_Z2  0xCA

void Blox_Touch_Init(void);
void Blox_Touch_SPI_Send(uint16_t data);
uint16_t Blox_Touch_GetX(int numTouch);
uint16_t Blox_Touch_GetY(int numTouch);
uint16_t Blox_Touch_GetZ1(int numTouch);
uint16_t Blox_Touch_GetZ2(int numTouch);
void Touch_SPI_Send(uint16_t data);
uint16_t Touch_SPI_Receive(void); 
/** @} */ 
#endif
