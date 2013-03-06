/**
 * @file    blox_ir.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/19/2010
 * @brief   Contains function prototypes for the IR interface.
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

#ifndef __BLOX_IR_H
#define __BLOX_IR_H

#include "stm32f10x.h"
#include "blox_usart.h"

#include "stdio.h"
#include "string.h"

/**
 * @ingroup driver_ir
 * @{
 *
 * @brief A mapping from IR ID to cardinal directions.
 */
typedef enum {
  IR_EAST_ID = 1,
  IR_SOUTH_ID,
  IR_NORTH_ID,
  IR_WEST_ID
} IR_DIR;

#define IR_1_USART_ID 4
#define IR_2_USART_ID 5
#define IR_3_USART_ID 2
#define IR_4_USART_ID 3

#define IR_SHUTDOWN_GPIO 		GPIOD
#define IR_SHUTDOWN_GPIO_CLK 	RCC_APB2Periph_GPIOD
#define IR_SHUTDOWN_PIN			GPIO_Pin_13
#define IR_SHUTDOWN_PIN_NUM		13

/**
 * @brief Mapping for types of IR frames.
 */
typedef enum {
  IR_FRAME_TYPE_NEIGHBOR,
  IR_FRAME_TYPE_USER
} IRFrameType;

#define IR_MAX_FRAME_LEN 100 
/**
 * @brief Defines data an IRFrame sends
 */
typedef struct {
  uint8_t src_id;           /**< the BLOX_ID of this Blox */
  uint8_t src_face_id;      /**< the ID of the sending face */
  IRFrameType type;         /**< Type to differentiate uses of IR */
  uint8_t len;              /**< the amount of data being sent */
  uint8_t *data;            /**< a pointer to the data being sent */
  uint8_t checksum;         /**< a checksum of the data */
} IRFrame;

void IR_Init(uint8_t id);
uint8_t IR_Receive(uint8_t id);
uint8_t IR_TryReceive(uint8_t id);
void IR_Send(uint8_t id, uint8_t data);
void IR_Sleep(void);
void IR_Wake(void);
void Blox_IR_Register_RX_IRQ(uint8_t id, void (*RX_Handler)(IRFrame *frame));
void Blox_IR_Enable_RX_IRQ(uint8_t id);
void Blox_IR_Disable_RX_IRQ(uint8_t id);
void IR_SendFrame(uint8_t id, IRFrameType type, uint8_t *data, uint8_t len);
/** @} */
#endif
