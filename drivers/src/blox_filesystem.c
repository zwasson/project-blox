/**
 * @file    blox_filesystem.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   Provides a filesystem interface to a section of flash memory.
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

#include "blox_filesystem.h"
/**
 * @ingroup driver_filesystem
 * @{
 */

/**
 * @brief The internal pointer to the FAT
 */
volatile FS_Table *fat = 0;

/**
 * @brief Initializes the filesystem.
 * @retval FS_OK if successful, another FS_STATUS if not.
 */
FS_STATUS FS_Init(bool create) {  
  if(fat != 0)
    return FS_OK;  
  fat = (FS_Table *)MEM_FAT_START;
  //Create the file system if desired
  if(fat->magic != FS_MAGIC && create)
    FS_CreateFS();
  else
    return FS_BAD_FAT;
    
  return FS_ChkValid();
}

/**
 * @brief Creates a filesystem at the default location. Should
 * only be used once.
 * @retval FS_OK if successful, another FS_STATUS if not.
 */
FS_STATUS FS_CreateFS(void) {
  uint32_t i;
  FS_Table *fat_new = (FS_Table *)malloc(PAGE_SIZE);
  
  if(fat_new == NULL)
    return FS_CREATE_FAIL;

  fat_new->magic = FS_MAGIC;
  fat_new->numFiles = 0;
  fat_new->free_top = (uint32_t *)(MEM_STORE_START);
  fat_new->free_numPages = (MEM_STORE_SIZE/PAGE_SIZE);
  for(i = 0; i < FS_MAX_FILES; i++) {
    fat_new->table[i].id = FS_MAX_FILES;
    fat_new->table[i].numPages = 0;
    fat_new->table[i].data = 0;
  }
  
  fat = (FS_Table *)MEM_FAT_START;
  
  FS_SwapPage((uint32_t *)fat_new, (uint32_t *)fat);
  free(fat_new);  
  return FS_OK;
}

/**
 * Determine if the FAT is in a valid state.
 * @retval FS_OK if valid, FS_BAD_FAT on failure
 */
FS_STATUS FS_ChkValid(void) {	
  uint32_t i, real_free_top, real_free_numPages;
  if (fat->magic != FS_MAGIC) {
    Blox_DebugStr("FS_ChkValid failed! Bad magic.\r\n");  
	  return FS_BAD_FAT;
  }

  for(i = 0; i < fat->numFiles; i++) {
	  if(fat->table[i].id == FS_MAX_FILES) {
      Blox_DebugPat("FS_ChkValid failed! Bad uninitialized id: %u\r\n", fat->table[i].id);
      return FS_BAD_FAT;    
    }
  }

  if(fat->numFiles == 0) {
    if ((uint32_t)fat->free_top != MEM_STORE_START
          || fat->free_numPages != MEM_STORE_SIZE/PAGE_SIZE) {
      Blox_DebugStr("FS_ChkValid failed! Bad empty free_top or free_size.\r\n");
      return FS_BAD_FAT;
    }
  } else {
    real_free_top = (uint32_t)fat->table[fat->numFiles-1].data
                  + fat->table[fat->numFiles-1].numPages * PAGE_SIZE;
    real_free_numPages = (MEM_STORE_START + MEM_STORE_SIZE - real_free_top)/PAGE_SIZE;
    if ((uint32_t)fat->free_top != real_free_top
             || fat->free_numPages != real_free_numPages) {
      Blox_DebugStr("FS_ChkValid failed! Bad real free_top or free_numPages.\r\n");
      return FS_BAD_FAT;    
    }
  }
  return FS_OK;
}

/**
 * @brief Retrieves a file handle from the filesystem given a specific file id.
 * @param id the unique id of the file within the filesystem.
 * @retval The FS_File * of the file being requested. 0 on error  
 */
FS_File * FS_GetFile(uint8_t id) {
  if (fat == 0)
	  return 0;
  if (id > (fat->numFiles-1))
	  return 0;
  return (FS_File *)(fat->table+id);
}

