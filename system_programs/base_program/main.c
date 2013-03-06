/**
 * @file    base_program.c
 * @author  Jesse Tannahill/Zach Wasson
 * @version V0.2
 * @date    10/18/2010
 * @brief   Provides a filesystem interface to a section of flash memory.
 *  The base program sits on a Blox and is the only program that runs at
 *  startup. The user interface is largely interrupt-driven, responding
 *  to stimulus from the touchpanels until a user wants to a perform an
 *  action. Main actions are to load new programs via the Transfer driver
 *  and running a program that is already loaded in flash via the Flash
 *  driver.
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
#include "blox_transfer.h"
#include "blox_usb.h"
#include "blox_xbee.h"
#include "blox_filesystem.h"
#include "blox_role.h"
#include "blox_led.h"
#include "blox_base_ui.h"
#include "blox_gesture.h"
#include "string.h"
#include "stdio.h"

/**
 * @ingroup base_program
 * @{
 */
#define MAIN_MENU_NUM_ENTRIES     4

#define TEXT_START_LOCATION          43
#define TEXT_LOCATION_LEFT_ALIGNED   16
#define TEXT_LOCATION_OFFSET         14

void Base_RX(BloxFrame *frame);

void Base_UI_MainMenu(void);
void Base_UI_ApplicationsMenu(void);
void Base_UI_CalibrationMenu(void);
void Base_UI_SysInfoMenu(void);
void Base_UI_USBMenu(void);
void Base_UI_Loading(void);

/**
 * @brief Provides a GUI for the base program.
 *        The base program allows users to select applications
 *        to run, calibrate system variables, receive USB commands,
 *        and view system info.
 * @retval None
 */
int main(void)
{
  if (FS_GetAppFlag() == 1) {
    FS_RunStage();
  }

  Blox_System_Init();
  FS_Init(1);
  Blox_LED_Init();
  Blox_XBee_Init();
  Blox_XBee_Register_RX_IRQ(&Base_RX);
  Blox_XBee_Enable_RX_IRQ();
  SysTick_Init();
  USB_Init();
  Blox_OLED_Init();
  Blox_Gesture_Init();
  
  Base_UI_MainMenu();

  while(1) {
    static uint32_t old_north_time, old_south_time, old_east_time, old_west_time;

    if(Blox_Gesture_GetGesture(TOUCH_NORTH_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_NORTH_ID) != old_north_time) {
      Blox_UI_SelectEntryAbove();
      old_north_time = Blox_Gesture_GetGestureTime(TOUCH_NORTH_ID);
      Blox_LED_Toggle(LED_NORTH);
    }
    if(Blox_Gesture_GetGesture(TOUCH_SOUTH_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_SOUTH_ID) != old_south_time) {
      Blox_UI_SelectEntryBelow();
      old_south_time = Blox_Gesture_GetGestureTime(TOUCH_SOUTH_ID);
      Blox_LED_Toggle(LED_SOUTH);
    }
    if(Blox_Gesture_GetGesture(TOUCH_EAST_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_EAST_ID) != old_east_time) {
      Blox_UI_RunEntry();
      old_east_time = Blox_Gesture_GetGestureTime(TOUCH_EAST_ID);
      Blox_LED_Toggle(LED_EAST);
    }
    if(Blox_Gesture_GetGesture(TOUCH_WEST_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_WEST_ID) != old_west_time) {
      Blox_UI_Back();
      old_west_time = Blox_Gesture_GetGestureTime(TOUCH_WEST_ID);
      Blox_LED_Toggle(LED_WEST);
    }
  }
}

/**
 * @brief Draws the main menu
 * @retval None
 */
void Base_UI_MainMenu(void) {
  char *entries[MAIN_MENU_NUM_ENTRIES];
  ptrVoidFn entry_handlers[MAIN_MENU_NUM_ENTRIES];
  entries[0] = "Applications";
  entries[1] = "Calibration";
  entries[2] = "System Info";
  entries[3] = "USB Mode";
  
  entry_handlers[0] = Base_UI_ApplicationsMenu;
  entry_handlers[1] = Base_UI_CalibrationMenu;
  entry_handlers[2] = Base_UI_SysInfoMenu;
  entry_handlers[3] = Base_UI_USBMenu;
  
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_UI_DrawTitle("MAIN MENU");
  
  Blox_UI_SetEntries(entries, entry_handlers, NULL, MAIN_MENU_NUM_ENTRIES);
  Blox_UI_DrawEntries();
}

/**
 * @brief Handler for calling applications using blox_base_ui
 * @retval None
 */
void Application_Handler(void) {
  Base_UI_Loading();
  FS_RunFile(Blox_UI_GetEntryID());
}

/**
 * @brief Draws the applications menu
 * @retval None
 */
void Base_UI_ApplicationsMenu(void) {
  uint8_t applications_num_entries = 0;
  char *entries[MAX_ENTRIES];
  ptrVoidFn entry_handlers[MAX_ENTRIES];
  uint8_t i;
  
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_UI_DrawTitle("APPLICATIONS");
  
  if((applications_num_entries = FS_GetNumFiles()) > 5)
    applications_num_entries = 5;
    
  for(i = 0; i < applications_num_entries; i++) {
    FS_File *file = FS_GetFile(i);
    entries[i] = file->name;
    entry_handlers[i] = Application_Handler;
  }
  
  Blox_UI_SetEntries(entries, entry_handlers, &Base_UI_MainMenu, applications_num_entries);
  Blox_UI_DrawEntries();  
}

