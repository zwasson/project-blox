/**
 * @file    blox_usb.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/19/2010
 * @brief   Contains function prototypes for the USB interface.
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

#ifndef __BLOX_USB_H
#define __BLOX_USB_H

#include "stm32f10x.h"
#include "blox_usart.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

/**
 * @ingroup driver_usb
 * @{
 */
#define USB_USART_ID 1

void USB_Init(void);
uint8_t USB_Receive(void);
int16_t USB_TryReceive(void);
void USB_Send(uint8_t data);
void USB_SendData(uint8_t *data, uint32_t len);
void USB_SendPat(char *format, ...);
/** @} */
#endif
