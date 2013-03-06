/**
 * @file    blox_oled.h
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/20/2010
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

#ifndef __BLOX_OLED_H
#define __BLOX_OLED_H

#include "blox_system.h"
#include "blox_counter.h"
#include "blox_vusart.h"

/**
 * @ingroup driver_oled
 * @{
 */
#define OLED_USART_ID 2

#define OLED_RESET_GPIO 		GPIOC
#define OLED_RESET_GPIO_CLK 	RCC_APB2Periph_GPIOC
#define OLED_RESET_PIN			GPIO_Pin_3
#define OLED_RESET_PIN_NUM		3


/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N     0

#define OLED_BAUDRATE                  9600//57600
#define OLED_RESETPIN                  8      // PIN of reset
#define OLED_INITDELAYMS               5000

#define OLED_ACK  0x06  // Ok
#define OLED_NAK  0x15  // Error

/********************************************/ 
/*            General Commands              */
/********************************************/

#define	OLED_AUTOBAUD                   0x55
#define	OLED_DEVICE_INFO                0x56
#define	OLED_BKG_COLOR                  0x42
#define	OLED_CLEAR                      0x45
#define	OLED_DISPLAY_CONTROL            0x59
#define	OLED_COMMAND_DISPLAY					  0x01
#define	OLED_COMMAND_CONTRAST					  0x02
#define	OLED_COMMAND_POWER					    0x03
#define	OLED_SLEEP                      0x5A

/********************************************/ 
/*            Graphics Commands             */
/********************************************/
#define	OLED_ADD_USER_CHAR              0x41
#define	OLED_DRAW_CIRCLE                0x43
#define	OLED_DRAW_USER_CHAR             0x44
#define	OLED_DRAW_TRIANGLE              0x47
#define	OLED_DRAW_ICON                  0x49
#define	OLED_OPAQUE_BKG_COLOR           0x4B 
#define	OLED_DRAW_LINE                  0x4C
#define	OLED_DRAW_PIXEL                 0x50
#define	OLED_READ_PIXEL                 0x52
#define	OLED_SCREEN_COPY_PASTE          0x63
#define	OLED_DRAW_POLYGON               0x67
#define	OLED_REPLACE_COLOR              0x6B
#define	OLED_SET_PIN_SIZE               0x70
#define	OLED_DRAW_RECT                  0x72

/********************************************/ 
/*            Text Commands                 */
/********************************************/
#define	OLED_SET_FONT                   0x46
  #define	OLED_FONT_5X7                 0x00
  #define	OLED_FONT_8X8                 0x01
  #define	OLED_FONT_8X12                0x02

#define OLED_SET_VIS                    0x4F
  #define OLED_SET_VIS_TRANS            0x00
  #define OLED_SET_VIS_OPAQ             0x01
#define OLED_DRAW_STRING_GRAPHICS       0x53
#define	OLED_DRAW_CHAR_TEXT             0x54
#define	OLED_DRAW_TEXT_BUTTON           0x62
#define	OLED_DRAW_STRING_TEXT           0x73
#define	OLED_DRAW_CHAR_GRAPHICS         0x74


/********************************************/ 
/*            SD Commands                   */
/********************************************/
#define OLED_SD_CMD_EXT                 0x40
#define OLED_SD_CMODE_256               0x08
#define OLED_SD_CMODE_65K               0x10

#define	OLED_SD_SET_ADDRESS_PNT         0x41
#define	OLED_SD_SCREEN_SAVE             0x43
#define	OLED_SD_DISPLAY_ICON            0x49
#define	OLED_SD_DISPLAY_OBJECT          0x4F
#define	OLED_SD_RUN_SCRIPT              0x50
#define	OLED_SD_READ_SECTOR             0x52 
#define	OLED_SD_DISPLAY_VID             0x56
#define	OLED_SD_WRITE_SECTOR            0x57
#define	OLED_SD_INIT_CARD               0x69
#define	OLED_SD_READ_BYTE               0x72
#define	OLED_SD_WRITE_BYTE              0x77

/********************************************/ 
/*            SD Sectors                    */
/********************************************/
#define OLED_IMG_0                  0x001000
#define OLED_IMG_1                  0x001010
#define OLED_IMG_2                  0x001010
#define OLED_IMG_3                  0x001010
#define OLED_IMG_4                  0x001010
#define OLED_IMG_5                  0x001010
#define OLED_IMG_6                  0x001010
#define OLED_IMG_7                  0x001010
#define OLED_IMG_8                  0x001010
#define OLED_IMG_9                  0x001010
#define OLED_IMG_10                 0x001010
#define OLED_IMG_11                 0x001010
#define OLED_IMG_12                 0x001010
#define OLED_IMG_13                 0x001010
#define OLED_IMG_14                 0x001010

/********************************************/ 
/*                Colors                    */
/********************************************/
#define COLOR_BLACK                 0x0000
#define COLOR_BLUE                  0x001F
#define COLOR_WHITE                 0xFFFF
#define COLOR_YELLOW                0xFFE0
#define COLOR_GRAPE                 0x8210
#define COLOR_LIGHT_BROWN           0xBAA7
#define COLOR_DARK_BROWN            0x7000

//void OLED_Reset (void);
void Blox_OLED_Init (void);
uint8_t Blox_OLED_Receive (void);
void Blox_OLED_Send (uint8_t data);

/* General Commands */  
void Blox_OLED_Clear(void);

/* Graphics Commands */ 
void Blox_OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, uint16_t color);
void Blox_OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
void Blox_OLED_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void Blox_OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);

/* Text Commands */ 
void Blox_OLED_SetFont(uint8_t font);
void Blox_OLED_SetOpaque(void);
void Blox_OLED_DrawStringGraphics(uint8_t x, uint8_t y, uint8_t font, uint16_t color, uint8_t width, uint8_t height, uint8_t *string);
void Blox_OLED_DrawCharText(uint8_t character, uint8_t column, uint8_t row, uint16_t color); 
void Blox_OLED_DrawStringText(uint8_t column, uint8_t row, uint8_t font_size, uint16_t color, uint8_t *string);
void Blox_OLED_DrawCharGraphics(uint8_t character, uint8_t x, uint8_t y, uint16_t color, uint8_t width, uint8_t height);

/* SD Commands */
void Blox_OLED_SD_DisplayIcon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t sector); 
/** @} */
#endif
