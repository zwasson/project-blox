/**
 * @file    blox_vusart.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/02/2010
 * @brief   Contains function prototypes for the virtual USART interface.
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

#ifndef __BLOX_VUSART_H
#define __BLOX_VUSART_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "blox_tim.h"
#include "blox_exti.h"

/**
 *@ingroup driver_vusart
 *@{
 */
#define VUSART_TIMx              2
#define VUSART_TIM_IRQn   TIM2_IRQn
#define VUSART_TIM_CLK           72000000
#define _9600bps          (uint16_t)(VUSART_TIM_CLK / 9600)
#define _19200bps         (uint16_t)(VUSART_TIM_CLK / 19200)
#define _38400bps         (uint16_t)(VUSART_TIM_CLK / 38400)
#define _57600bps         (uint16_t)(VUSART_TIM_CLK / 57600)
#define _115200bps        (uint16_t)(VUSART_TIM_CLK / 115200)

/* virtual USART for XBee */
#define VUSART1_GPIO    	      GPIOB
#define VUSART1_GPIO_CLK 	      RCC_APB2Periph_GPIOB
#define VUSART1_CLK             TIM_CLK
#define VUSART1_RxPin    	      GPIO_Pin_12
#define VUSART1_TxPin    	      GPIO_Pin_13
#define VUSART1_RxPinSource   	12
#define VUSART1_RxPortSource    GPIO_PortSourceGPIOB

/* virtual USART for OLED Display */
#define VUSART2_GPIO     	      GPIOA
#define VUSART2_GPIO_CLK 	      RCC_APB2Periph_GPIOA
#define VUSART2_CLK             TIM_CLK
#define VUSART2_RxPin    	      GPIO_Pin_1
#define VUSART2_TxPin    	      GPIO_Pin_0
#define VUSART2_RxPinSource   	1
#define VUSART2_RxPortSource    GPIO_PortSourceGPIOA

/**
 * @brief Status to return on VUSART commands
 */
typedef enum {
  VUSART_SUCCESS = 0,
  INVALID_ID,
  TX_BUSY,
  RX_EMPTY,
  RXNE_IRQ_UNAVAILABLE
} VUSART_STATUS;

void Blox_VUSART_Init(uint8_t id);
void Blox_VUSART_SetBaudrate(uint8_t id, uint16_t baudrate);
VUSART_STATUS Blox_VUSART_TryReceive(uint8_t id, uint8_t *data);
VUSART_STATUS Blox_VUSART_TrySend(uint8_t id, uint8_t data);
VUSART_STATUS Blox_VUSART_Receive(uint8_t id, uint8_t *data);
VUSART_STATUS Blox_VUSART_Send(uint8_t id, uint8_t data);
VUSART_STATUS Blox_VUSART_SendData(uint8_t id, uint8_t *data, uint32_t len);
VUSART_STATUS Blox_VUSART_Register_RXNE_IRQ(uint8_t id, void (*RXNE_Handler)(void));
VUSART_STATUS Blox_VUSART_Enable_RXNE_IRQ(uint8_t id);
VUSART_STATUS Blox_VUSART_Disable_RXNE_IRQ(uint8_t id);
/** @} */
#endif
