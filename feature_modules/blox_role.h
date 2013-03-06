/**
 * @file    blox_system.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    11/03/2010
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

#ifndef __BLOX_ROLES_H
#define __BLOX_ROLES_H

#include "blox_system.h"
//#include "blox_transfer.h"
#include "blox_filesystem.h"
#include "blox_xbee.h"
#include "blox_debug.h"

/**
 * @ingroup feature_role
 * @{ 
 */
#define ROLE_MAX 64
#define ROLE_INF 255

/**
 * @brief Status to return on Role functions
 */
typedef enum {
  ROLE_RUN_FAIL = -3,
  ROLE_ADD_FAIL,
  ROLE_OOM,
  ROLE_OK
} ROLE_STATUS;

/**
 * @brief Struct that represents a role.
 */
typedef struct {
  uint8_t min;              /**< The minimum number of Blox needed of this role */
  uint8_t max;              /**< The maximum number of Blox desired of this role */
  ptrVoidFn fn;             /**< The function to call for this role */
  uint8_t num_allocated;    /**< The number of Blox allocated so far of this role */
} Role;

/**
 * @brief Struct with all role information
 */
typedef struct {
  char name[FS_FILE_MAX_NAME_LEN];  /**< The name of the program as a string */
  uint8_t name_len;                 /**< The length of the name */
  uint8_t num_roles;                /**< The number of Roles */
  Role roles[ROLE_MAX];             /**< An array of Roles  */
  uint8_t num_blox_found;           /**< The number of blox with the application foun so far */
  uint8_t num_blox_started;         /**< The number of blox that have stated this application so far */
  uint8_t num_needed;               /**< The total number of Blox needed */
  uint8_t num_wanted;               /**< The total number of Blox wanted */
  uint32_t blox_found[ROLE_MAX];    /**< An array of the Blox_Ids found */
} RoleInfo;

/**
 * @brief An enum of the possible RoleFrame opcodes
 */
typedef enum {
  EMPTY,
  PROG_QUERY,
  PROG_ACK,
  PROG_START,
  PARENT_QUERY,
  PARENT_ACK
} RoleFrameOps;

/**
 * @brief Role-level frame passed into XBee
 */
typedef struct {
  uint8_t opcode;                         /**< the RoleFrameOps value for the desired opcode */
  uint8_t data [BLOX_FRAME_DATA_LEN-1];   /**< the raw data being sent */
} RoleFrame;

/**
 * @brief App-layer frame for a [PARENT|PROG]_QUERY
 */
typedef struct {
  char name[FS_FILE_MAX_NAME_LEN];  /**< The name of the program being queried about */
} QueryFrame;

/**
 * @brief App-layer frame for a PARENT_ACK
 */
typedef struct {
  uint8_t role_id;    /**< The role_id this child should run as */
} ParentAckFrame;

ROLE_STATUS Blox_Role_Init(char *name, uint8_t len);
ROLE_STATUS Blox_Role_Add(ptrVoidFn fn, uint8_t min, uint8_t max);
ROLE_STATUS Blox_Role_Run(void);
/** @} */
#endif
