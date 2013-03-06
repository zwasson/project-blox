/**
 * @file    blox_tim.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   A basic wrapper around the timers on the STM32F103
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

/* TO DO:
  add functionality to warn or block users from using Timer_Init on an initialized timer
  check if range for TIM1 and TIM8 is less than counter value used for ARR
  TIM1 and TIM8 UP IRQ
  add code for ARR in advanced config function
  add priority stuff for NVIC
*/

#include "blox_tim.h"

/**
 * @ingroup driver_timer
 * @{
 */
uint16_t TIM_PSC[8];
void (*TIM_Handler[28])(void);
uint16_t TIM_IRQ_period[28];

void Blox_Timer_DeInit_Timer(void);
void Blox_Timer_RCC_Configuration(uint8_t TIMx);
void Blox_Timer_NVIC_Configuration(uint8_t TIMx);
TIMER_ID Timer_OC_IRQ_Configuration(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState);
TIMER_ID Timer_UP_IRQ_Configuration(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState);

/**
 * @brief Initializes Timer.
 * @param TIMx where x can be (1...8) to select the timer.
 * @param TIM_CLK specifies the TIM_CLK to set for the given timer
 *   where TIM_CLK can be in the range of 1.1kHz to 72MHz
 *   TIM_CLK = SYS_CLK / (PSC + 1)
 * @retval None
 */
void Blox_Timer_Init(uint8_t TIMx, uint32_t TIM_CLK) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_PSC[TIMx-1] = (uint16_t) (SystemCoreClock / TIM_CLK) - 1;
  Blox_Timer_RCC_Configuration(TIMx);
  Blox_Timer_NVIC_Configuration(TIMx);
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = TIM_PSC[TIMx-1];
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  switch(TIMx) {
    case 1:
      TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
      break;
    case 2:
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
      break;
    case 3:
      TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
      break;
    case 4:
      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
      break;
    case 5:
      TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
      break;
    case 6:
      TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
      break;
    case 7:
      TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
      break;
    case 8:
      TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
      break;   
   }
   
   Blox_System_Register_DeInit(&RCC_DeInit);
   Blox_System_Register_DeInit(&Blox_Timer_DeInit_Timer);
}

/**
 * @brief De-initializes the Timers for all the Timer interfaces. 
 * @retval None
 */
void Blox_Timer_DeInit_Timer(void) {
  TIM_DeInit(TIM1);
  TIM_DeInit(TIM2);
  TIM_DeInit(TIM3);
  TIM_DeInit(TIM4);
  TIM_DeInit(TIM5);
  TIM_DeInit(TIM6);
  TIM_DeInit(TIM7);
  TIM_DeInit(TIM8);
}

/**
 * @brief Initializes clocks for the given timer.
 * @param TIMx where x can be (1...8) to select the timer.
 * @retval None
 */
void Blox_Timer_RCC_Configuration(uint8_t TIMx) {
  switch(TIMx) {
    case 1:
      RCC_APB2PeriphClockCmd(TIM1_CLK, ENABLE);
      break;
    case 2:
      RCC_APB1PeriphClockCmd(TIM2_CLK, ENABLE);
      break;
    case 3:
      RCC_APB1PeriphClockCmd(TIM3_CLK, ENABLE);
      break;
    case 4:
      RCC_APB1PeriphClockCmd(TIM4_CLK, ENABLE);
      break;
    case 5:
      RCC_APB1PeriphClockCmd(TIM5_CLK, ENABLE);
      break;
    case 6:
      RCC_APB1PeriphClockCmd(TIM6_CLK, ENABLE);
      break;
    case 7:
      RCC_APB1PeriphClockCmd(TIM7_CLK, ENABLE);
      break;
    case 8:
      RCC_APB2PeriphClockCmd(TIM8_CLK, ENABLE);
      break;
  }
}

/**
 * @brief Initializes interrupts for the given timer.
 * @param TIMx where x can be (1...8) to select the timer.
 * @retval None
 */
