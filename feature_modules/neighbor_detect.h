/**
 * @file    neighbor_detect.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/17/2010
 * @brief   Contains function prototypes for the neighbor detection module
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

#ifndef __NEIGHBOR_DETECT
#define __NEIGHBOR_DETECT
 
#include "stm32f10x.h"
#include "blox_ir.h"
#include "blox_tim.h"

#include "blox_usb.h"
 
/**
 * @ingroup feature_neighbor
 * @{
 */
#define NEIGHBOR_TIMx             6
#define NEIGHBOR_TIM_CLK          10000
#define NEIGHBOR_SAMPLE_PERIOD    1000

void Neighbor_Detect_Init(void);
void Neighbor_Register_IR_RX_IRQ(uint8_t id, void (*IR_User_Handler)(IRFrame *frame));
uint8_t IR_Get_Neighbor(IR_DIR face_id);
/** @} */
#endif
