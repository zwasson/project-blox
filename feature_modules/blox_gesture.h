/**
 * @file    blox_gesture.h
 * @author  Ankita Kaul
 * @version V0.1
 * @date    11/08/2010
 * @brief   Driver that performs touchpanel gesture detection 
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

#ifndef __BLOX_GESTURE_H
#define __BLOX_GESTURE_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#include "blox_system.h"
#include "blox_counter.h"
#include "blox_exti.h"
#include "blox_touch.h"
#include "blox_tim.h" 

/**
 * @ingroup feature_gesture
 * @{
 */
#define TOUCH_TIMx			3 
#define TOUCH_CLK			180000
#define	TOUCH_DETECT_FREQ 	3600	      //interrupt every .02s

#define PRESSURE_THRESHOLD  5
#define XTHRESH				25
#define	YTHRESH				25
#define XNOMOV				7
#define	YNOMOV				7

#define	TOUCH_X_STABLE		-1
#define	TOUCH_Y_STABLE		-1	
#define TOUCH_GESTURE_LR	1
#define	TOUCH_GESTURE_RL	2
#define TOUCH_GESTURE_DU	3
#define TOUCH_GESTURE_UD	4
#define TOUCH_DIAG_DLUR		5
#define TOUCH_DIAG_DRUL		6
#define TOUCH_DIAG_ULDR		7
#define TOUCH_DIAG_URDL		8
#define TOUCH_GESTURE_TAP	9 

/**
 * @brief Contains gesture data
 */
typedef struct {
  uint32_t timestamp;   /**< The timestamp of the gesture */
  int gesture;          /**< The gesture id */
} GestureRecord;

void Blox_Gesture_Init(void);
void Blox_Gesture_DeInit(void);
void Blox_touch1_tracker(void);
void Blox_touch2_tracker(void);
void Blox_touch3_tracker(void);
void Blox_touch4_tracker(void);
int Blox_Gesture_GetGesture(int touchNumber);
int Blox_Gesture_GetGestureTime(int touchNumber);
/** @} */
#endif