void Blox_Timer_NVIC_Configuration(uint8_t TIMx) {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  switch(TIMx) {
    case 1:
      NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
      NVIC_Init(&NVIC_InitStructure);
      NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
      break;
    case 2:
      NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
      break;
    case 3:
      NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
      break;
    case 4:
      NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
      break;
    case 5:
      NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
      break;
    case 6:
      NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
      break;
    case 7:
      NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
      break;
    case 8:
      NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
      NVIC_Init(&NVIC_InitStructure);
      NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
      break;
  }
  NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief Registers a timer interrupt for a given timer.
 * @param TIMx where x can be (1...8) to select the timer.
 * @param period specifies the period between timer interrupts.
 * @param Timer_Handler the handler function for the timer interrupt.
 * @param NewState new state of the timer interrupt. ENABLE or DISABLE
 * @retval the id for the given interrupt or error
 */
TIMER_ID Blox_Timer_Register_IRQ(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState) {  
  TIMER_ID id;
  switch(TIMx) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
      id = Timer_OC_IRQ_Configuration(TIMx, period, Timer_Handler, NewState);
      break;
    case 6:
    case 7:
      id = Timer_UP_IRQ_Configuration(TIMx, period, Timer_Handler, NewState);
      break;
  }
  return id;
}

/**
 * @brief Registers an output compare interrupt for a given timer.
 * @param TIMx where x can be (1,2,3,4,5,8) to select the timer.
 * @param period specifies the period between timer interrupts.
 * @param Timer_Handler the handler function for the timer interrupt.
 * @param NewState new state of the timer interrupt. ENABLE or DISABLE
 * @retval the id for the given interrupt or error
 */
TIMER_ID Timer_OC_IRQ_Configuration(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState) {
  TIMER_ID id;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  switch(TIMx) {
    case 1:
      
      if(TIM_Handler[TIM1CH1] == NULL) {
        id = TIM1CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM1, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM1, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM1CH2] == NULL) {
        id = TIM1CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM1, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM1, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM1CH3] == NULL) {
        id = TIM1CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM1, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM1, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM1CH4] == NULL) {
        id = TIM1CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM1, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM1, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM1, ENABLE);
      break;  
    case 2:
      if(TIM_Handler[TIM2CH1] == NULL) {
        id = TIM2CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM2, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM2CH2] == NULL) {
        id = TIM2CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM2, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM2, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM2CH3] == NULL) {
        id = TIM2CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM2, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM2, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM2CH4] == NULL) {
        id = TIM2CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM2, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM2, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM2, ENABLE);
      break; 
    case 3:
      if(TIM_Handler[TIM3CH1] == NULL) {
        id = TIM3CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM3, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM3, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM3CH2] == NULL) {
        id = TIM3CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM3, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM3, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM3CH3] == NULL) {
        id = TIM3CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM3, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM3, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM3CH4] == NULL) {
        id = TIM3CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM3, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM3, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM3, ENABLE);
      break; 
    case 4:
      if(TIM_Handler[TIM4CH1] == NULL) {
        id = TIM4CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM4, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM4, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM4CH2] == NULL) {
        id = TIM4CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM4, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM4, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM4CH3] == NULL) {
        id = TIM4CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM4, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM4, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM4CH4] == NULL) {
        id = TIM4CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM4, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM4, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM4, ENABLE);
      break; 
    case 5:
      if(TIM_Handler[TIM5CH1] == NULL) {
        id = TIM5CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM5, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM5, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM5CH2] == NULL) {
        id = TIM5CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM5, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM5, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM5CH3] == NULL) {
        id = TIM5CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM5, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM5, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM5CH4] == NULL) {
        id = TIM5CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM5, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM5, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM5, ENABLE);
      break;  
    case 8:
      if(TIM_Handler[TIM8CH1] == NULL) {
        id = TIM8CH1;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC1Init(TIM8, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM8, TIM_IT_CC1, NewState);
      }
      else if(TIM_Handler[TIM8CH2] == NULL) {
        id = TIM8CH2;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC2Init(TIM8, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM8, TIM_IT_CC2, NewState);
      }
      else if(TIM_Handler[TIM8CH3] == NULL) {
        id = TIM8CH3;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC3Init(TIM8, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM8, TIM_IT_CC3, NewState);
      }
      else if(TIM_Handler[TIM8CH4] == NULL) {
        id = TIM8CH4;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_OCInitStructure.TIM_Pulse = period;
        TIM_OC4Init(TIM8, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Disable);
        TIM_ITConfig(TIM8, TIM_IT_CC4, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM8, ENABLE);
      break;
    default:
      id = INVALID_TIMER;
      break;
  }
  return id;
}

