/**
 * @file    blox_base_ui.h
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/21/2010
 * @brief   Function prototypes for the base program user interface
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

#ifndef __BLOX_BASE_UI_H
#define __BLOX_BASE_UI_H

#include "string.h"
#include "stdio.h"
#include "blox_oled.h"
#include "blox_filesystem.h"

/**
 * @ingroup base_ui
 * @{
 */
#define CENTER_TEXT(text, width) (64-width*TITLE_SIZE*strlen(text)/2)

//header
#define HEADER_BACKGROUND_COLOR       COLOR_BLUE
#define HEADER_TEXT_COLOR             COLOR_WHITE
#define HEADER_HEIGHT                 12
#define HEADER_TITLE_LOCATION_X       3
#define HEADER_TITLE_LOCATION_Y       2
#define HEADER_VERSION_LOCATION_X     96
#define HEADER_VERSION_LOCATION_Y     2

//title
#define TITLE_SIZE                    1
#define TITLE_LOCATION_Y              19
#define TITLE_COLOR                   COLOR_WHITE
#define TITLE_LINE_X_START            15
#define TITLE_LINE_X_END              127-(TITLE_LINE_X_START+1)
#define TITLE_LINE_Y                  35
#define TITLE_LINE_COLOR              COLOR_BLUE

//entry
#define ENTRY_START_LOCATION          43
#define ENTRY_LOCATION_OFFSET         14
#define MAX_ENTRIES                   5
#define ENTRY_COLOR                   COLOR_WHITE
#define ENTRY_SELECTED_COLOR          COLOR_YELLOW
#define MAX_ENTRY_LENGTH              20

//footer
#define FOOTER_TEXT                   "Project Blox"
#define FOOTER_BACKGROUND_COLOR       COLOR_BLUE
#define FOOTER_TEXT_COLOR             COLOR_WHITE
#define FOOTER_HEIGHT                 12
#define FOOTER_COPYRIGHT_LOCATION_X   CENTER_TEXT(FOOTER_TEXT, 6)
#define FOOTER_COPYRIGHT_LOCATION_Y   117

void Blox_UI_DrawHeader(void);
void Blox_UI_DrawFooter(void);
void Blox_UI_DrawTitle(char *title);
void Blox_UI_SetEntries(char **entries, ptrVoidFn *entries_handler, 
                        ptrVoidFn BackHandler, uint8_t numEntries);
void Blox_UI_DrawEntries(void);
void Blox_UI_SelectEntry(char *entry);
void Blox_UI_SelectEntryAbove(void);
void Blox_UI_SelectEntryBelow(void);
void Blox_UI_RunEntry(void);
uint8_t Blox_UI_GetEntryID(void);
void Blox_UI_Back(void);
void Blox_UI_ClearScreen(void);
/** @} */
#endif
