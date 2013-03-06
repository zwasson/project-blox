/**
 * @file    blox_led.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/30/2010
 * @brief   Contains function prototypes for the LEDs.
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

#ifndef __BLOX_LED_H
#define __BLOX_LED_H

#include "blox_system.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/**
 * @ingroup driver_led
 * @{
 */
#define LED_GPIO        GPIOC
#define LED_CLK         RCC_APB2Periph_GPIOC
#define LED1_GPIO_Pin   GPIO_Pin_6
#define LED2_GPIO_Pin   GPIO_Pin_7
#define LED3_GPIO_Pin   GPIO_Pin_8
#define LED4_GPIO_Pin   GPIO_Pin_9

/**
 * @brief A mapping from LED ID to cardinal directions.
 */
typedef enum {
  LED1 = 1,
  LED2,
  LED3,
  LED4
} LED_ID;

#define LED_EAST  LED1
#define LED_SOUTH LED2
#define LED_WEST  LED3
#define LED_NORTH LED4

void Blox_LED_Init(void);
void Blox_LED_On(LED_ID id);
void Blox_LED_Off(LED_ID id);
void Blox_LED_Toggle(LED_ID id);
/** @} */
#endif
