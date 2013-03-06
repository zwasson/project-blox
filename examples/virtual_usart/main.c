/********************************************************************************
 * @file    main.c
 * @author  Zach Wasson
 * @version V0.1
 * @brief   Tests the virtual USART driver.
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
void OLED_Handler (void);
void Blink_Handler (void);
void NULL_Handler (void);

TIMER_ID id = INVALID_TIMER;

int main(void)
{
  uint32_t i;
  
  //FIFO_Type FIFO_Test;
  //RCC_Configuration(); 
  //NVIC_Configuration(); 
  //GPIO_Configuration();
  SysTick_Init();
  //GPIOC->ODR &= ~(1<<3);
  Blox_VUSART_Init(2);
  //OLED_Init();
  //SysTick_Wait(20);
  //GPIOC->ODR |= (1<<3);
  SysTick_Wait(1000);

  /* Test program for virtual USART */
  //VUSART2_Tx is connected to USART4_Rx
  //VUSART2_Rx is connected to USART4_Tx 
  /*Blox_Virt_USART_Send(2, 0x15);
  data = Blox_USART_Receive(4);
  Blox_USART_Send(4, data);
  
  Blox_Virt_USART_Send(2, 0xDE);
  data = Blox_USART_Receive(4);
  Blox_USART_Send(4, data);
  
  Blox_Virt_USART_Send(2, 0xAD);
  data = Blox_USART_Receive(4);
  Blox_USART_Send(4, data);
  
  Blox_Virt_USART_Send(2, 0xBE);
  data = Blox_USART_Receive(4);
  Blox_USART_Send(4, data);
  
  Blox_Virt_USART_Send(2, 0xEF);
  data = Blox_USART_Receive(4);
  Blox_USART_Send(4, data);*/
  //Blox_VUSART_Receive(2, &data);
  //while(Blox_VUSART_Send(2, 0x55)==TX_BUSY);
  //while(Blox_VUSART_Receive(2, &data)==RX_EMPTY);
  
  SysTick_Wait(20);
  
  Blox_Timer_Init(1, 72000);
  Blox_Timer_Register_IRQ(1, 1000, &NULL_Handler, ENABLE);
  
  
  /*for(i = 0; i < 256; i++) {
    datarray[i] = i;
  }
  for(i = 0; i < 256; i++) {
    Blox_USART_Send(4, datarray[i]);
  }
  
  while((tmp = Blox_Virt_USART_Receive(2)) != FIFO_BAD) {
    USB_SendPat("%x\r\n", tmp);
  } */  
   
  /* Infinite loop */
  while (1)
  { 	
  }
}

void OLED_Handler(void) {
  static uint8_t count;
  count++;
  if(count >= 8)
    Blox_Timer_Disable_IRQ(id);
  GPIOD->ODR ^= (1<<13);
  Blox_Timer_Modify_IRQ(id, 1875);
}

void Blink_Handler (void) {
  GPIOA->ODR ^= (1<<8);
}

void NULL_Handler (void) {
  static uint32_t cnt;
  cnt++;
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
