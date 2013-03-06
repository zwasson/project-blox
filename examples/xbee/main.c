/********************************************************************************
 * @file    main.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @brief Tests the XBee driver.
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
#include "blox_xbee.h"
#include "blox_usb.h"
#include "blox_role.h"

void RCC_Configuration (void);
void GPIO_Configuration (void);

void blink (BloxFrame *frame) {
  USB_SendPat("%u: %d\r\n", frame->src_id, frame->data[0]);
  GPIOA->ODR ^= (1<<8);
}

void xbee_receive() {
  uint8_t ret;
  RoleFrame frame;
  RCC_Configuration(); //for on-board LED
  GPIO_Configuration(); //for on-board LED
  USB_Init();
  Blox_System_Init();
  USB_SendPat("Xbee Test Receive Init\r\n");
  USB_SendPat("ID:%x\r\n", Blox_System_GetId());
  ret = Blox_XBee_Init();
  if(ret == XBEE_OK)
    USB_SendPat("XBee Test Init Success\r\n");
  Blox_XBee_Register_RX_IRQ(&blink);
  Blox_XBee_Enable_RX_IRQ();

  while(1) ;
}

void xbee_send() {
  uint8_t ret;
  RCC_Configuration(); //for on-board LED
  GPIO_Configuration(); //for on-board LED
  USB_Init();
  Blox_System_Init();
  USB_SendPat("Xbee Test Send Init\r\n");
  USB_SendPat("ID:%x\r\n", Blox_System_GetId());
  ret = Blox_XBee_Init();
  if(ret == XBEE_OK)
    USB_SendPat("XBee Test Init Success\r\n");

  while(1) {
    Blox_XBee_Send("X", 1, FRAME_TYPE_USER, XBEE_BLOX_BROADCAST_ID);
    SysTick_Wait(100);
    GPIOA->ODR ^= (1<<8);
  }
}

int main(void)
{
  //xbee_receive();
  xbee_send();
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
}
