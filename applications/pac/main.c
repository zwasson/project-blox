/********************************************************************************
 * @file    main.c
 * @author  Deepyaman Datta
 * @version V0.1
 * @date    10/20/2010
 * @brief   Pac-Man
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

#include "blox_counter.h"
#include "blox_ir.h"
#include "blox_oled.h"
#include "blox_system.h"
#include "blox_usb.h"
#include "neighbor_detect.h"
#include "blox_led.h"
#include "blox_gesture.h"

//cell
#define PAC_CELL_HEIGHT 128
#define PAC_CELL_MIN 0
#define PAC_CELL_MAX 127
#define PAC_CELL_WIDTH 128

//color
#define PAC_COLOR_BLACK 0x0000
#define PAC_COLOR_BLUE 0x001F
#define PAC_COLOR_WHITE 0xFFFF
#define PAC_COLOR_YELLOW 0xFFE0

//dir
#define PAC_DIR_LEFT 0
#define PAC_DIR_UP 1
#define PAC_DIR_RIGHT 2
#define PAC_DIR_DOWN 3

//init
#define PAC_INIT_PLAYER_CELL_X PAC_CELL_MAX/4
#define PAC_INIT_PLAYER_CELL_Y PAC_CELL_MAX/4
#define PAC_INIT_PLAYER_DIR PAC_DIR_LEFT
#define PAC_INIT_PLAYER_MATRIX_X 13
#define PAC_INIT_PLAYER_MATRIX_Y 22

//mask
#define PAC_MASK_LEFT 1<<PAC_DIR_LEFT
#define PAC_MASK_UP 1<<PAC_DIR_UP
#define PAC_MASK_RIGHT 1<<PAC_DIR_RIGHT
#define PAC_MASK_DOWN 1<<PAC_DIR_DOWN

//matrix
#define PAC_MATRIX_HEIGHT 29
#define PAC_MATRIX_WIDTH 26

//player
#define PAC_PLAYER_HEIGHT PAC_CELL_HEIGHT/2
#define PAC_PLAYER_WIDTH PAC_CELL_WIDTH/2

//wait
#define PAC_WAIT 100

void Pac_DrawCell(uint16_t cell);
void Pac_DrawPlayer(void);
void Pac_Init(void);
void Pac_MovePlayer(void);

#define PAC_NUM_FRAMES    45

uint8_t PAC_CELL[PAC_MATRIX_HEIGHT*PAC_MATRIX_WIDTH]={
//0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25
  12, 5,  5,  5,  5,  13, 5,  5,  5,  5,  5,  9,  0,  0,  12, 5,  5,  5,  5,  5,  13, 5,  5,  5,  5,  9,  //0
  10, 0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  10, //1
  10, 0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  10, //2
  10, 0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  10, //3
  14, 5,  5,  5,  5,  15, 5,  5,  13, 5,  5,  7,  5,  5,  7,  5,  5,  13, 5,  5,  15, 5,  5,  5,  5,  11, //4
  10, 0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  10, //5
  10, 0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  10, //6
  6,  5,  5,  5,  5,  11, 0,  0,  6,  5,  5,  9,  0,  0,  12, 5,  5,  3,  0,  0,  14, 5,  5,  5,  5,  3,  //7
  0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  //8
  0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  //9
  0,  0,  0,  0,  0,  10, 0,  0,  12, 5,  5,  7,  5,  5,  7,  5,  5,  9,  0,  0,  10, 0,  0,  0,  0,  0,  //10
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //11
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //12
  5,  5,  5,  5,  5,  15, 5,  5,  11, 0,  0,  0,  0,  0,  0,  0,  0,  14, 5,  5,  15, 5,  5,  5,  5,  5,  //13
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //14
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //15
  0,  0,  0,  0,  0,  10, 0,  0,  14, 5,  5,  5,  5,  5,  5,  5,  5,  11, 0,  0,  10, 0,  0,  0,  0,  0,  //16
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //17
  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  //18
  12, 5,  5,  5,  5,  15, 5,  5,  7,  5,  5,  9,  0,  0,  12, 5,  5,  7,  5,  5,  15, 5,  5,  5,  5,  9,  //19
  10, 0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  10, //20
  10, 0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  10, //21
  6,  5,  9,  0,  0,  14, 5,  5,  13, 5,  5,  7,  5,  15,  7,  5,  5,  13, 5,  5,  11, 0,  0,  12, 5,  3,  //22
  0,  0,  10, 0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  10, 0,  0,  //23
  0,  0,  10, 0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  10, 0,  0,  //24
  12, 5,  7,  5,  5,  3,  0,  0,  6,  5,  5,  9,  0,  0,  12, 5,  5,  3,  0,  0,  6,  5,  5,  7,  5,  9,  //25
  10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, //26
  10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, //27
  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  7,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  3   //28
//0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25
};

//struct for reading
typedef struct{
  uint8_t x;
  uint8_t y;
  uint16_t sector;
}PAC_Icon;

#define PAC_BLANK     0x1710
#define PAC_CENTER    0x10D0

PAC_Icon PAC_MOVE_LEFT[PAC_NUM_FRAMES] = {
  {64, 32, PAC_BLANK}, {64, 32, 0x1000}, {64, 32, 0x1010}, {64, 32, 0x1020}, {64, 32, 0x1030}, {64, 32, 0x1040}, {64, 32, 0x1050},
  {64, 32, 0x1060}, {64, 32, 0x1070}, {64, 32, 0x1080}, {64, 32, 0x1090}, {64, 32, 0x10A0}, {64, 32, 0x10B0},
  {64, 32, 0x10C0}, {60, 32, PAC_CENTER}, {56, 32, 0x10C0}, {52, 32, 0x10E0}, {48, 32, 0x10C0}, {44, 32, PAC_CENTER},
  {40, 32, 0x10C0}, {36, 32, 0x10E0}, {32, 32, 0x10C0}, {28, 32, PAC_CENTER}, {24, 32, 0x10C0}, {20, 32, 0x10E0},
  {16, 32, 0x10C0}, {12, 32, PAC_CENTER}, {8, 32, 0x10C0}, {4, 32, 0x10E0}, {0, 32, 0x10C0}, {0, 32, 0x10F0}, 
  {0, 32, 0x1100}, {0, 32, 0x1110}, {0, 32, 0x1120}, {0, 32, 0x1130}, {0, 32, 0x1140}, {0, 32, 0x1150}, 
  {0, 32, 0x1160}, {0, 32, 0x1170}, {0, 32, 0x1180}, {0, 32, 0x1190}, {0, 32, 0x11A0}, {0, 32, 0x11B0}, 
  {0, 32, 0x11C0}, {0, 32, PAC_BLANK}
};

PAC_Icon PAC_MOVE_UP[PAC_NUM_FRAMES] = {
  {32, 64, PAC_BLANK}, {32, 64, 0x11D0}, {32, 64, 0x11E0}, {32, 64, 0x11F0}, {32, 64, 0x1200}, {32, 64, 0x1210}, {32, 64, 0x1220},
  {32, 64, 0x1230}, {32, 64, 0x1240}, {32, 64, 0x1250}, {32, 64, 0x1260}, {32, 64, 0x1270}, {32, 64, 0x1280},
  {32, 64, 0x1290}, {32, 60, PAC_CENTER}, {32, 56, 0x1290}, {32, 52, 0x12A0}, {32, 48, 0x1290}, {32, 44, PAC_CENTER},
  {32, 40, 0x1290}, {32, 36, 0x12A0}, {32, 32, 0x1290}, {32, 28, PAC_CENTER}, {32, 24, 0x1290}, {32, 20, 0x12A0},
  {32, 16, 0x1290}, {32, 12, PAC_CENTER}, {32, 8, 0x1290}, {32, 4, 0x12A0}, {32, 0, 0x1290}, {32, 0, 0x12B0}, 
  {32, 0, 0x12C0}, {32, 0, 0x12D0}, {32, 0, 0x12E0}, {32, 0, 0x12F0}, {32, 0, 0x1300}, {32, 0, 0x1310}, 
  {32, 0, 0x1320}, {32, 0, 0x1330}, {32, 0, 0x1340}, {32, 0, 0x1350}, {32, 0, 0x1360}, {32, 0, 0x1370}, 
  {32, 0, 0x1380}, {32, 0, PAC_BLANK}
};

PAC_Icon PAC_MOVE_RIGHT[PAC_NUM_FRAMES] = {
  {0, 32, PAC_BLANK}, {0, 32, 0x1390}, {0, 32, 0x13A0}, {0, 32, 0x13B0}, {0, 32, 0x13C0}, {0, 32, 0x13D0}, {0, 32, 0x13E0},
  {0, 32, 0x13F0}, {0, 32, 0x1400}, {0, 32, 0x1410}, {0, 32, 0x1420}, {0, 32, 0x1430}, {0, 32, 0x1440},
  {0, 32, 0x1450}, {4, 32, PAC_CENTER}, {8, 32, 0x1450}, {12, 32, 0x1460}, {16, 32, 0x1450}, {20, 32, PAC_CENTER},
  {24, 32, 0x1450}, {28, 32, 0x1460}, {32, 32, 0x1450}, {36, 32, PAC_CENTER}, {40, 32, 0x1450}, {44, 32, 0x1460},
  {48, 32, 0x1450}, {52, 32, PAC_CENTER}, {56, 32, 0x1450}, {60, 32, 0x1460}, {64, 32, 0x1450}, {64, 32, 0x1470}, 
  {64, 32, 0x1480}, {64, 32, 0x1490}, {64, 32, 0x14A0}, {64, 32, 0x14B0}, {64, 32, 0x14C0}, {64, 32, 0x14D0}, 
  {64, 32, 0x14E0}, {64, 32, 0x14F0}, {64, 32, 0x1500}, {64, 32, 0x1510}, {64, 32, 0x1520}, {64, 32, 0x1530}, 
  {64, 32, 0x1540}, {64, 32, PAC_BLANK}
};

PAC_Icon PAC_MOVE_DOWN[PAC_NUM_FRAMES] = {
  {32, 0, PAC_BLANK}, {32, 0, 0x1550}, {32, 0, 0x1560}, {32, 0, 0x1570}, {32, 0, 0x1580}, {32, 0, 0x1590}, {32, 0, 0x15A0},
  {32, 0, 0x15B0}, {32, 0, 0x15C0}, {32, 0, 0x15D0}, {32, 0, 0x15E0}, {32, 0, 0x15F0}, {32, 0, 0x1600},
  {32, 0, 0x1610}, {32, 4, PAC_CENTER}, {32, 8, 0x1610}, {32, 12, 0x1620}, {32, 16, 0x1610}, {32, 20, PAC_CENTER},
  {32, 24, 0x1610}, {32, 28, 0x1620}, {32, 32, 0x1610}, {32, 36, PAC_CENTER}, {32, 40, 0x1610}, {32, 44, 0x1620},
  {32, 48, 0x1610}, {32, 52, PAC_CENTER}, {32, 56, 0x1610}, {32, 60, 0x1620}, {32, 64, 0x1610}, {32, 64, 0x1630}, 
  {32, 64, 0x1640}, {32, 64, 0x1650}, {32, 64, 0x1660}, {32, 64, 0x1670}, {32, 64, 0x1680}, {32, 64, 0x1690}, 
  {32, 64, 0x16A0}, {32, 64, 0x16B0}, {32, 64, 0x16C0}, {32, 64, 0x16D0}, {32, 64, 0x16E0}, {32, 64, 0x16F0}, 
  {32, 64, 0x1700}, {32, 64, PAC_BLANK}
};

/*uint16_t right[43] = {
  whateverXis<<8||whateverYis,0x1000,etc... 0x1010, 0x1020, 0x1030, 0x1040, 0x1050, 0x1060, 0x1070, 0x1080, 0x1090, 0x10A0, 0x10B0, 
  0x10C0, 0x10D0, 0x10F0,
}*/

