/**
 * @file    blox_led.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/30/2010
 * @brief   A driver for the Blox LEDs
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

#include "blox_led.h"

void Blox_LED_DeInit_GPIO(void);
void Blox_LED_RCC_Configuration(void);
void Blox_LED_GPIO_Configuration(void);

/**
 * @ingroup driver_led
 * @{ */

/**
 * @brief Initializes the LEDs
 * @retval None
 */
void Blox_LED_Init(void) {
  Blox_LED_RCC_Configuration();
  Blox_LED_GPIO_Configuration();
  
  Blox_System_Register_DeInit(&RCC_DeInit);
  Blox_System_Register_DeInit(&Blox_LED_DeInit_GPIO);
}

/**
 * @brief Deinitializes the LEDs
 * @retval None
 */
void Blox_LED_DeInit_GPIO(void) {
  GPIO_DeInit(LED_GPIO);
}

/**
 * @brief Turns an LED on
 * @param id the id of the LED
 * @retval None
 */
void Blox_LED_On(LED_ID id) {
  if(id == 1) {
    LED_GPIO->ODR |= LED1_GPIO_Pin;
  } else if(id == 2) {
    LED_GPIO->ODR |= LED2_GPIO_Pin;
  } else if(id == 3) {
    LED_GPIO->ODR |= LED3_GPIO_Pin;
  } else if(id == 4) {
    LED_GPIO->ODR |= LED4_GPIO_Pin;
  }
}

/**
 * @brief Turns an LED off
 * @param id the id of the LED
 * @retval None
 */
void Blox_LED_Off(LED_ID id) {
  if(id == 1) {
    LED_GPIO->ODR &= ~LED1_GPIO_Pin;
  } else if(id == 2) {
    LED_GPIO->ODR &= ~LED2_GPIO_Pin;
  } else if(id == 3) {
    LED_GPIO->ODR &= ~LED3_GPIO_Pin;
  } else if(id == 4) {
    LED_GPIO->ODR &= ~LED4_GPIO_Pin;
  }
}

/**
 * @brief Toggles an LED
 * @param id the id of the LED
 * @retval None
 */
void Blox_LED_Toggle(LED_ID id) {
  if(id == 1) {
    LED_GPIO->ODR ^= LED1_GPIO_Pin;
  } else if(id == 2) {
    LED_GPIO->ODR ^= LED2_GPIO_Pin;
  } else if(id == 3) {
    LED_GPIO->ODR ^= LED3_GPIO_Pin;
  } else if(id == 4) {
    LED_GPIO->ODR ^= LED4_GPIO_Pin;
  }
}

/**
 * @brief Initializes the clocks for the LEDs
 * @retval None
 */
void Blox_LED_RCC_Configuration(void) {
  RCC_APB2PeriphClockCmd(LED_CLK, ENABLE);
}

/**
 * @brief Initializes the GPIOs for the LEDs
 * @retval None
 */
void Blox_LED_GPIO_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStructure; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LED1_GPIO_Pin | LED2_GPIO_Pin | LED3_GPIO_Pin | LED4_GPIO_Pin;
  GPIO_Init(LED_GPIO, &GPIO_InitStructure);
}
/** @} */