/**
 * @brief Registers an update interrupt for a given timer.
 * @param TIMx where x can be (6,7) to select the timer.
 * @param period specifies the period between timer interrupts.
 * @param Timer_Handler the handler function for the timer interrupt.
 * @param NewState new state of the timer interrupt. ENABLE or DISABLE
 * @retval the id for the given interrupt or error
 */
TIMER_ID Timer_UP_IRQ_Configuration(uint8_t TIMx, uint16_t period, void (*Timer_Handler)(void), FunctionalState NewState) {
  TIMER_ID id;
  switch(TIMx) {
    case 6:
      if(TIM_Handler[TIM6UP] == NULL) {
        id = TIM6UP;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_SetAutoreload(TIM6, period);
        TIM_ITConfig(TIM6, TIM_IT_Update, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM6, ENABLE);
      break;
    case 7:
      if(TIM_Handler[TIM7UP] == NULL) {
        id = TIM7UP;
        TIM_Handler[id] = Timer_Handler;
        TIM_IRQ_period[id] = period;
        TIM_SetAutoreload(TIM7, period);
        TIM_ITConfig(TIM7, TIM_IT_Update, NewState);
      }
      else {
        id = IRQ_UNAVAILABLE;
      }
      TIM_Cmd(TIM7, ENABLE);
      break;
    default:
      id = INVALID_TIMER;
      break;
  }
  return id;
}

/**
 * @brief Releases a given timer interrupt
 * @param id specifies the id of the timer interrupt
 * @retval None
 */
void Blox_Timer_Release_IRQ(TIMER_ID id) {
  Blox_Timer_Disable_IRQ(id);
  TIM_Handler[id] = NULL;
}

// calculations to fix CH[1-4] for TIM1 and TIM8 since CNT is reset to 0
/**
 * @brief Enables interrupts for a given interrupt
 * @param id specifies the id of the timer interrupt
 * @retval None
 */
void Blox_Timer_Enable_IRQ(TIMER_ID id) {
  switch(id) {
    case TIM1UP:
      TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
      TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
      break;
    case TIM1CH1:
      TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
      TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
      TIM_SetCompare1(TIM1, TIM_IRQ_period[id] + TIM_GetCounter(TIM1));
      break;
    case TIM1CH2:
      TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
      TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM1, TIM_IRQ_period[id] + TIM_GetCounter(TIM1));
      break;
    case TIM1CH3:
      TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
      TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM1, TIM_IRQ_period[id] + TIM_GetCounter(TIM1));
      break;
    case TIM1CH4:
      TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
      TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM1, TIM_IRQ_period[id] + TIM_GetCounter(TIM1));
      break;
    case TIM2CH1:
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
      TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE); 
      TIM_SetCompare1(TIM2, TIM_IRQ_period[id] + TIM_GetCounter(TIM2));
      break;
    case TIM2CH2:
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
      TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM2, TIM_IRQ_period[id] + TIM_GetCounter(TIM2));
      break;
    case TIM2CH3:
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
      TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM2, TIM_IRQ_period[id] + TIM_GetCounter(TIM2));
      break;
    case TIM2CH4:
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
      TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM2, TIM_IRQ_period[id] + TIM_GetCounter(TIM2));
      break;
    case TIM3CH1:
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
      TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
      TIM_SetCompare1(TIM3, TIM_IRQ_period[id] + TIM_GetCounter(TIM3));
      break;
    case TIM3CH2:
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
      TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM3, TIM_IRQ_period[id] + TIM_GetCounter(TIM3));
      break;
    case TIM3CH3:
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
      TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM3, TIM_IRQ_period[id] + TIM_GetCounter(TIM3));
      break;
    case TIM3CH4:
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
      TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM3, TIM_IRQ_period[id] + TIM_GetCounter(TIM3));
      break;
    case TIM4CH1:
      TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
      TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
      TIM_SetCompare1(TIM4, TIM_IRQ_period[id] + TIM_GetCounter(TIM4));
      break;
    case TIM4CH2:
      TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
      TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM4, TIM_IRQ_period[id] + TIM_GetCounter(TIM4));
      break;
    case TIM4CH3:
      TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
      TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM4, TIM_IRQ_period[id] + TIM_GetCounter(TIM4));
      break;
    case TIM4CH4:
      TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
      TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM4, TIM_IRQ_period[id] + TIM_GetCounter(TIM4));
      break;
    case TIM5CH1:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
      TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);
      TIM_SetCompare1(TIM5, TIM_IRQ_period[id] + TIM_GetCounter(TIM5));
      break;
    case TIM5CH2:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
      TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM5, TIM_IRQ_period[id] + TIM_GetCounter(TIM5));
      break;
    case TIM5CH3:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);
      TIM_ITConfig(TIM5, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM5, TIM_IRQ_period[id] + TIM_GetCounter(TIM5));
      break;
    case TIM5CH4:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);
      TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM5, TIM_IRQ_period[id] + TIM_GetCounter(TIM5));
      break;
    case TIM6UP:
      TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
      TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
      TIM_SetCounter(TIM6, 0);
      break;
    case TIM7UP:
      TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
      TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
      TIM_SetCounter(TIM7, 0);
      break;
    case TIM8UP:
      TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
      TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
      
      break;
    case TIM8CH1:
      TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
      TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);
      TIM_SetCompare1(TIM8, TIM_IRQ_period[id] + TIM_GetCounter(TIM8));
      break;
    case TIM8CH2:
      TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);
      TIM_ITConfig(TIM8, TIM_IT_CC2, ENABLE);
      TIM_SetCompare2(TIM8, TIM_IRQ_period[id] + TIM_GetCounter(TIM8));
      break;
    case TIM8CH3:
      TIM_ClearITPendingBit(TIM8, TIM_IT_CC3);
      TIM_ITConfig(TIM8, TIM_IT_CC3, ENABLE);
      TIM_SetCompare3(TIM8, TIM_IRQ_period[id] + TIM_GetCounter(TIM8));
      break;
    case TIM8CH4:
      TIM_ClearITPendingBit(TIM8, TIM_IT_CC4);
      TIM_ITConfig(TIM8, TIM_IT_CC4, ENABLE);
      TIM_SetCompare4(TIM8, TIM_IRQ_period[id] + TIM_GetCounter(TIM8));
      break;
  }
}

