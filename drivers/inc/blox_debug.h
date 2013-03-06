/**
 * @file    blox_debug.h
 * @author  Jesse Tannahill
 * @version V1.0
 * @date    10/27/2010
 * @brief   Contains global debugging information for different modules to use
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

#ifndef __BLOX_DEBUG_H
#define __BLOX_DEBUG_H

/**
 * @ingroup driver_debug
 * @{
 */
#define BLOX_DEBUG 0
#if BLOX_DEBUG
  #include "string.h"
  #include "blox_usb.h"

  #define Blox_Debug_Init() USB_Init();
  #define Blox_Debug(data) USB_Send(data);
  #define Blox_DebugStr(data) { USB_SendData((uint8_t *)data, strlen(data)); }
  #define Blox_DebugPat(format, ...) { USB_SendPat(format, __VA_ARGS__); }
  
#else
  #define Blox_Debug_Init() 
  #define Blox_Debug(data)
  #define Blox_DebugStr(data)
  #define Blox_DebugPat(format, ...)
#endif

/** @} */
#endif
