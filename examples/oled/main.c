/********************************************************************************
 * @file    main.c
 * @author  Dan Cleary
 * @version V0.1
 * @brief   Test program for the OLED Display.
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
 
#include "stm32f10x.h"
#include "blox_exti.h"
#include "blox_fifo.h"
#include "blox_usb.h"
#include "blox_tim.h"
#include "blox_vusart.h"
#include "blox_counter.h"
#include "blox_oled.h"

void RCC_Configuration (void);
void GPIO_Configuration (void);
void NVIC_Configuration (void);

TIMER_ID id = INVALID_TIMER;

int main(void)
{
  uint32_t i;

  SysTick_Init();

  Blox_OLED_Init(); 

  /*Blox_OLED_Send(0x40);
  Blox_OLED_Send(0x49);
  Blox_OLED_Send(0x00);
  Blox_OLED_Send(0x00);
  Blox_OLED_Send(0x40);
  Blox_OLED_Send(0x40);
  Blox_OLED_Send(0x10);
  Blox_OLED_Send(0x00);
  Blox_OLED_Send(0x10);
  Blox_OLED_Send(0x00);*/
  //Blox_OLED_Send(0x0B);
 
  /*for(i = 0; i < 128; i++) {
    Blox_OLED_DrawLine(i, 0, i, 128, 44488);
  }*/  
  
  #define FRAME_RATE 50
  /* Infinite loop */
  while (1)
  {
    static uint8_t i = 64;
    Blox_OLED_SD_DisplayIcon(i, 32, 64, 64, 0x001000);
    SysTick_Wait(FRAME_RATE);
    i--;
    if(i == 0)
      i = 64;
    Blox_OLED_SD_DisplayIcon(i, 32, 64, 64, 0x001010);
    SysTick_Wait(FRAME_RATE);
    i--;
    if(i == 0)
      i = 64;
    Blox_OLED_SD_DisplayIcon(i, 32, 64, 64, 0x001020);
    SysTick_Wait(FRAME_RATE);
    i--;
    if(i == 0)
      i = 64;
    Blox_OLED_SD_DisplayIcon(i, 32, 64, 64, 0x001010);
    SysTick_Wait(FRAME_RATE);
    i--;
    if(i == 0)
      i = 64;
  }
}

/* System Clock Configuration */
void RCC_Configuration (void)
{
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/* GPIO Port Configuration */
void GPIO_Configuration (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* GPIOA Config */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //OLED Reset is push-pull, 50Mz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void NVIC_Configuration (void) {
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the EXTI10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}