void Pac_MoveLeft(void) {
  static uint8_t i;
  Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[i].x, PAC_MOVE_LEFT[i].y, 64, 64, PAC_MOVE_LEFT[i].sector);
  i++;
  if(i == PAC_NUM_FRAMES)
    i = 0;
}

void Pac_MoveUp(void) {
  static uint8_t i;
  Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[i].x, PAC_MOVE_UP[i].y, 64, 64, PAC_MOVE_UP[i].sector);
  i++;
  if(i == PAC_NUM_FRAMES)
    i = 0;
}

void Pac_MoveRight(void) {
  static uint8_t i;
  Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[i].x, PAC_MOVE_RIGHT[i].y, 64, 64, PAC_MOVE_RIGHT[i].sector);
  i++;
  if(i == PAC_NUM_FRAMES)
    i = 0;
}

void Pac_MoveDown(void) {
  static uint8_t i;
  Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[i].x, PAC_MOVE_DOWN[i].y, 64, 64, PAC_MOVE_DOWN[i].sector);
  i++;
  if(i == PAC_NUM_FRAMES)
    i = 0;
}

//player
uint8_t PAC_PLAYER_CELL_X;
uint8_t PAC_PLAYER_CELL_Y;
uint8_t PAC_PLAYER_DIR;
uint8_t PAC_PLAYER_MATRIX_X;
uint8_t PAC_PLAYER_MATRIX_Y;

