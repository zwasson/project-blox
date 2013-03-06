/**
 * @file    blox_base_ui.c 
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/21/2010
 * @brief   Functions used to create the base program UI
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

#include "blox_base_ui.h"

/**
 * @ingroup base_ui
 * @{
 */
void Blox_UI_DrawEntry(char *entry, uint8_t index, uint16_t color);

char entry_names[MAX_ENTRIES][FS_FILE_MAX_NAME_LEN];
ptrVoidFn entry_handlers[MAX_ENTRIES];
ptrVoidFn back_handler;
uint8_t selected_entry;
uint8_t total_entries;

/**
 * @brief Draws the header for the UI
 * @retval None
 */
void Blox_UI_DrawHeader(void) {
  char buffer[13];
  Blox_OLED_DrawRectangle(0, 0, 127, HEADER_HEIGHT, HEADER_BACKGROUND_COLOR);
  Blox_OLED_DrawStringGraphics(HEADER_TITLE_LOCATION_X, HEADER_TITLE_LOCATION_Y, OLED_FONT_5X7,
                               HEADER_TEXT_COLOR, 1, 1, "Base Program");
  sprintf(buffer, "ID: %.1d", Blox_System_GetId());
  Blox_OLED_DrawStringGraphics(HEADER_VERSION_LOCATION_X, HEADER_VERSION_LOCATION_Y, OLED_FONT_5X7,
                               HEADER_TEXT_COLOR, 1, 1, buffer);
}

/**
 * @brief Draws the footer for the UI
 * @retval None
 */
void Blox_UI_DrawFooter(void) {
  Blox_OLED_DrawRectangle(0, 127-FOOTER_HEIGHT, 127, 127, FOOTER_BACKGROUND_COLOR);
  Blox_OLED_DrawStringGraphics(FOOTER_COPYRIGHT_LOCATION_X, FOOTER_COPYRIGHT_LOCATION_Y, OLED_FONT_5X7,
                               FOOTER_TEXT_COLOR, 1, 1, FOOTER_TEXT);
}

/**
 * @brief Draws the title for the UI
 * @param title The title text to be displayed
 * @retval None
 */
void Blox_UI_DrawTitle(char *title) {
  Blox_OLED_DrawStringGraphics(CENTER_TEXT(title, 8), TITLE_LOCATION_Y, OLED_FONT_8X12,
                               TITLE_COLOR, TITLE_SIZE, TITLE_SIZE, title);
  Blox_OLED_DrawLine(TITLE_LINE_X_START, TITLE_LINE_Y, TITLE_LINE_X_END, TITLE_LINE_Y, TITLE_LINE_COLOR);
}

/**
 * @brief Sets the entries for the UI page
 * @param entries The names of entries for the UI page
 * @param entries_handler The handler functions for the entries
 * @param BackHandler The handler function for going back a page
 * @param numEntries The number of entries for the UI page
 * @retval None
 */
void Blox_UI_SetEntries(char **entries, ptrVoidFn *entries_handler, ptrVoidFn BackHandler, uint8_t numEntries) {
  uint8_t i;
  for(i = 0; i < numEntries; i++) {
    strcpy(entry_names[i], entries[i]);
    entry_handlers[i] = entries_handler[i];
  }
  total_entries = numEntries;
  selected_entry = 0;
  back_handler = BackHandler;
}

/**
 * @brief Draws the entries for the UI page
 * @retval None
 */
void Blox_UI_DrawEntries(void) {
  uint8_t i;
  for(i = 0; i < total_entries; i++) {
    Blox_UI_DrawEntry(entry_names[i], i, ENTRY_COLOR);
  }
  Blox_UI_SelectEntry(entry_names[selected_entry]);
}

/**
 * @brief Draws a single entry
 * @param entry The entry to be displayed
 * @param index The index of the entry in order to offset the text
 * @param color The color of the entry text
 * @retval None
 */
void Blox_UI_DrawEntry(char *entry, uint8_t index, uint16_t color) {
  if(strlen(entry) > MAX_ENTRY_LENGTH) {
    char buffer[MAX_ENTRY_LENGTH+1];
    strncpy(buffer, entry, MAX_ENTRY_LENGTH-3);
    buffer[MAX_ENTRY_LENGTH-3] = '.';
    buffer[MAX_ENTRY_LENGTH-2] = '.';
    buffer[MAX_ENTRY_LENGTH-1] = '.';
    buffer[MAX_ENTRY_LENGTH] = 0;
    Blox_OLED_DrawStringGraphics(CENTER_TEXT(buffer, 6), ENTRY_START_LOCATION+index*ENTRY_LOCATION_OFFSET,
                                 OLED_FONT_5X7, color, 1, 1, buffer);
  } else {
    Blox_OLED_DrawStringGraphics(CENTER_TEXT(entry, 6), ENTRY_START_LOCATION+index*ENTRY_LOCATION_OFFSET,
                                 OLED_FONT_5X7, color, 1, 1, entry);
  }
}

/**
 * @brief Selects an entry for the UI
 * @param entry The entry to be selected
 * @retval None
 */
void Blox_UI_SelectEntry(char *entry) {
  uint8_t i;
  for(i = 0; i < total_entries; i++) {
    if(strcmp(entry_names[i], entry) == 0) {
      Blox_UI_DrawEntry(entry_names[selected_entry], selected_entry, ENTRY_COLOR);
      Blox_UI_DrawEntry(entry, i, ENTRY_SELECTED_COLOR); 
      selected_entry = i;
    }
  }
}

/**
 * @brief Selects the entry above the currently selected one
 * @retval None
 */
void Blox_UI_SelectEntryAbove(void) {
  if(total_entries == 0 || total_entries == 1)
    return;
  Blox_UI_DrawEntry(entry_names[selected_entry], selected_entry, ENTRY_COLOR);
  if(selected_entry == 0)
    selected_entry = total_entries - 1;
  else
    selected_entry = selected_entry - 1;
  Blox_UI_DrawEntry(entry_names[selected_entry], selected_entry, ENTRY_SELECTED_COLOR); 
}

/**
 * @brief Selects the entry below the currently selected one
 * @retval None
 */
void Blox_UI_SelectEntryBelow(void) {
  if(total_entries == 0 || total_entries == 1)
    return;
  Blox_UI_DrawEntry(entry_names[selected_entry], selected_entry, ENTRY_COLOR);
  if(selected_entry == total_entries - 1)
    selected_entry = 0;
  else
    selected_entry = selected_entry + 1;
  Blox_UI_DrawEntry(entry_names[selected_entry], selected_entry, ENTRY_SELECTED_COLOR); 
}

/**
 * @brief Runs the currently selected entry
 * @retval None
 */
void Blox_UI_RunEntry(void) {
  if(total_entries != 0 && entry_handlers[selected_entry] != NULL) {
    (*entry_handlers[selected_entry])();
  }
}

/**
 * @brief Gets the id of the selected entry
 * @retval The id of the currently selected entry
 */
uint8_t Blox_UI_GetEntryID(void) {
  return selected_entry;
}

/**
 * @brief Calls the Back function for the UI page
 * @retval None
 */
void Blox_UI_Back(void) {
  if(back_handler != NULL) {
    (*back_handler)();
  }
}

/**
 * @brief Clears the current UI page
 * @retval None
 */
void Blox_UI_ClearScreen(void) {
  total_entries = 0;
  Blox_OLED_Clear();
}
/** @} */