/**
 * @brief  Gets a file in the filesystem by looking for the filename.
 * @param  name the name of the application.
 * @retval NULL if the file can't be found, a pointer otherwise.
 */
FS_File * FS_GetFileFromName(char *name) {
  uint8_t num_files = FS_GetNumFiles();
  uint8_t i;
  for (i = 0; i < num_files; i++) {
    FS_File *file;
    file = FS_GetFile(i);
    if (memcmp(name, file->name, FS_FILE_MAX_NAME_LEN) == 0)
      return file;
  }
  return NULL;
}

/**
 * @brief Returns the number of files being managed by the filesystem.
 * @retval The number of files being managed by the filesystem.
 */
uint8_t FS_GetNumFiles(void) { return fat->numFiles; } 

/**
 * @brief Deletes a file from the filesystem. Shifts data up so
 * 				fragmentation does not occur.
 * @param id the unique id of the file within the filesystem.
 * @retval An FS_STATUS indicating whether the delete was successful.
 */
FS_STATUS FS_DeleteFile(uint8_t id) {
  uint32_t i, j;
  FS_Table *fat_new;
  if (fat == 0)
	  return FS_FAT_NOT_INIT;
  if (id > (fat->numFiles-1))
	  return FS_FILE_NOT_INIT;
  fat_new = (FS_Table *)malloc(PAGE_SIZE);
  memmove(fat_new, (const void *)fat, PAGE_SIZE);
  
  if (fat_new->numFiles-1 == id) {
    fat_new->free_top = (uint32_t *)((uint32_t)(fat_new->free_top) - fat_new->table[id].numPages * PAGE_SIZE);
  } else {
    fat_new->free_top = fat_new->table[id].data;
    for (i = id; i < fat->numFiles-1; i++) {
      fat_new->table[i].numPages = fat_new->table[i+1].numPages;
      strcpy(fat_new->table[i].name, fat_new->table[i+1].name);      
      fat_new->table[i].data = fat_new->free_top;
      for (j = 0; j < fat_new->table[i].numPages; j++) {
        FS_SwapPage((uint32_t *)((uint32_t)(fat_new->table[i+1].data)+ j*PAGE_SIZE),
                    (uint32_t *)((uint32_t)(fat_new->table[i].data)  + j*PAGE_SIZE));
      }
      fat_new->free_top = (uint32_t *)((uint32_t)(fat_new->table[i].data) + fat_new->table[i].numPages * PAGE_SIZE);
    }
  }

  fat_new->free_numPages = (MEM_STORE_START+MEM_STORE_SIZE-(uint32_t)(fat_new->free_top))/PAGE_SIZE;
  fat_new->numFiles--;
  fat_new->table[fat_new->numFiles].id = FS_MAX_FILES;
  
  FS_SwapPage((uint32_t *)fat_new, (uint32_t *)fat);
  free(fat_new);

  return FS_ChkValid();
}

/**
 * @brief  Creates a new file of a given size in the filesystem.
 * @param  name the name of the new file
 * @param  numPages the number of pages the new file needs
 * @retval The unique id of the file within the filesystem. FS_MAX_FILES on error.
 */
uint8_t FS_CreateFile(char *name, uint8_t numPages) {
uint8_t new_id;
FS_Table *new_fat;
  Blox_DebugStr("Creating a file!\r\n");
  if (fat == 0)
    return FS_MAX_FILES;
  if (fat->numFiles == FS_MAX_FILES || strlen(name) > FS_FILE_MAX_NAME_LEN-1)
    return FS_MAX_FILES;
  if (fat->free_numPages < numPages || numPages == 0)
    return FS_MAX_FILES;
  new_fat = (FS_Table *)malloc(PAGE_SIZE);
  memmove(new_fat, (const void *)fat, PAGE_SIZE);
  new_id = new_fat->numFiles++;
  new_fat->table[new_id].id = new_id;
  strcpy(new_fat->table[new_id].name, name);
  new_fat->table[new_id].numPages = numPages;
  new_fat->table[new_id].data = new_fat->free_top;
  new_fat->free_top = (uint32_t *)((uint32_t)new_fat->free_top + new_fat->table[new_id].numPages * PAGE_SIZE);
  new_fat->free_numPages -= new_fat->table[new_id].numPages; 
  
  FS_SwapPage((uint32_t *)new_fat, (uint32_t *)fat);
  free(new_fat);
  
  Blox_DebugStr("Created a new file!\r\n");

  if(FS_ChkValid() != FS_OK) {
    Blox_DebugStr("FS_CreateFile ChkValid failed!\r\n");
    return FS_MAX_FILES;
  }

  return new_id;
}

