/**
 * @file    blox_system.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/19/2010
 * @brief   Defines system-wide concepts including the memory map, and
 *          deinitialization.
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

#include "blox_system.h"
#include "blox_filesystem.h"
/**
 * @ingroup driver_system
 * @{
 */
SysVar *sys = (SysVar *)MEM_SYS_VAR_START;;
static ptrVoidFn deInit[MAX_DEINIT_FN];
static uint32_t numDeInit;


/**
 * @brief Initializes the pointer to the system variables array
 * @retval None
 */
void Blox_System_Init(void) {
  Blox_Debug_Init();
  //sys = (SysVar *)MEM_SYS_VAR_START;  
  if(sys->magic != SYS_MAGIC) {
    Blox_DebugPat("Blox_System_Init sys magic fails, got:%x\r\n", sys->magic);
    while(1) ;
  }   
    
  numDeInit = 0;
}

/**
 * @brief De-initializes all the peripherals in the system.
 * @retval None
 */
void Blox_System_DeInit(void) {
  uint32_t i;
  for (i = 0; i < numDeInit; i++)
    deInit[i]();
}

/**
 * @brief Registers a new function to be called when the system deInits.
 *        Only adds if it isn't already there.
 * @param fn: a fn pointer to a module's deinit function
 * @retval None
 */
void Blox_System_Register_DeInit(ptrVoidFn fn) {
  uint32_t i;
  if (numDeInit == MAX_DEINIT_FN) {
    Blox_DebugPat("Blox_System_Register_DeInit numDeInit reached max of:%d\r\n",
        MAX_DEINIT_FN);
    while(1) ;
  }
  
  for (i = 0; i < numDeInit; i++) {
    if(deInit[i] == fn)
      return;
  }
  
  deInit[numDeInit++] = fn;
}

/**
 * @brief Creates an initial SysVar struct.
 * @retval None.
 */
void Blox_System_Create(void) {
  SysVar *sys_new = (SysVar *)malloc(PAGE_SIZE);
  sys_new->magic = SYS_MAGIC;
  sys_new->id = SYS_INV_ID;
  sys_new->ACCEL_X = 0;
  sys_new->ACCEL_Y = 0;
  sys_new->ACCEL_Z = 0;
  sys_new->TOUCH_1_X = 0;
  sys_new->TOUCH_1_Y = 0;
  sys_new->TOUCH_2_X = 0;
  sys_new->TOUCH_2_Y = 0;
  sys_new->TOUCH_3_X = 0;
  sys_new->TOUCH_3_Y = 0;
  sys_new->TOUCH_4_X = 0;
  sys_new->TOUCH_4_Y = 0; 
   
  sys = (SysVar *)MEM_SYS_VAR_START;
  FS_SwapPage((uint32_t *)sys_new, (uint32_t *)sys);
  free(sys_new);
}

/**
 * @brief Returns the current SysVar struct.
 * @retval the blox's id
 */
uint32_t Blox_System_GetId(void) {
  return sys->id;
}

/**
 * @brief Returns the current SysVar struct.
 * @retval the current SysVar struct
 */
void Blox_System_GetVars(SysVar *retSys) { 
  memcpy(retSys, sys, sizeof(SysVar));
}

/**
 * @brief Writes over the SysVar in flash.
 * @param newVars: a SysVar with the new contents to be stored in flash.
 * @retval None.
 */
void Blox_System_WriteVars(SysVar *newVars) {
  FS_SwapPage((uint32_t *)newVars, (uint32_t *)sys);
}
/** @} */
