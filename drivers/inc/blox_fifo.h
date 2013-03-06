/**
 * @file    blox_fifo.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   Contains FIFO definition and function prototypes for FIFO interaction.
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

#ifndef __BLOX_FIFO_H
#define __BLOX_FIFO_H

#include "stm32f10x.h"

/**
 * @ingroup driver_fifo
 * @{
 */
/* FIFO_SIZE must be a power of two and less than 256*/
#define FIFO_SIZE 32
#define FIFO_BAD  65536

/**
 * @brief 			Basic struct for each fifo
 */
typedef struct {
  uint16_t data[FIFO_SIZE];   /**< Buffer for reading */
  uint8_t read;               /**< Next character to read */
  uint8_t write;              /**< Next character to write */
} FIFO_Type;

/**
 * @brief 	Enum to return the status of the FIFO driver
 */
typedef enum {
  FIFO_EMPTY = -1,
  FIFO_OK,
  FIFO_FULL
} FIFO_STATUS;

void Blox_FIFO_Init(FIFO_Type *fifo);
FIFO_STATUS Blox_FIFO_Put(FIFO_Type *fifo, uint16_t data);
uint32_t Blox_FIFO_Get(FIFO_Type *fifo);
uint8_t Blox_FIFO_Size(FIFO_Type *fifo);
/** @} */
#endif