/**
 * @brief Disables interrupts for a given interrupt
 * @param id specifies the id of the timer interrupt
 * @retval None
 */
void Blox_Timer_Disable_IRQ(TIMER_ID id) {
  switch(id) {
    case TIM1UP:
      TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
      break;
    case TIM1CH1:
      TIM_ITConfig(TIM1, TIM_IT_CC1, DISABLE);
      break;
    case TIM1CH2:
      TIM_ITConfig(TIM1, TIM_IT_CC2, DISABLE);
      break;
    case TIM1CH3:
      TIM_ITConfig(TIM1, TIM_IT_CC3, DISABLE);
      break;
    case TIM1CH4:
      TIM_ITConfig(TIM1, TIM_IT_CC4, DISABLE);
      break;
    case TIM2CH1:
      TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE); 
      break;
    case TIM2CH2:
      TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);
      break;
    case TIM2CH3:
      TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);
      break;
    case TIM2CH4:
      TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);
      break;
    case TIM3CH1:
      TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
      break;
    case TIM3CH2:
      TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
      break;
    case TIM3CH3:
      TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);
      break;
    case TIM3CH4:
      TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);
      break;
    case TIM4CH1:
      TIM_ITConfig(TIM4, TIM_IT_CC1, DISABLE);
      break;
    case TIM4CH2:
      TIM_ITConfig(TIM4, TIM_IT_CC2, DISABLE);
      break;
    case TIM4CH3:
      TIM_ITConfig(TIM4, TIM_IT_CC3, DISABLE);
      break;
    case TIM4CH4:
      TIM_ITConfig(TIM4, TIM_IT_CC4, DISABLE);
      break;
    case TIM5CH1:
      TIM_ITConfig(TIM5, TIM_IT_CC1, DISABLE);
      break;
    case TIM5CH2:
      TIM_ITConfig(TIM5, TIM_IT_CC2, DISABLE);
      break;
    case TIM5CH3:
      TIM_ITConfig(TIM5, TIM_IT_CC3, DISABLE);
      break;
    case TIM5CH4:
      TIM_ITConfig(TIM5, TIM_IT_CC4, DISABLE);
      break;
    case TIM6UP:
      TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
      break;
    case TIM7UP:
      TIM_ITConfig(TIM7, TIM_IT_Update, DISABLE);
      break;
    case TIM8UP:
      TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);
      break;
    case TIM8CH1:
      TIM_ITConfig(TIM8, TIM_IT_CC1, DISABLE);
      break;
    case TIM8CH2:
      TIM_ITConfig(TIM8, TIM_IT_CC2, DISABLE);
      break;
    case TIM8CH3:
      TIM_ITConfig(TIM8, TIM_IT_CC3, DISABLE);
      break;
    case TIM8CH4:
      TIM_ITConfig(TIM8, TIM_IT_CC4, DISABLE);
      break;
  }
}