int main(void)
{  
  uint16_t i = 0;
  //Blox_Role_Init("ir_role_test", 12);
  //Blox_Role_Add(&IR_Tx, 1, 1);
  //Blox_Role_Add(&IR_Rx, 1, 1); 
  //Blox_Role_Run();
  //IR_Tx();
  //IR_Rx();
  Blox_OLED_Init();
  Blox_LED_Init();
  Blox_Gesture_Init();
  Neighbor_Detect_Init();
  Pac_Init();
  SysTick_Init();
  Blox_Timer_Init(7, 10000);
  Blox_Timer_Register_IRQ(7, 750, &Pac_MovePlayer, ENABLE);
  
  i = sizeof(PAC_Icon);
  
  for(;;){
    static uint32_t old_north_time, old_south_time, old_east_time, old_west_time;

    if(Blox_Gesture_GetGesture(TOUCH_NORTH_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_NORTH_ID) != old_north_time) {
      PAC_PLAYER_DIR = PAC_DIR_DOWN;
      old_north_time = Blox_Gesture_GetGestureTime(TOUCH_NORTH_ID);
    }
    if(Blox_Gesture_GetGesture(TOUCH_SOUTH_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_SOUTH_ID) != old_south_time) {
      PAC_PLAYER_DIR = PAC_DIR_UP;
      old_south_time = Blox_Gesture_GetGestureTime(TOUCH_SOUTH_ID);
    }
    if(Blox_Gesture_GetGesture(TOUCH_EAST_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_EAST_ID) != old_east_time) {
      PAC_PLAYER_DIR = PAC_DIR_LEFT;
      old_east_time = Blox_Gesture_GetGestureTime(TOUCH_EAST_ID);
    }
    if(Blox_Gesture_GetGesture(TOUCH_WEST_ID) == TOUCH_GESTURE_TAP &&
       Blox_Gesture_GetGestureTime(TOUCH_WEST_ID) != old_west_time) {
      PAC_PLAYER_DIR = PAC_DIR_RIGHT;
      old_west_time = Blox_Gesture_GetGestureTime(TOUCH_WEST_ID);
    }
    
    Blox_LED_Off(LED_NORTH);
    Blox_LED_Off(LED_SOUTH);
    Blox_LED_Off(LED_EAST);
    Blox_LED_Off(LED_WEST);
    switch(PAC_PLAYER_DIR) {
      case PAC_DIR_UP:
        Blox_LED_On(LED_NORTH);
        break;
      case PAC_DIR_DOWN:
        Blox_LED_On(LED_SOUTH);
        break;
      case PAC_DIR_RIGHT:
        Blox_LED_On(LED_EAST);
        break;
      case PAC_DIR_LEFT:
        Blox_LED_On(LED_WEST);
        break;
    }
  }
}

