/**
 * @file    blox_exti.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   Contains function prototypes for the EXTI interface.
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

#ifndef __BLOX_EXTI_H
#define __BLOX_EXTI_H

#include "blox_system.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

/**
 * @ingroup driver_exti
 * @{
 *
 * @brief Enum for possible EXTI ids
 */
typedef enum {
  EXTI_INVALID_LINE = -2,
  EXTI_IRQ_UNAVAILABLE,
  EXTI0,
  EXTI1,
  EXTI2,
  EXTI3,
  EXTI4,
  EXTI5,
  EXTI6,
  EXTI7,
  EXTI8,
  EXTI9,
  EXTI10,
  EXTI11,
  EXTI12,
  EXTI13,
  EXTI14,
  EXTI15
} EXTI_ID;

void Blox_EXTI_Init(void);
EXTI_ID Blox_EXTI_Register_HW_IRQ(uint8_t GPIO_PortSource, uint8_t line, void (*EXTI_Handler)(void));
EXTI_ID Blox_EXTI_Register_SW_IRQ(void (*EXTI_Handler)(void));
void Blox_EXTI_Release_IRQ(EXTI_ID id);
void Blox_EXTI_Trigger_SW_IRQ(EXTI_ID id);
void Blox_EXTI_Enable_IRQ(EXTI_ID id);
void Blox_EXTI_Disable_IRQ(EXTI_ID id);
/** @} */
#endif
