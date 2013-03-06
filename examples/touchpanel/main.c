/**
 * @file    main.c
 * @author  Ankita Kaul & Jesse Tannahill 
 * @version V0.1
 * @date    11/1/10
 * @brief   Tests the touchpanel driver.  
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
#include "blox_touch.h"
#include "blox_usb.h"
#include "blox_counter.h"

void RCC_Configuration (void);
void GPIO_Configuration (void);

int main(void)
{
  USB_Init();
  SysTick_Init();
  RCC_Configuration();  
  GPIO_Configuration();  
  Blox_Touch_Init();
  //Blox_Touch_GetX();
  while(1)
  {
    if(Blox_Touch_GetZ1(1) > 20) {
      long i;						      
	    for(i = 0; i < 1000000; i++);  	
	    GPIOA->ODR ^= (1<<8);
    }
    /*uint16_t Xval = Blox_Touch_GetX(1);
    uint16_t Yval = Blox_Touch_GetY(1); 
    uint16_t Z1val = Blox_Touch_GetZ1(1);
    uint16_t Z2val = Blox_Touch_GetZ2(1);
    USB_SendPat("X: %d\tY: %d\tZ1: %d\tZ2: %d\r\n\n", Xval, Yval, Z1val, Z2val);
    SysTick_Wait(100);*/
   }
  
  
 /* while (1)
  {
  	long i;						      
	for(i = 0; i < 1000000; i++);  	
	  GPIOA->ODR ^= (1<<8);
  }*/ 
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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
