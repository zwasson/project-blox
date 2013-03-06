/********************************************************************************
 * @file    main.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @brief Test program for neighbor detection.
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
#include "blox_ir.h"
#include "blox_usb.h"
#include "blox_tim.h"
#include "blox_role.h"
#include "blox_counter.h"
#include "neighbor_detect.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define TxIR_ID 3
#define RxIR_ID 4

void RCC_Configuration(void);
void GPIO_Configuration(void);
void IR_Rx(void);
void IR_Tx(void);

typedef struct {
  uint8_t bool;
  char string[5];
  uint32_t id;
} test_t;

void IR_User_Handler(IRFrame *frame) {
  test_t *var = (test_t *)(frame->data);
  if(var->bool == TRUE) {
    USB_Send('1');
  } else {
    USB_Send('0');
  }
  /*uint8_t i;
  for(i = 0; i < frame->len; i++)
    USB_Send(frame->data[i]);*/
}

int main(void)
{  
  Blox_Role_Init("ir_role_test", 12);
  Blox_Role_Add(&IR_Tx, 1, 1);
  Blox_Role_Add(&IR_Rx, 1, 1); 
  Blox_Role_Run();
  
  while(1);
  
  /*IR_Tx();
  IR_Rx();
  test_t test_var;
  
  SysTick_Init();
  
  USB_Init();
  Neighbor_Detect_Init();
  Neighbor_Register_IR_RX_IRQ(1, &IR_User_Handler);
  Neighbor_Register_IR_RX_IRQ(2, &IR_User_Handler);
  Neighbor_Register_IR_RX_IRQ(3, &IR_User_Handler);
  Neighbor_Register_IR_RX_IRQ(4, &IR_User_Handler);
  
  test_var.id = 0xF398;
  test_var.string[0] = 'c';
  test_var.string[1] = 'o';
  test_var.string[2] = 'o';
  test_var.string[3] = 'l';
  test_var.string[4] = 0;
  test_var.bool = TRUE;
  while (1) {
    uint8_t i;
    for(i = 0; i < 4; i++) {
      IR_SendFrame(i, IR_FRAME_TYPE_USER, (uint8_t *)(&test_var), sizeof(test_t));
      //IR_SendFrame(i, IR_FRAME_TYPE_USER, "FUN", 3);
    }
    SysTick_Wait(500);
  }*/
}

/*void Blinky(void) {
  GPIOA->ODR ^= (1<<8);
  USB_Send(IR_Receive(RxIR_ID));
}*/

void IR_RX_Test_Handler(IRFrame *frame) {
  GPIOA->ODR ^= (1<<8);
  USB_Send(frame->data[0]);
  free(frame->data);
  free(frame);
}

void IR_Tx(void) {
  SysTick_Init();
  RCC_Configuration();
  GPIO_Configuration();
  IR_Init(TxIR_ID);
  while(1) {
    //IR_Send(TxIR_ID, 0x41);
    IR_SendFrame(TxIR_ID, IR_FRAME_TYPE_USER, "Hello", 5);
    GPIOA->ODR ^= (1<<8);
    SysTick_Wait(100);
  }
}

void IR_Rx(void) {
  SysTick_Init();
  RCC_Configuration();
  GPIO_Configuration();
  IR_Init(RxIR_ID);
  USB_Init();
  Blox_IR_Register_RX_IRQ(RxIR_ID, &IR_RX_Test_Handler);
  Blox_IR_Enable_RX_IRQ(RxIR_ID);
  
  while(1) {
    //USB_Send(IR_Receive(RxIR_ID));
    //GPIOA->ODR ^= (1<<8);
  }
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
