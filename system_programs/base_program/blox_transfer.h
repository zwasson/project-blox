/**
 * @file    blox_transfer.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   Contains function prototypes for the transfer interface.
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

#ifndef __BLOX_TRANSFER_H
#define __BLOX_TRANSFER_H

#include "blox_system.h"
#include "blox_usb.h"
#include "blox_filesystem.h"

/**
 * @ingroup base_transfer
 * @{
 */
#define TRANSFER_ACK 0x79
#define TRANSFER_NAK 0x1F

/**
 * @brief Enum of the possible transfer statuses
 */
typedef enum {
	TRANSFER_OK = 0,
	TRANSFER_CMD_FAIL,
	TRANSFER_INV_OPCODE	
} TRANSFER_STATUS;

/**
 * @brief Enum of the available opcodes
 */
typedef enum {
  OP_BOT = 0,
	RCV_APP,
	DEL_APP,
	LST_APPS,
	RUN_APP,
  OP_TOP
} TRANSFER_OPCODE;

void Transfer_Init(void);
void Transfer_Slave(void);
/** @} */
#endif
