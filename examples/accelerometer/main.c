/*******************************************************************************
 * @file    main.c
 * @author  Dan Cleary
 * @version V0.1
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
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "blox_accel.h"
#include "blox_debug.h"
#include "blox_usb.h"
#include "blox_system.h"

int main(void)
{
  int i;
  Blox_Accel_Init();
  USB_Init();

  USB_SendPat("HellO");
  /* Infinite loop */
  while (1)
  { 
      for (i = 0; i < 1000000; i++);
      USB_SendPat("X:%d  ", Blox_Accel_GetX()); 
      USB_SendPat("Y:%d  ", Blox_Accel_GetY());
      USB_SendPat("Z:%d  ", Blox_Accel_GetZ());
      
      USB_SendPat("\r\n");
  }
}