/**
 * @brief Draws the calibration menu
 * @retval None
 */
void Base_UI_CalibrationMenu(void) {
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_UI_DrawTitle("CALIBRATION");
  
  Blox_UI_SetEntries(NULL, NULL, &Base_UI_MainMenu, 0);
}

/**
 * @brief Draws the system info menu. The system info menu displays
 *        the ID of the Blox, the current number of programs, and the 
          amount of free and used space.
 * @retval None
 */
void Base_UI_SysInfoMenu(void) {
  uint8_t numFiles;
  uint32_t freeSpace = 0;
  uint32_t usedSpace = 0;
  char buffer[20];
  uint8_t i;
  
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_UI_DrawTitle("SYSTEM INFO");
  
  Blox_UI_SetEntries(NULL, NULL, &Base_UI_MainMenu, 0);
  
  sprintf(buffer, "Blox ID: %.1d", Blox_System_GetId());
  Blox_OLED_DrawStringGraphics(TEXT_LOCATION_LEFT_ALIGNED, TEXT_START_LOCATION, OLED_FONT_5X7,
                               COLOR_WHITE, 1, 1, buffer);
  
  numFiles = FS_GetNumFiles();                            
  sprintf(buffer, "Num Apps: %.1d", numFiles);
  Blox_OLED_DrawStringGraphics(TEXT_LOCATION_LEFT_ALIGNED, TEXT_START_LOCATION + TEXT_LOCATION_OFFSET,
                               OLED_FONT_5X7, COLOR_WHITE, 1, 1, buffer);
  
  for(i = 0; i < numFiles; i++) {
    FS_File *file = FS_GetFile(i);
    usedSpace += file->numPages;
  }                           
  sprintf(buffer, "Used Space: %d", usedSpace);
  Blox_OLED_DrawStringGraphics(TEXT_LOCATION_LEFT_ALIGNED, TEXT_START_LOCATION + 2*TEXT_LOCATION_OFFSET,
                               OLED_FONT_5X7, COLOR_WHITE, 1, 1, buffer);
  freeSpace = (MEM_STORE_SIZE/PAGE_SIZE) - usedSpace;
  sprintf(buffer, "Free Space: %d", freeSpace);
  Blox_OLED_DrawStringGraphics(TEXT_LOCATION_LEFT_ALIGNED, TEXT_START_LOCATION + 3*TEXT_LOCATION_OFFSET,
                               OLED_FONT_5X7, COLOR_WHITE, 1, 1, buffer);
}

/**
 * @brief Draws the USB menu and starts listening for USB packets.
 * @note The Blox must be reset after entering USB mode
 * @retval None
 */
void Base_UI_USBMenu(void) {
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_UI_DrawTitle("USB MODE");
  
  Blox_UI_SetEntries(NULL, NULL, &Base_UI_MainMenu, 0);
  
  //connected text
  
  Transfer_Init();
  Transfer_Slave();
}

/**
 * @brief Draws a loading screen
 * @retval None
 */
void Base_UI_Loading(void) {
  Blox_UI_ClearScreen();
  Blox_UI_DrawHeader();
  Blox_UI_DrawFooter();
  Blox_OLED_DrawStringGraphics(CENTER_TEXT("LOADING", 8), 64,
                               OLED_FONT_8X12, COLOR_WHITE, 1, 1, "LOADING");
}

/**
 * @brief Listens for other Blox asking for new participants. 
 * @retval None
 */
void Base_RX(BloxFrame *frame) {
  //Addressed to me?
  if(frame->dst_id == XBEE_BLOX_BROADCAST_ID || frame->dst_id == Blox_System_GetId()) {
    if (frame->type == FRAME_TYPE_ROLE) {
      RoleFrame *rFrame = (RoleFrame *)(&(frame->data));
      switch(rFrame->opcode) {
      case PROG_QUERY:  
        Blox_LED_Toggle(LED1);
        //See if I have this application
        if (FS_GetFileFromName(((QueryFrame *)&(rFrame->data))->name) != NULL) {
          RoleFrame respFrame;
          SysTick_Wait(XBEE_HOLD_PERIOD); //Wait for hold period.
          respFrame.opcode = PROG_ACK;
          Blox_LED_Toggle(LED2);
          Blox_XBee_Send_Period((uint8_t *)&respFrame, sizeof(RoleFrame), FRAME_TYPE_ROLE, frame->src_id, XBEE_HOLD_PERIOD);
        }
        break;
      case PROG_START:
      {
        //Get the file
        FS_File *file;
        SysTick_Wait(XBEE_HOLD_PERIOD);
        file = FS_GetFileFromName(((QueryFrame *)&(rFrame->data))->name);
        FS_RunFile(file->id);
      } 
        break;
      case PARENT_QUERY:
        Blox_LED_Toggle(LED2);
        break;
      }
    }
  }
}
/** @} */
