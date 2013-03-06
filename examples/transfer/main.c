/********************************************************************************
 * @file    transfer.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/28/2010
 * @brief Tests the transfer program by interacting with transfer_test.py on a
 *        computer.
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
 
#include "blox_system.h"
#include "blox_transfer.h"
#include "blox_usb.h"
#include "blox_filesystem.h"
#include "blox_tim.h"
#include "blox_led.h"

void RCC_Configuration (void);
void GPIO_Configuration (void);


void blinky(void) {
  Blox_LED_Toggle(LED1);
}

int main(void)
{
  RCC_Configuration();    
  GPIO_Configuration();
  Blox_LED_Init();
  Blox_Timer_Init(1, 10000);
  Blox_Timer_Register_IRQ(1, 1000, &blinky, ENABLE);
  USB_Init();
  Transfer_Init();
  Transfer_Slave(); 
}

/* System Clock Configuration */
void RCC_Configuration (void)
{
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/* GPIO Port Configuration */
void GPIO_Configuration (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* GPIOA Config */
  GPIO_DeInit(GPIOA);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