void Pac_DrawCell(uint16_t cell){
  Blox_OLED_DrawRectangle(PAC_CELL_MIN,PAC_CELL_MIN,PAC_CELL_MAX,PAC_CELL_MAX,PAC_COLOR_BLACK);
  
  //left
  if(PAC_CELL[cell]&PAC_MASK_LEFT){
    Blox_OLED_DrawLine(PAC_CELL_MIN,PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_COLOR_BLUE);
    Blox_OLED_DrawLine(PAC_CELL_MIN,PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_COLOR_BLUE);
  }
  else
    Blox_OLED_DrawLine(PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_COLOR_BLUE);
    
  //up
  if(PAC_CELL[cell]&PAC_MASK_UP){
    Blox_OLED_DrawLine(PAC_CELL_MAX/8,PAC_CELL_MIN,PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_COLOR_BLUE);
    Blox_OLED_DrawLine(PAC_CELL_MAX*7/8,PAC_CELL_MIN,PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_COLOR_BLUE);
  }
  else
    Blox_OLED_DrawLine(PAC_CELL_MAX/8,PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_COLOR_BLUE);    
    
  //right
  if(PAC_CELL[cell]&PAC_MASK_RIGHT){
    Blox_OLED_DrawLine(PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_CELL_MAX,PAC_CELL_MAX/8,PAC_COLOR_BLUE);
    Blox_OLED_DrawLine(PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX,PAC_CELL_MAX*7/8,PAC_COLOR_BLUE);
  }
  else
    Blox_OLED_DrawLine(PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_COLOR_BLUE);
    
  //down
  if(PAC_CELL[cell]&PAC_MASK_DOWN){
    Blox_OLED_DrawLine(PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX/8,PAC_CELL_MAX,PAC_COLOR_BLUE);
    Blox_OLED_DrawLine(PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX,PAC_COLOR_BLUE);
  }
  else
    Blox_OLED_DrawLine(PAC_CELL_MAX/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_CELL_MAX*7/8,PAC_COLOR_BLUE);
}