/**
 * @brief Modifies the period for a given output compare interrupt
 * @param id specifies the id of the timer interrupt
 * @param period specifies the new period between timer interrupts.
 * @retval None
 */
void Blox_Timer_Modify_IRQ(TIMER_ID id, uint16_t period) {
  TIM_IRQ_period[id] = period;
}

/**
  * @brief  This function handles timer 1 capture/compare interrupt request.
  * @retval None
  */
void TIM1_CC_IRQHandler(void) {
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    if(TIM_Handler[TIM1CH1] != NULL) {
      TIM_Handler[TIM1CH1]();
    }
    TIM_SetCompare1(TIM1, TIM_GetCapture1(TIM1) + TIM_IRQ_period[TIM1CH1]);
  }
  else if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    if(TIM_Handler[TIM1CH2] != NULL) {
      TIM_Handler[TIM1CH2]();
    }
    TIM_SetCompare2(TIM1, TIM_GetCapture2(TIM1) + TIM_IRQ_period[TIM1CH2]);
  }
  else if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
    if(TIM_Handler[TIM1CH3] != NULL) {
      TIM_Handler[TIM1CH3]();
    }
    TIM_SetCompare3(TIM1, TIM_GetCapture3(TIM1) + TIM_IRQ_period[TIM1CH3]);
  }
  else if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
    if(TIM_Handler[TIM1CH4] != NULL) {
      TIM_Handler[TIM1CH4]();
    }
    TIM_SetCompare4(TIM1, TIM_GetCapture4(TIM1) + TIM_IRQ_period[TIM1CH4]);
  }
}

/**
  * @brief  This function handles timer 2 interrupt request.
  * @retval None
  */
void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    if(TIM_Handler[TIM2CH1] != NULL) {
      TIM_Handler[TIM2CH1]();
    }
    TIM_SetCompare1(TIM2, TIM_GetCapture1(TIM2) + TIM_IRQ_period[TIM2CH1]);
  }
  else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    if(TIM_Handler[TIM2CH2] != NULL) {
      TIM_Handler[TIM2CH2]();
    }
    TIM_SetCompare2(TIM2, TIM_GetCapture2(TIM2) + TIM_IRQ_period[TIM2CH2]);
  }
  else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    if(TIM_Handler[TIM2CH3] != NULL) {
      TIM_Handler[TIM2CH3]();
    }
    TIM_SetCompare3(TIM2, TIM_GetCapture3(TIM2) + TIM_IRQ_period[TIM2CH3]);
  }
  else if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
    if(TIM_Handler[TIM2CH4] != NULL) {
      TIM_Handler[TIM2CH4]();
    }
    TIM_SetCompare4(TIM2, TIM_GetCapture4(TIM2) + TIM_IRQ_period[TIM2CH4]);
  }
}

/**
  * @brief  This function handles timer 3 interrupt request.
  * @retval None
  */
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    if(TIM_Handler[TIM3CH1] != NULL) {
      TIM_Handler[TIM3CH1]();
    }
    TIM_SetCompare1(TIM3, TIM_GetCapture1(TIM3) + TIM_IRQ_period[TIM3CH1]);
  }
  else if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    if(TIM_Handler[TIM3CH2] != NULL) {
      TIM_Handler[TIM3CH2]();
    }
    TIM_SetCompare2(TIM3, TIM_GetCapture2(TIM3) + TIM_IRQ_period[TIM3CH2]);
  }
  else if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    if(TIM_Handler[TIM3CH3] != NULL) {
      TIM_Handler[TIM3CH3]();
    }
    TIM_SetCompare3(TIM3, TIM_GetCapture3(TIM3) + TIM_IRQ_period[TIM3CH3]);
  }
  else if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    if(TIM_Handler[TIM3CH4] != NULL) {
      TIM_Handler[TIM3CH4]();
    }
    TIM_SetCompare4(TIM3, TIM_GetCapture4(TIM3) + TIM_IRQ_period[TIM3CH4]);
  }
}

/**
  * @brief  This function handles timer 4 interrupt request.
  * @retval None
  */
