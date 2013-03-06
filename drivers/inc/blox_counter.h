/**
 * @file    blox_counter.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/30/2010
 * @brief   Contains function prototypes for the counter.
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

#ifndef __BLOX_COUNTER_H
#define __BLOX_COUNTER_H

#include "stm32f10x.h"
/**
 *@ingroup driver_counter
 * @{
 */
void SysTick_Init(void);
uint32_t SysTick_Get_Milliseconds(void);
uint32_t SysTick_Get_Seconds(void);
uint32_t SysTick_Get_Minutes(void);
void SysTick_Wait(uint32_t ms);
/** @} */
#endif