/**
 * @brief  Writes data into a file in the filesystem at a given offset.
 * @param  id the unique id of the file within the filesystem
 * @param  data a pointer to the buffer to be copied
 * @param  page_offset the offset within the file the buffer should be copied to.
 * @retval An FS_STATUS denoting whether the write was successful.
 */
FS_STATUS FS_WriteFilePage(uint8_t id, uint32_t *data, uint32_t page_offset) {
  if(page_offset > fat->table[id].numPages-1)
    return FS_BAD_WRITE;
  FS_SwapPage(data, (uint32_t *)((uint32_t)(fat->table[id].data) + page_offset*PAGE_SIZE));
  return FS_OK;
}

/**
 * @brief  Swaps a page in RAM with a page in Flash.
 * @param  src the address of the start of the page in RAM
 * @param  dst the address of the start of the page in Flash
 * @retval None.
 */
void FS_SwapPage(uint32_t *src, uint32_t *dst) {
int i;
  FLASH_Unlock();
  FLASH_ErasePage((uint32_t)dst);
  for(i = 0; i < PAGE_SIZE/WORD_SIZE; i++)
    FLASH_ProgramWord((uint32_t)(dst++), src[i]);
}

/**
 * @brief  Rounds a size up to the next multiple of PAGE_SIZE.
 * @param  size the size to be rounded.
 * @retval the rounded size.
 */
uint32_t FS_RoundPageUp(uint32_t size) {
  if((uint32_t)size % PAGE_SIZE)
    return (uint32_t)size / PAGE_SIZE + 1;
  else
    return (uint32_t)size / PAGE_SIZE;
}

/**
 * @brief  De-initializes the system and runs the application stored at the file id.
 * @param  file_id the id of the file to be run.
 * @retval None.
 */
void FS_RunFile(uint8_t file_id) {
  uint32_t i;
  FS_File *file = FS_GetFile(file_id);
  //Swap to staging area
  for (i = 0; i < file->numPages; i++) {
    FS_SwapPage((uint32_t *)((char *)(file->data)+i*PAGE_SIZE),
                (uint32_t *)((char *)MEM_STAGE_START+i*PAGE_SIZE));
  }

  FS_SetAppFlag(1);
  NVIC_SystemReset();
}

/**
 * @brief  Runs the application stored in the staging area. Assumes from a system reset.
 * @retval None.
 */
void FS_RunStage(void) {
  uint32_t app_addr;
  volatile uint32_t jump_addr;
  void (*app_fn)(void);
  //Deinit everything, set up vector/stack, and jump  
  app_addr = MEM_STAGE_START;  
  jump_addr = *(__IO uint32_t *) (app_addr+4);
  app_fn = (void (*)(void))jump_addr;  

  FS_SetAppFlag(0);
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, app_addr);
  __set_MSP(*(__IO uint32_t*)app_addr);
  app_fn();
    
  while (1) ; //Shouldn't get here
}

/**
 * @brief  Returns the flag designating if an application should be run.
 * @retval The flag.
 */
uint8_t FS_GetAppFlag(void) {
  return *(uint8_t *)FS_APP_FLAG_LOC;
}

/**
 * @brief  Sets the flag desinating if an application should be run.
 * @param  val The flag.
 * @retval None.
 */
void FS_SetAppFlag(uint8_t val) {
  *(uint8_t *)FS_APP_FLAG_LOC = val;
}
/** @} */
