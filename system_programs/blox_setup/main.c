/**
 * @file    blox_setup.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    11/7/2010
 * @brief   Sets up the system variables and filesystem to a known good state.
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
#include "blox_filesystem.h"
#include "blox_xbee.h"
#include "blox_usb.h"

/**
 * @ingroup blox_setup
 * @{
 */
#define BLOX_ID 1

#define DEF_ACCEL_X 128
#define DEF_ACCEL_Y 128
#define DEF_ACCEL_Z 128

#define DEF_TOUCH_X 64
#define DEF_TOUCH_Y 64

void RCC_Configuration (void);
void GPIO_Configuration (void);

/**
 * @brief Sets up the the blox system
 * @retval None
 */
int main(void)
{
  SysVar vars;
  RCC_Configuration();  
  GPIO_Configuration();
  
  Blox_System_Create();
  Blox_System_Init();
  USB_Init();
  Blox_System_GetVars(&vars);
  
  vars.id = BLOX_ID;
  vars.ACCEL_X = DEF_ACCEL_X;
  vars.ACCEL_Y = DEF_ACCEL_Y;
  vars.ACCEL_Z = DEF_ACCEL_Z;
  vars.TOUCH_1_X = DEF_TOUCH_X;
  vars.TOUCH_1_Y = DEF_TOUCH_Y;
  vars.TOUCH_2_X = DEF_TOUCH_X;
  vars.TOUCH_2_Y = DEF_TOUCH_Y;
  vars.TOUCH_3_X = DEF_TOUCH_X;
  vars.TOUCH_3_Y = DEF_TOUCH_Y;
  vars.TOUCH_4_X = DEF_TOUCH_X;
  vars.TOUCH_4_Y = DEF_TOUCH_Y;  
  Blox_System_WriteVars(&vars);
  
  if (FS_CreateFS() == FS_CREATE_FAIL) {
    //Trap
    while (1);
  }
  USB_SendPat("Configuring Xbee...\r\n");
  if (Blox_XBee_Config() == XBEE_INIT_FAIL)
    USB_SendPat("Failed\r\n");
  else {
    USB_SendPat("Success\r\n");
    Blox_XBee_Print();
  }
  
  /* Infinite loop */
  while (1) {
    long i;
    GPIOA->ODR ^= (1<<8);  
    for(i = 0; i < 1000000; i++);
  }
}

/**
 * @brief Configures the clocks
 * @retval None
 */
void RCC_Configuration (void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/**
 * @brief Configures the GPIOs
 * @retval None
 */
void GPIO_Configuration (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/** @} */
