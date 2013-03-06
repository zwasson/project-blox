/**
 * @file    blox_role.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    11/03/2010
 * @brief   Module that allows a distributed program to be stored in a single
 *          program and then started on multiple Blox automatically.
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

#include "blox_role.h"
#include "blox_led.h"

/**
 * @ingroup feature_role
 * @{
 */
#define ROLE_FLAG_LOC 0x20006000
#define ROLE_FN_LOC   0x20006004

/* Private function prototypes */
void Blox_Role_RX(BloxFrame *frame);
uint8_t Role_NextID(void);

/**
 * @brief The RoleInfo struct for this program.
 */
static RoleInfo info;

/**
 * An ENUM for possible parent/child states
 */
typedef enum {
  STATE_EMPTY,
  STATE_PARENT,
  STATE_CHILD
} State;

/**
 * @brief State of the currently running application.
 */
static State parent = STATE_EMPTY;
/**
 * @brief The role_id being executed
 */
static uint8_t role_id = 0;
/**
 * @brief A flag denoting if a base program has been allocated but not yet run.
 */
static uint8_t allocating = FALSE;

/**
 * @brief Initializes the role driver's data structures.
 * @param name The name of the role
 * @param len The length of the name
 * @retval ROLE_OK if everything initializes correctly.
 */
ROLE_STATUS Blox_Role_Init(char *name, uint8_t len) {
  if (*(uint32_t *)ROLE_FLAG_LOC == 1) {
    *(uint32_t *)ROLE_FLAG_LOC = 0;
    (*(ptrVoidFn *)ROLE_FN_LOC)();
  }

  SysTick_Init();
  Blox_XBee_Init();
  Blox_XBee_Register_RX_IRQ(&Blox_Role_RX);
  Blox_XBee_Enable_RX_IRQ();   
  Blox_LED_Init();
  
  memset(&info, 0, sizeof(RoleInfo));
  memcpy(info.name, name, len);
  info.name_len = len;
  info.num_blox_found = 1;
  info.num_blox_started = 1;
 
  return ROLE_OK;
}

/**
 * @brief Executes when XBee receives a BloxFrame.
 * @param frame The BloxFrame parsed out of a received XBee frame.
 * @retval None.
 */
void Blox_Role_RX(BloxFrame *frame) {
  static uint16_t id_in_progress = 0xFFFF;
  static uint16_t count = 0;
  count++;
  //Addressed to me?
  if(frame->dst_id == XBEE_BLOX_BROADCAST_ID || frame->dst_id == Blox_System_GetId()
          && (frame->src_id == id_in_progress || allocating == FALSE)) {
    if (frame->type == FRAME_TYPE_ROLE) {
      RoleFrame respFrame;
      RoleFrame *rFrame = (RoleFrame *)(&(frame->data));
      switch(rFrame->opcode) {
      case PARENT_QUERY:  
        if(parent == STATE_PARENT) {
          if (memcmp(info.name, ((QueryFrame *)&(rFrame->data))->name, info.name_len) == 0) {
            SysTick_Wait(XBEE_HOLD_PERIOD); //Wait for other to stop transmitting PARENT_QUERY
            respFrame.opcode = PARENT_ACK;
            ((ParentAckFrame *)&(respFrame.data))->role_id = Role_NextID(); //TODO: Get a Role ID here
            Blox_LED_On(LED2);
            Blox_XBee_Send_Period((uint8_t *)&respFrame, sizeof(RoleFrame), FRAME_TYPE_ROLE, frame->src_id, XBEE_HOLD_PERIOD);
            Blox_LED_Off(LED2);
            info.num_blox_started++;
            allocating = FALSE;
            id_in_progress = 0xFFFF;
          }
        }
        break;
      case PARENT_ACK:
        Blox_LED_On(LED4);
        role_id = ((ParentAckFrame *)(&(rFrame->data)))->role_id;
        parent = STATE_CHILD;
        break;
      case PROG_ACK:
        id_in_progress = frame->src_id;
        allocating = TRUE;
        Blox_LED_On(LED4);
        SysTick_Wait(XBEE_HOLD_PERIOD); //Hold period
        Blox_LED_Off(LED4);
        respFrame.opcode = PROG_START;
        memcpy(&(((QueryFrame *)&respFrame.data)->name), info.name, FS_FILE_MAX_NAME_LEN);
        Blox_XBee_Send_Period((uint8_t *)&respFrame, sizeof(RoleFrame), FRAME_TYPE_ROLE, frame->src_id, XBEE_HOLD_PERIOD);
        info.num_blox_found++;
      }
    }
  }
}

/**
 * @brief Adds a new role to the role driver.
 * @param fn: the function to be run for the new role.
 * @param min: the minimum number of Blox necessary of this role.
 * @param max: the maxiumum number of Blox wanted of this role.
 * @retval ROLE_OK if the add is successful.
 * @retval ROLE_OOM if the maximum number of roles have been added.
 * @retval ROLE_ADD_FAIL if min is negative or larger than max
 */