void TIM4_IRQHandler(void) {
  if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
    if(TIM_Handler[TIM4CH1] != NULL) {
      TIM_Handler[TIM4CH1]();
    }
    TIM_SetCompare1(TIM4, TIM_GetCapture1(TIM4) + TIM_IRQ_period[TIM4CH1]);
  }
  else if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
    if(TIM_Handler[TIM4CH2] != NULL) {
      TIM_Handler[TIM4CH2]();
    }
    TIM_SetCompare2(TIM4, TIM_GetCapture2(TIM4) + TIM_IRQ_period[TIM4CH2]);
  }
  else if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
    if(TIM_Handler[TIM4CH3] != NULL) {
      TIM_Handler[TIM4CH3]();
    }
    TIM_SetCompare3(TIM4, TIM_GetCapture3(TIM4) + TIM_IRQ_period[TIM4CH3]);
  }
  else if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
    if(TIM_Handler[TIM4CH4] != NULL) {
      TIM_Handler[TIM4CH4]();
    }
    TIM_SetCompare4(TIM4, TIM_GetCapture4(TIM4) + TIM_IRQ_period[TIM4CH4]);
  }
}

/**
  * @brief  This function handles timer 5 interrupt request.
  * @retval None
  */
void TIM5_IRQHandler(void) {
  if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
    if(TIM_Handler[TIM5CH1] != NULL) {
      TIM_Handler[TIM5CH1]();
    }
    TIM_SetCompare1(TIM5, TIM_GetCapture1(TIM5) + TIM_IRQ_period[TIM5CH1]);
  }
  else if (TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
    if(TIM_Handler[TIM5CH2] != NULL) {
      TIM_Handler[TIM5CH2]();
    }
    TIM_SetCompare2(TIM5, TIM_GetCapture2(TIM5) + TIM_IRQ_period[TIM5CH2]);
  }
  else if (TIM_GetITStatus(TIM5, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);
    if(TIM_Handler[TIM5CH3] != NULL) {
      TIM_Handler[TIM5CH3]();
    }
    TIM_SetCompare3(TIM5, TIM_GetCapture3(TIM5) + TIM_IRQ_period[TIM5CH3]);
  }
  else if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);
    if(TIM_Handler[TIM5CH4] != NULL) {
      TIM_Handler[TIM5CH4]();
    }
    TIM_SetCompare4(TIM5, TIM_GetCapture4(TIM5) + TIM_IRQ_period[TIM5CH4]);
  }
}

/**
  * @brief  This function handles timer 5 interrupt request.
  * @retval None
  */
void TIM6_IRQHandler(void) {
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    if(TIM_Handler[TIM6UP] != NULL) {
      TIM_Handler[TIM6UP]();
    }
  }
}

/**
  * @brief  This function handles timer 5 interrupt request.
  * @retval None
  */
void TIM7_IRQHandler(void) {
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    if(TIM_Handler[TIM7UP] != NULL) {
      TIM_Handler[TIM7UP]();
    }
  }
}
  
/**
  * @brief  This function handles timer 8 capture/compare interrupt request.
  * @retval None
  */
void TIM8_CC_IRQHandler(void) {
  if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
    if(TIM_Handler[TIM8CH1] != NULL) {
      TIM_Handler[TIM8CH1]();
    }
    TIM_SetCompare1(TIM8, TIM_GetCapture1(TIM8) + TIM_IRQ_period[TIM8CH1]);
  }
  else if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);
    if(TIM_Handler[TIM8CH2] != NULL) {
      TIM_Handler[TIM8CH2]();
    }
    TIM_SetCompare2(TIM8, TIM_GetCapture2(TIM8) + TIM_IRQ_period[TIM8CH2]);
  }
  else if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIM8, TIM_IT_CC3);
    if(TIM_Handler[TIM8CH3] != NULL) {
      TIM_Handler[TIM8CH3]();
    }
    TIM_SetCompare3(TIM8, TIM_GetCapture3(TIM8) + TIM_IRQ_period[TIM8CH3]);
  }
  else if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM8, TIM_IT_CC4);
    if(TIM_Handler[TIM8CH4] != NULL) {
      TIM_Handler[TIM8CH4]();
    }
    TIM_SetCompare4(TIM8, TIM_GetCapture4(TIM8) + TIM_IRQ_period[TIM8CH4]);
  }
}
/** @} */