void Pac_DrawPlayer(void){
  Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[0].x, PAC_MOVE_RIGHT[0].y, 64, 64, PAC_MOVE_RIGHT[0].sector);
}

void Pac_Init(void){
  //global variables
  PAC_PLAYER_CELL_X=PAC_INIT_PLAYER_CELL_X;
  PAC_PLAYER_CELL_Y=PAC_INIT_PLAYER_CELL_Y;
  PAC_PLAYER_DIR=PAC_INIT_PLAYER_DIR;
  PAC_PLAYER_MATRIX_X=PAC_INIT_PLAYER_MATRIX_X;
  PAC_PLAYER_MATRIX_Y=PAC_INIT_PLAYER_MATRIX_Y;
  
  PAC_PLAYER_DIR = PAC_DIR_RIGHT;
  
  Pac_DrawCell(PAC_PLAYER_MATRIX_X+PAC_PLAYER_MATRIX_Y*PAC_MATRIX_WIDTH);
  Pac_DrawPlayer();
}

#define CENTER_INDEX    22
#define STOP_INDEX      8
void Pac_MovePlayer(void){
  static uint8_t index;
  static uint8_t previousDir = PAC_DIR_RIGHT;
  index++;
  switch(PAC_PLAYER_DIR){
    case PAC_DIR_LEFT:
      if(previousDir == PAC_DIR_UP) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
          previousDir = PAC_DIR_LEFT;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
        }
      } else if(previousDir == PAC_DIR_DOWN) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
          previousDir = PAC_DIR_LEFT;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
        }
      } else if(previousDir == PAC_DIR_RIGHT) {
        index = PAC_NUM_FRAMES - index + 1;
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
        previousDir = PAC_DIR_LEFT;
      } else if(index > STOP_INDEX && index < PAC_NUM_FRAMES - STOP_INDEX) {
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
      }
      break;
    case PAC_DIR_UP:
      if(previousDir == PAC_DIR_LEFT) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
          previousDir = PAC_DIR_UP;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
        }
      } else if(previousDir == PAC_DIR_RIGHT) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
          previousDir = PAC_DIR_UP;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
        }
      } else if(previousDir == PAC_DIR_DOWN) {
        index = PAC_NUM_FRAMES - index + 1;
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
        previousDir = PAC_DIR_UP;
      } else if(index > STOP_INDEX && index < PAC_NUM_FRAMES - STOP_INDEX) {
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
      }
      break;
    case PAC_DIR_RIGHT:
      if(previousDir == PAC_DIR_UP) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
          previousDir = PAC_DIR_RIGHT;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_UP[index].x, PAC_MOVE_UP[index].y, 64, 64, PAC_MOVE_UP[index].sector);
        }
      } else if(previousDir == PAC_DIR_DOWN) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
          previousDir = PAC_DIR_RIGHT;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
        }
      } else if(previousDir == PAC_DIR_LEFT) {
        index = PAC_NUM_FRAMES - index + 1;
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
        previousDir = PAC_DIR_RIGHT;
      } else if(index > STOP_INDEX && index < PAC_NUM_FRAMES - STOP_INDEX) {
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
      }
      break;
    case PAC_DIR_DOWN:
      if(previousDir == PAC_DIR_LEFT) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
          previousDir = PAC_DIR_DOWN;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_LEFT[index].x, PAC_MOVE_LEFT[index].y, 64, 64, PAC_MOVE_LEFT[index].sector);
        }
      } else if(previousDir == PAC_DIR_RIGHT) {
        if(index == CENTER_INDEX) {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
          previousDir = PAC_DIR_DOWN;
        } else {
          Blox_OLED_SD_DisplayIcon(PAC_MOVE_RIGHT[index].x, PAC_MOVE_RIGHT[index].y, 64, 64, PAC_MOVE_RIGHT[index].sector);
        }
      } else if(previousDir == PAC_DIR_UP) {
        index = PAC_NUM_FRAMES - index + 1;
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
        previousDir = PAC_DIR_DOWN;
      } else if(index > STOP_INDEX && index < PAC_NUM_FRAMES - STOP_INDEX) {
        Blox_OLED_SD_DisplayIcon(PAC_MOVE_DOWN[index].x, PAC_MOVE_DOWN[index].y, 64, 64, PAC_MOVE_DOWN[index].sector);
      }
      break;
  }
}

