/**
 * @file    blox_system.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/31/2010
 * @brief   Prototypes for system-wide functions and definitions.
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

#ifndef __BLOX_SYSTEM_H
#define __BLOX_SYSTEM_H

#include "stm32f10x.h"
#include "stdlib.h"

#include "blox_debug.h"

/**
 * @ingroup driver_system
 * @{
 */
#define TRUE 1
#define FALSE 0

#define NULL 0

#define PAGE_SIZE 0x800
#define WORD_SIZE 0x4

/** Memory Map **/
/* On STM32F103VE Flash : 0x08000000-0x08080000, 2K page size
 * 0x08000000
 *      .	      Base Program
 * 0x08008000
 *      .       System Variables
 * 0x08008800
 *      .	      FS FAT   
 * 0x08009000
 *      .	      FS File Store
 * 0x08080000
 */
#define MEM_MAP_START 0x08000000
#define MEM_MAP_SIZE 0x08080000
#define MEM_BASE_PROG_START 	0x08000000
#define MEM_BASE_PROG_SIZE 	PAGE_SIZE*32
#define MEM_SYS_VAR_START	0x08010000
#define MEM_SYS_VAR_SIZE 	PAGE_SIZE
#define MEM_STAGE_START		0x08010800
#define MEM_START_SIZE		PAGE_SIZE*32
#define MEM_FAT_START		0x08020800
#define MEM_FAT_SIZE		PAGE_SIZE
#define MEM_STORE_START		0x08021000
#define MEM_STORE_SIZE		PAGE_SIZE*190

#define MAX_DEINIT_FN 128
typedef void (*ptrVoidFn)(void);

#define SYS_MAGIC 0xCAFEBABE
#define SYS_INV_ID 0xFFFFFFFF
/**
 * @brief Defines a system variable
 */
typedef struct {
  uint32_t magic;       /**< a constant to validate the system variable */
	uint32_t id;          /**< the id of the Blox */
	int32_t ACCEL_X;      /**< calibration data for X-axis of accelerometer */
  int32_t ACCEL_Y;      /**< calibration data for Y-axis of accelerometer */
  int32_t ACCEL_Z;      /**< calibration data for Z-axis of accelerometer */
	int32_t TOUCH_1_X;    /**< calibration data for X-coordinate of touch1 */
  int32_t TOUCH_1_Y;    /**< calibration data for Y-coordinate of touch1 */
  int32_t TOUCH_2_X;    /**< calibration data for X-coordinate of touch2 */
  int32_t TOUCH_2_Y;    /**< calibration data for Y-coordinate of touch2 */
  int32_t TOUCH_3_X;    /**< calibration data for X-coordinate of touch3 */
  int32_t TOUCH_3_Y;    /**< calibration data for Y-coordinate of touch3 */
  int32_t TOUCH_4_X;    /**< calibration data for X-coordinate of touch4 */
  int32_t TOUCH_4_Y;    /**< calibration data for Y-coordinate of touch4 */
} SysVar;

void Blox_System_Init(void);
void Blox_System_Create(void);
void Blox_System_DeInit(void);
void Blox_System_Register_DeInit(ptrVoidFn fn);
uint32_t Blox_System_GetId(void);
void Blox_System_GetVars(SysVar *retSys);
void Blox_System_WriteVars(SysVar *newVars);
/** @} */
#endif
