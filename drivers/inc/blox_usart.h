/**
 * @file    blox_usart.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   Contains function prototypes for the usart interface.
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

#ifndef __BLOX_USART_H
#define __BLOX_USART_H

#include "blox_system.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"

/**
 * @ingroup driver_usart
 * @{
 */

#define USART1_GPIO    	 	GPIOA
#define USART1_CLK      	RCC_APB2Periph_USART1
#define USART1_GPIO_CLK 	RCC_APB2Periph_GPIOA
#define USART1_RxPin    	GPIO_Pin_10
#define USART1_TxPin    	GPIO_Pin_9

#define USART2_GPIO     	GPIOA
#define USART2_CLK      	RCC_APB1Periph_USART2
#define USART2_GPIO_CLK 	RCC_APB2Periph_GPIOA
#define USART2_RxPin    	GPIO_Pin_3
#define USART2_TxPin    	GPIO_Pin_2

#define USART3_GPIO     	GPIOB
#define USART3_CLK      	RCC_APB1Periph_USART3
#define USART3_GPIO_CLK 	RCC_APB2Periph_GPIOB
#define USART3_RxPin    	GPIO_Pin_11
#define USART3_TxPin    	GPIO_Pin_10

#define UART4_GPIO     		GPIOC
#define UART4_CLK      		RCC_APB1Periph_UART4
#define UART4_GPIO_CLK 		RCC_APB2Periph_GPIOC
#define UART4_RxPin    		GPIO_Pin_11
#define UART4_TxPin    		GPIO_Pin_10

#define UART5_GPIO_TX   	GPIOC
#define UART5_GPIO_RX		  GPIOD
#define UART5_CLK      		RCC_APB1Periph_UART5
#define UART5_GPIO_TX_CLK RCC_APB2Periph_GPIOC
#define UART5_GPIO_RX_CLK RCC_APB2Periph_GPIOD
#define UART5_RxPin    		GPIO_Pin_2
#define UART5_TxPin    		GPIO_Pin_12

void Blox_USART_Init(uint8_t);
uint8_t Blox_USART_Receive(uint8_t id);
int16_t Blox_USART_TryReceive(uint8_t id);
void Blox_USART_Send(uint8_t id, uint8_t data);
void Blox_USART_Register_RXNE_IRQ(uint8_t id, void (*RXNE_Handler)(void));
void Blox_USART_Enable_RXNE_IRQ(uint8_t id);
void Blox_USART_Disable_RXNE_IRQ(uint8_t id);
/** @} */
#endif