ROLE_STATUS Blox_Role_Add(ptrVoidFn fn, uint8_t min, uint8_t max) {
  if (info.num_roles == ROLE_MAX)
    return ROLE_OOM;
  if (min == 0 || min > max)
    return ROLE_ADD_FAIL;
  
  info.roles[info.num_roles].fn = fn;
  info.roles[info.num_roles].min = min;
  info.num_needed += min;
  info.roles[info.num_roles].max = max;
  info.num_wanted += max;
  info.roles[info.num_roles++].num_allocated = 0;
  
  if(info.num_roles == 1) {
    role_id = 0;
    info.roles[0].num_allocated = 1;
  }
  return ROLE_OK; 
}

/**
 * @brief  Runs the role, which is a multiple-step procedure. First the Blox
 *         should determine if it is the original starter of this application.
 *         if so, then it will query Blox running base programs to see if they
 *         can run the requested application. It then requests those base
 *         programs begin running the application and designates a role to the
 *         application once it requests a role.
 * @retval ROLE_RUN_FAIL if there are not enough Blox in the area to run.
 *         otherwise this method never returns.
 */
ROLE_STATUS Blox_Role_Run(void) {
  RoleFrame frame;
  uint8_t num_found;
  
  //Am I the parent?
  frame.opcode = PARENT_QUERY;
  memcpy(&(((QueryFrame *)frame.data)->name), info.name, FS_FILE_MAX_NAME_LEN);
  Blox_XBee_Send_Period((uint8_t *)&frame, sizeof(RoleFrame), FRAME_TYPE_ROLE, XBEE_BLOX_BROADCAST_ID, XBEE_HOLD_PERIOD);
  Blox_LED_On(LED1);
  SysTick_Wait(XBEE_HOLD_PERIOD*2); //Let parent respond.
  Blox_LED_Off(LED1);

  if (parent == STATE_CHILD) {
    *(uint32_t *)ROLE_FLAG_LOC = 1;
    FS_SetAppFlag(1);
    *(uint32_t *)ROLE_FN_LOC = (uint32_t)info.roles[role_id].fn;
    NVIC_SystemReset();
  }
  
  //I'm the parent. Find base programs with the program.
  parent = STATE_PARENT;  
  frame.opcode = PROG_QUERY;
  memcpy(&(((QueryFrame *)&frame.data)->name), info.name, FS_FILE_MAX_NAME_LEN);
  num_found = info.num_blox_found;
  while (info.num_blox_found < info.num_wanted) {
    allocating = FALSE;
    Blox_LED_On(LED3);
    Blox_XBee_Send_Period((uint8_t *)&frame, sizeof(RoleFrame), FRAME_TYPE_ROLE, XBEE_BLOX_BROADCAST_ID, XBEE_HOLD_PERIOD);
    Blox_LED_Off(LED3);
    SysTick_Wait(XBEE_HOLD_PERIOD*4);
    if(info.num_blox_found == num_found)
      break;
    num_found = info.num_blox_found;
    while (allocating == TRUE) ; //Wait for the other program to PROG_QUERY
  }
  
  //Done finding nodes, start yourself.
  *(uint32_t *)ROLE_FLAG_LOC = 1;
  FS_SetAppFlag(1);
  *(uint32_t *)ROLE_FN_LOC = (uint32_t)info.roles[role_id].fn;
  NVIC_SystemReset();
  
  return ROLE_OK;
}

/**
 * @brief Returns the next role_id to be allocated
 * All the role have their minimum number required filled first in order. Then
 * the maxiumum number are filled in a fair manner by adding one to each role in
 * order.
 * @retval The role_id to allocate
 */
uint8_t Role_NextID(void) {
  static uint8_t cur_role = 0;
  static uint8_t needs_full = FALSE, wants_full = FALSE;
  
  if(needs_full == FALSE) {
    uint8_t tmp_cur = cur_role;
    do {
      if (info.roles[cur_role].num_allocated < info.roles[cur_role].min) {
        info.roles[cur_role].num_allocated++;
        return cur_role;
      }
      cur_role = (cur_role + 1) % info.num_roles;
    } while (tmp_cur != cur_role) ;
    needs_full = TRUE;
    return Role_NextID();
  } else {
    if (wants_full == TRUE)
      return 0; //ERROR CONDITION
    else {
      uint8_t tmp_cur = cur_role;
      uint8_t ret_role;
      do {
        if (info.roles[cur_role].num_allocated < info.roles[cur_role].max) {
          info.roles[cur_role].num_allocated++;
          ret_role = cur_role;
          cur_role = (cur_role + 1) % info.num_roles;
          return ret_role;
        }
        cur_role = (cur_role + 1) % info.num_roles;
      } while (tmp_cur != cur_role) ;
      wants_full = TRUE;
      return Role_NextID();
    }
  }
}
/** @} */

