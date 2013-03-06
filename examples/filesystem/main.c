/********************************************************************************
 * @file    filesystem.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/27/2010
 * @brief Tests the filesystem driver by performing read/writes on the filesystem.  
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
 
#include "string.h" //strcpy
#include "stm32f10x.h"
#include "blox_filesystem.h"
#include "blox_usb.h"

#define MKFS 1

int main(void)
{
  uint32_t *page;
  USB_Init();
  
  Blox_DebugStr("Testing!!!\r\n");
  FS_Init(1);
  FS_CreateFS();
   
  FS_CreateFile("deepyaman", 2);
  FS_CreateFile("jackie's prog", 1);
  FS_CreateFile("zach's program", 3);
  FS_CreateFile("jesse", 1);
  
  page = (uint32_t *)malloc(PAGE_SIZE);
  memset(page, 0, PAGE_SIZE);
  page[0] = 0;
  page[1] = 0;
  FS_WriteFilePage(0, page, 0);
  page[0] = 0;
  page[1] = 1;
  FS_WriteFilePage(0, page, 1);
  
  page[0] = 1;
  page[1] = 0;
  FS_WriteFilePage(1, page, 0);
  
  page[0] = 2;
  page[1] = 0;
  FS_WriteFilePage(2, page, 0);
  page[0] = 2;
  page[1] = 1;
  FS_WriteFilePage(2, page, 1);
  page[0] = 2;
  page[1] = 2;
  FS_WriteFilePage(2, page, 2);
  
  page[0] = 3;
  page[1] = 0;
  FS_WriteFilePage(3, page, 0);
  free(page);
  
  FS_DeleteFile(1);
  FS_DeleteFile(2);

  while (1);
}
