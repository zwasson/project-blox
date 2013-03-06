/**
 * @file    blox_tim.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   Contains function prototypes for the timers.
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

#ifndef __BLOX_TIM_H
#define __BLOX_TIM_H

#include "blox_system.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"

/**
 * @ingroup driver_tim
 * @{
 */
#define TIM1_CLK    RCC_APB2Periph_TIM1
#define TIM2_CLK    RCC_APB1Periph_TIM2
#define TIM3_CLK    RCC_APB1Periph_TIM3
#define TIM4_CLK    RCC_APB1Periph_TIM4
#define TIM5_CLK    RCC_APB1Periph_TIM5
#define TIM6_CLK    RCC_APB1Periph_TIM6
#define TIM7_CLK    RCC_APB1Periph_TIM7
#define TIM8_CLK    RCC_APB2Periph_TIM8

/**
 * @brief Enum for possible Timer ids
 */
typedef enum {
  INVALID_TIMER = -2,
  IRQ_UNAVAILABLE,
  TIM1UP,
  TIM1CH1,
  TIM1CH2,
  TIM1CH3,
  TIM1CH4,
  TIM2CH1,
  TIM2CH2,
  TIM2CH3,
  TIM2CH4,
  TIM3CH1,
  TIM3CH2,
  TIM3CH3,
  TIM3CH4,
  TIM4CH1,
  TIM4CH2,
  TIM4CH3,
  TIM4CH4,
  TIM5CH1,
  TIM5CH2,
  TIM5CH3,
  TIM5CH4,
  TIM6UP,
  TIM7UP,
  TIM8UP,
  TIM8CH1,
  TIM8CH2,
  TIM8CH3,
  TIM8CH4
} TIMER_ID;

void Blox_Timer_Init(uint8_t TIMx, uint32_t TIM_CLK);
TIMER_ID Blox_Timer_Register_IRQ(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState);
void Blox_Timer_Release_IRQ(TIMER_ID id);
void Blox_Timer_Modify_IRQ(TIMER_ID id, uint16_t period);
void Blox_Timer_Enable_IRQ(TIMER_ID id);
void Blox_Timer_Disable_IRQ(TIMER_ID id);
/** @} */
#endif
