/**
 * @file    blox_counter.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/30/2010
 * @brief   Basic millisecond-resolution counter. Wraps after ~500 days
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

#include "blox_counter.h"

/**
 * @ingroup driver_counter
 * @{
 */

/**
 * @brief Number of milliseconds since SysTick_Init
 */
static uint32_t milliseconds;

/**
 * @brief Number of seconds since SysTick_Init
 */
static uint32_t seconds;

/**
 * @brief Number of minutes since SysTick_Init
 */
static uint32_t minutes;

/**
 * @brief Initializes the SysTick driver
 * @retval None.
 */
void SysTick_Init(void) {
  milliseconds = 0;
  seconds = 0;
  minutes = 0;
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
  NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
 * @brief Returns the number of milliseconds since SysTick_Init() was called.
 * @retval the number of milliseconds since SysTick_Init() was called.
 */
uint32_t SysTick_Get_Milliseconds(void) {
  return milliseconds;
}

/**
 * @brief Returns the number of seconds since SysTick_Init() was called.
 * @retval the number of seconds since SysTick_Init() was called.
 */
uint32_t SysTick_Get_Seconds(void) {
  return seconds;
}

/**
 * @brief Returns the number of minutes since SysTick_Init() was called.
 * @retval the number of minutes since SysTick_Init() was called.
 */
uint32_t SysTick_Get_Minutes(void) {
  return minutes;
}

/**
 * @brief Performs a blocking wait for ms milliseconds.
 * @param ms the number of milliseconds to wait
 * @retval None.
 */
void SysTick_Wait(uint32_t ms) {
  uint32_t currentTime = SysTick_Get_Milliseconds();
  while(SysTick_Get_Milliseconds() < currentTime + ms);
}

/**
 * @brief The interrupt handler that updates the global time values.
 * @retval None.
 */
void SysTick_Handler(void) {
  milliseconds++;
  if((milliseconds % 1000) == 0) {
    seconds++;
	  if((seconds % 60) == 0) {
      minutes++;
  	}
  }
}
