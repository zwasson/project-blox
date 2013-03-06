/**
 * @file    blox_filesystem.h
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   Contains function prototypes for the filesystem interface.
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

#ifndef __BLOX_FILESYSTEM_H
#define __BLOX_FILESYSTEM_H

#include "blox_system.h"
#include "stm32f10x_flash.h"
#include "misc.h"

#include "string.h"

/**
 * @ingroup driver_filesystem
 * @{
 */

/**
 * @brief Enum containing the status of the filesystem.
 */
typedef enum {
  FS_CREATE_FAIL = -6,
  FS_BAD_WRITE,
	FS_FULL,
	FS_FILE_NOT_INIT,
	FS_FAT_NOT_INIT,
	FS_BAD_FAT,
  FS_OK
} FS_STATUS;

/**
 * For our processor, Flash goes from 0x08000000 - 0x08080000, 512K 
 * 512K total, 2K page, 4K sector size
 */
#define FS_MAX_FILES 16
#define FS_MAGIC 0xdeadbeef
#define FS_FILE_MAX_NAME_LEN 32
#define FS_APP_FLAG_LOC 0x20005000

/**
 * @brief Defines a file's header. Contains the id of the file
 * 				within the FAT, the size in bytes of the file, and
 * 				a pointer to the data.
 */
typedef struct {
	uint8_t id;                       /**< The unique identifer of the file in the fs */
  char name[FS_FILE_MAX_NAME_LEN];  /**< a string name for the file */
	uint8_t numPages;                 /**< the number of pages the application takes up */
	uint32_t *data;                   /**< a pointer to the data of the file */
} FS_File;

/**
 * @brief The table of all FS_Files.
 */
typedef struct {
  volatile uint32_t magic;            /**< a constant to validate the FS */
	volatile uint32_t numFiles;         /**< the number of files in the FS */
	uint32_t *free_top;                 /**< the top of the free memory area */
  uint32_t free_numPages;             /**< the number of free pages left */
	FS_File table[FS_MAX_FILES];	      /**< the table of FS_Files */
} FS_Table;


FS_STATUS FS_Init(uint8_t create);
FS_STATUS FS_ChkValid(void);
FS_File * FS_GetFile(uint8_t id);
FS_File * FS_GetFileFromName(char *name);
uint8_t FS_GetNumFiles(void);
FS_STATUS FS_DeleteFile(uint8_t id);
uint8_t FS_CreateFile(char *name, uint8_t numPages);
FS_STATUS FS_WriteFilePage(uint8_t id, uint32_t *data, uint32_t page_offset);
FS_STATUS FS_CreateFS(void);
void FS_SwapPage(uint32_t *src, uint32_t *dst);
uint32_t FS_RoundPageUp(uint32_t size);
void FS_RunFile(uint8_t file_id);
void FS_RunStage(void);
uint8_t FS_GetAppFlag(void);
void FS_SetAppFlag(uint8_t val);
/** @} */
#endif
