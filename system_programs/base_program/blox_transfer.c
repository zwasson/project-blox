/**
 * @file    blox_transfer.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   This module facilitates two systems to manage applications
            on each other.
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

#include "blox_transfer.h"

/**
 * @ingroup base_transfer
 * @{
 */
/* Private function prototypes */
TRANSFER_STATUS Cmd_RCV_APP(void);
TRANSFER_STATUS Cmd_DEL_APP(void);
TRANSFER_STATUS Cmd_LST_APPS(void);
TRANSFER_STATUS Cmd_RUN_APP(void);

/**
 * @brief Initializes the transfer module.
 * @retval TRANSFER_OK if successful, another TRANSFER_STATUS if not.
 */
void Transfer_Init(void) {
  USB_Init();
  FS_Init(1);
}

/**
 * @brief Run in slave mode accepting and processing commands.
 */
void Transfer_Slave(void) {
  uint8_t checksum;
  TRANSFER_OPCODE opcode;
  
  while (1) {
    opcode = (TRANSFER_OPCODE)USB_Receive();
    checksum = USB_Receive();
    if ((checksum+opcode) != 0xFF) {
      USB_Send(TRANSFER_NAK);
      continue;
    }
    USB_Send(TRANSFER_ACK);

    switch(opcode) {
    case RCV_APP:
      Cmd_RCV_APP();
      break;
	  case DEL_APP:
      Cmd_DEL_APP();
      break;
	  case LST_APPS:
      Cmd_LST_APPS();
      break;
	  case RUN_APP:
      Cmd_RUN_APP();
      break;
    }
  }
}

/**
 * @brief Receives an application from a sender and stores it in the fs.
 * @retval TRANSFER_OK if the receive and application store succeed.
 *         -TRANSFER_CMD_FAIL if the receive or store fail.
 */
TRANSFER_STATUS Cmd_RCV_APP(void) {
  uint8_t checksum, name_len, id, numPages, *page;
  uint16_t i, j;
  char name[FS_FILE_MAX_NAME_LEN];  
  uint32_t size, remaining, read_amt;    
  /*** Get # of characters in filename ***/
  checksum = 0;
  name_len = USB_Receive();
  checksum = USB_Receive();
  if((checksum+name_len) != 0xFF || name_len > FS_FILE_MAX_NAME_LEN-1) {
    USB_Send(TRANSFER_NAK);
    return TRANSFER_CMD_FAIL;
  }
  USB_Send(TRANSFER_ACK);
  /*** Get filename ***/
  checksum = 0;
  for(i = 0; i < name_len; i++) {
    name[i] = USB_Receive();
    checksum += name[i];
  }
  name[i] = '\0';
  checksum += USB_Receive();
  if(checksum != 0xFF) {
    USB_Send(TRANSFER_NAK);
    return TRANSFER_CMD_FAIL;
  }
  USB_Send(TRANSFER_ACK);
     
  /*** Get file size in pages ***/
  checksum = 0;
  size = USB_Receive() 
       | (USB_Receive() << 8)
       | (USB_Receive() << 16)
       | (USB_Receive() << 24);
  checksum += size & 0xFF;
  checksum += (size >> 8) & 0xFF;
  checksum += (size >> 16) & 0xFF;
  checksum += (size >> 24) & 0xFF;
  checksum += USB_Receive();
  
  numPages = FS_RoundPageUp(size); 
  id = FS_CreateFile(name, numPages);
  if (checksum != 0xFF || id == FS_MAX_FILES) {
    USB_Send(TRANSFER_NAK);
    return TRANSFER_CMD_FAIL;
  }
  USB_Send(TRANSFER_ACK);
  
  /*** Receive pages 1 at a time ***/
  remaining = size;  
  page = (uint8_t *)malloc(PAGE_SIZE);  
  for(i = 0; i < numPages; i++) {
    memset(page, 0, PAGE_SIZE); //Zero for funs
    checksum = 0;
    if(remaining < PAGE_SIZE)
      read_amt = remaining; //Don't read too much on the last page
    else
      read_amt = PAGE_SIZE; 
    for(j = 0; j < read_amt; j++) {
      page[j] = USB_Receive();
      checksum += page[j];
    }
    checksum += USB_Receive();
    FS_WriteFilePage(id, (uint32_t *)page, i);
    if (checksum != 0xFF || id == FS_MAX_FILES) {
      USB_Send(TRANSFER_NAK);
      FS_DeleteFile(id);
      free(page);
      return TRANSFER_CMD_FAIL;
    }
    USB_Send(TRANSFER_ACK);        
    remaining -= PAGE_SIZE;
  }
  free(page);
  return TRANSFER_OK;
}

/**
 * @brief Receives an app. id from a sender and removes it from the fs.
 * @retval TRANSFER_OK if the receive and delete succeed.
 *         -TRANSFER_CMD_FAIL if the receive or delete fail.
 */
TRANSFER_STATUS Cmd_DEL_APP(void) {
uint8_t checksum, id;
  /*** Receive file id ***/
  id = USB_Receive();
  checksum = id;
  checksum += USB_Receive();
  if(checksum != 0xFF 
     || id >= FS_GetNumFiles()
     || FS_DeleteFile(id) != FS_OK) {
    USB_Send(TRANSFER_NAK);
    return TRANSFER_CMD_FAIL;
  }
  USB_Send(TRANSFER_ACK);  

  return TRANSFER_OK;
}

/**
 * @brief Sends a command with headers for all the applications in the fs.
 * @retval TRANSFER_OK if the send succeeds.
 *         -TRANSFER_CMD_FAIL if the send fails.
 */
TRANSFER_STATUS Cmd_LST_APPS(void) {
  uint8_t checksum, numFiles, i, j;
  FS_File *file;
  /*** Send number of files in FS ***/
  numFiles = FS_GetNumFiles();
  checksum = 0xFF - numFiles;
  USB_Send(numFiles);
  USB_Send(checksum);
  if (USB_Receive() == TRANSFER_NAK)
    return TRANSFER_CMD_FAIL;
  /*** Send files 1 at a time ***/
  for (i = 0; i < numFiles; i++) {
    file = FS_GetFile(i);
    checksum = 0;
    USB_Send(file->id);
    checksum += file->id;
    for (j = 0; j < FS_FILE_MAX_NAME_LEN; j++) {
      USB_Send(file->name[j]);
      checksum += file->name[j];      
    }
    checksum += file->numPages;    
    USB_Send(file->numPages);
    USB_Send(0xFF-checksum);
    
    if (USB_Receive() != TRANSFER_ACK)
      return TRANSFER_CMD_FAIL;
  }
  
  return TRANSFER_OK;
}

/**
 * @brief Receives an app. id, retrieves it from the fs, and runs the app.
 * @retval Doesn't return on success. -TRANSFER_CMD_FAIL on failure.
 */
TRANSFER_STATUS Cmd_RUN_APP(void) {
  uint8_t checksum, id;
  
  /*** Receive file id ***/
  id = USB_Receive();
  checksum = id;
  checksum += USB_Receive();
  if(checksum != 0xFF 
     || id >= FS_GetNumFiles()) {
    USB_Send(TRANSFER_NAK);
    return TRANSFER_CMD_FAIL;
  }
  USB_Send(TRANSFER_ACK);  
  
  //Run the application in the file
  FS_RunFile(id);
  
  return TRANSFER_CMD_FAIL; //Shouldn't ever get here.
}
/** @} */
