/**
 * @file    blox_fifo.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   A FIFO implemenation
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
 
#include "blox_fifo.h"
/**
 * @ingroup driver_fifo
 * @{
 */

/**
 * @brief Initializes the FIFO_Type structure provided
 * @param [in] fifo A pointer to the fifo to be initialized
 * @return None.
 */
void Blox_FIFO_Init(FIFO_Type *fifo) {
  fifo->read = 0;
  fifo->write = 0;
}
 
/**
 * @brief Puts data in fifo
 * @param [in] fifo A pointer to the fifo to be initialized
 * @param [in] data The data to be put in the fifo
 * @return status of the FIFO
 */
FIFO_STATUS Blox_FIFO_Put(FIFO_Type *fifo, uint16_t data) {
  if(Blox_FIFO_Size(fifo) < FIFO_SIZE) {
    fifo->data[fifo->write & (FIFO_SIZE - 1)] = data;
    fifo->write++;
    return FIFO_OK;
  }
  return -FIFO_FULL;
}

/**
 * @brief Gets data out of fifo
 * @param [in] fifo A pointer to the fifo to be initialized
 * @return data or FIFO_BAD if fifo is empty
 */
uint32_t Blox_FIFO_Get(FIFO_Type *fifo) {
  if(Blox_FIFO_Size(fifo) > 0) {
    uint16_t data;
    data = fifo->data[fifo->read & (FIFO_SIZE - 1)];
    fifo->read++;
    return data;
  }
  return FIFO_BAD;
}

/**
 * @brief Gets size of fifo
 * @param [in] fifo A pointer to the fifo to be initialized
 * @return the current size of the fifo
 */
uint8_t Blox_FIFO_Size(FIFO_Type *fifo) {
  return (fifo->write - fifo->read);
}

/** @} */
