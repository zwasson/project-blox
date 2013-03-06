/**
 * @file    blox_oled.c
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/20/2010
 * @brief   Driver for Blox OLED screen
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
 
#include "blox_oled.h"

/**
 * @ingroup driver_oled
 * @{
 */

/* Private function prototypes */
void OLED_RCC_Configuration(void);
void OLED_GPIO_Configuration(void);
void OLED_ResetDisplay(void);

/**
 * @brief Switches the Reset pin PC3 for the OLED display.
 * @retval None
 */
void OLED_Reset(void) {
  GPIOC->ODR &= ~(1<<3);
  //TODO: insert timer wait 200ms
  SysTick_Wait(20);
  GPIOC->ODR |= (1<<3);
  SysTick_Wait(20);
}

/**
 * @brief Initializes the OLED display.
 * @retval None
 */
void Blox_OLED_Init(void) {
  uint8_t garbage;
  OLED_RCC_Configuration();
  OLED_GPIO_Configuration();
  SysTick_Init();
  Blox_VUSART_Init(OLED_USART_ID);

  OLED_Reset();
  SysTick_Wait(2000);
  Blox_VUSART_TryReceive(2, &garbage);  //receive any garbage data
  Blox_OLED_Send(OLED_AUTOBAUD); 
  Blox_OLED_Receive();
}

/**
 * @brief Initializes clocks for OLED reset pin.
 * @retval None
 */
void OLED_RCC_Configuration() {
    RCC_APB2PeriphClockCmd(OLED_RESET_GPIO_CLK, ENABLE);
}

/**
 * @brief Initializes the gpio for the OLED reset pin.
 * @retval None
 */
void OLED_GPIO_Configuration() {
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  //OLED Reset is push-pull, 50Mz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = OLED_RESET_PIN;
  GPIO_Init(OLED_RESET_GPIO, &GPIO_InitStructure);
}

/**
 * @brief Receive a byte from the OLED. Wrapper around USART
 * @retval The received command or 0 on error.
 */
uint8_t Blox_OLED_Receive(void) {
  uint8_t data;
  while(Blox_VUSART_TryReceive(2, &data) == RX_EMPTY);
  return data;
}

/**
 * @brief Send a byte to the OLED. Wrapper around USART
 * @param data The byte to send.   
 * @retval None
 */
void Blox_OLED_Send(uint8_t data) {
  while(Blox_VUSART_TrySend(2, data) == TX_BUSY);
}

/********************************************/ 
/*            General Commands              */
/********************************************/

/**
 * @brief Clear OLED display
 * @retval none.
 */
void Blox_OLED_Clear(void){
  Blox_OLED_Send(OLED_CLEAR); // Pixel write
  Blox_OLED_Receive();
}

/********************************************/ 
/*            Graphics Commands             */
/********************************************/

/**
 * @brief Draw circle on OLED display. 
 * @param x X-coordinate of circle's center.
 * @param y Y-coordinate of circle's center.
 * @param radius Radius of circle.
 * @param color 2 byte RGB color of circle.   
 * @retval None
 */
void Blox_OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, uint16_t color){
  Blox_OLED_Send(OLED_DRAW_CIRCLE); 
	Blox_OLED_Send(x);
	Blox_OLED_Send(y);
	Blox_OLED_Send(radius);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);
  Blox_OLED_Receive();
}

/**
 * @brief Draw line on OLED display. 
 * @param x1 X-coordinate of line's start position.
 * @param y1 Y-coordinate of line's start position.
 * @param x2 X-coordinate of line's end position.
 * @param y2 Y-coordinate of line's end position.
 * @param color 2 byte RGB color of pixel.   
 * @retval None
 */
void Blox_OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color){
  Blox_OLED_Send(OLED_DRAW_LINE); 
	Blox_OLED_Send(x1);
	Blox_OLED_Send(y1);  
	Blox_OLED_Send(x2);
	Blox_OLED_Send(y2);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);
  Blox_OLED_Receive();
}

/**
 * @brief Draw pixel on OLED display. 
 * @param x X-coordinate of pixel.
 * @param y Y-coordinate of pixel.
 * @param color 2 byte RGB color of pixel.   
 * @retval None
 */
void Blox_OLED_DrawPixel(uint8_t x, uint8_t y, uint16_t color){
  Blox_OLED_Send(OLED_DRAW_PIXEL); 
	Blox_OLED_Send(x);
	Blox_OLED_Send(y);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);
  Blox_OLED_Receive();
}

/**
 * @brief Draw line on OLED display. 
 * @param x1 X-coordinate of top-left corner of rectangle.
 * @param y1 Y-coordinate of top-left corner of rectangle.
 * @param x2 X-coordinate of bottom-right corner of rectangle.
 * @param y2 Y-coordinate of bottom-right corner of rectangle.
 * @param color 2 byte RGB color of pixel.   
 * @retval None
 */
void Blox_OLED_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color){
  Blox_OLED_Send(OLED_DRAW_RECT); 
	Blox_OLED_Send(x1);
	Blox_OLED_Send(y1);
	Blox_OLED_Send(x2);
	Blox_OLED_Send(y2);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);
  Blox_OLED_Receive();
}

/********************************************/ 
/*            Text Commands                 */
/********************************************/

/**
 * @brief Sets font type for OLED characters. 
 * @param font: Font type.
 *          @arg OLED_FONT_5X7
 *          @arg OLED_FONT_8X8
 *          @arg OLED_FONT_8x12
 * @retval None
 */
void Blox_OLED_SetFont(uint8_t font){
  Blox_OLED_Send(OLED_SET_FONT); 
	Blox_OLED_Send(font);
  Blox_OLED_Receive();
}

/**
 * @brief Sets the font to have an opaque background.
 * @retval None
 */
void Blox_OLED_SetOpaque(void){
  Blox_OLED_Send(OLED_SET_VIS);
  Blox_OLED_Send(OLED_SET_VIS_OPAQ);
  Blox_OLED_Receive();
}

/**
 * @brief Draw graphics formatted string on OLED display. 
 * @param x X-coordinate of top left corner of character
 * @param y Y-coordinate of top left corner of character    
 *          range: 0 - 9 for 8x8 and 8x12
 * @param font Font type.
 *          @arg OLED_FONT_5X7
 *          @arg OLED_FONT_8X8
 *          @arg OLED_FONT_8x12
 * @param color 2 byte RGB color of pixel. 
 * @param width width multiplier of default width
 * @param height height multiplier of default height
 * @param string Pointer to string of text to be displayed.   
 * @retval None
 */
void Blox_OLED_DrawStringGraphics(uint8_t x, uint8_t y, uint8_t font, uint16_t color, uint8_t width, uint8_t height, uint8_t *string){
  uint8_t *pt;

  Blox_OLED_Send(OLED_DRAW_STRING_GRAPHICS); 
	Blox_OLED_Send(x);    
	Blox_OLED_Send(y); 
	Blox_OLED_Send(font);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);  
	Blox_OLED_Send(width);    
	Blox_OLED_Send(height); 

  pt = string;
  while (*pt)
	{
		Blox_OLED_Send(*pt);
    pt++;
	}

	Blox_OLED_Send(0x00); //string terminator
  Blox_OLED_Receive();
}

/**
 * @brief Draw text formatted character on OLED display. 
 * @param character: Character to display
 * @param column Horizontal start position of text.
 *          range: 0 - 20 for 5x7                 
 *          range: 0 - 15 for 8x8 and 8x12
 * @param row Vertical start position of text.
 *          range: 0 - 15 for 5x7                 
 *          range: 0 - 9 for 8x8 and 8x12
 * @param color 2 byte RGB color of pixel.  
 * @retval None
 */
void Blox_OLED_DrawCharText(uint8_t character, uint8_t column, uint8_t row, uint16_t color){
  Blox_OLED_Send(OLED_DRAW_CHAR_TEXT); 
	Blox_OLED_Send(character);
	Blox_OLED_Send(column);    
	Blox_OLED_Send(row); 
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);  
  Blox_OLED_Receive();
}

/**
 * @brief Draw text formatted string on OLED display. 
 * @param column Horizontal start position of text.
 *          range: 0 - 20 for 5x7                 
 *          range: 0 - 15 for 8x8 and 8x12
 * @param row Vertical start position of text.
 *          range: 0 - 15 for 5x7                 
 *          range: 0 - 9 for 8x8 and 8x12
 * @param font Font type.
 *          @arg OLED_FONT_5X7
 *          @arg OLED_FONT_8X8
 *          @arg OLED_FONT_8x12
 * @param color 2 byte RGB color of pixel.
 * @param string Pointer to string of text to be displayed.   
 * @retval None
 */
void Blox_OLED_DrawStringText(uint8_t column, uint8_t row, uint8_t font, uint16_t color, uint8_t *string){
  uint8_t *pt;

  Blox_OLED_Send(OLED_DRAW_STRING_TEXT); 
	Blox_OLED_Send(column);    
	Blox_OLED_Send(row); 
	Blox_OLED_Send(font);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);

  pt = string;
  while (*pt)
	{
		Blox_OLED_Send(*pt);
    pt++;
	}

	Blox_OLED_Send(0x00); //string terminator
  Blox_OLED_Receive();
}

/**
 * @brief Draw graphics formatted character on OLED display. 
 * @param character Character to display
 * @param x 	X-coordinate of top left corner of character
 * @param y	Y-coordinate of top left corner of character
 * @param color 2 byte RGB color of pixel.
 * @param width width multiplier of default width
 * @param height height multiplier of default height    
 * @retval None
 */
void Blox_OLED_DrawCharGraphics(uint8_t character, uint8_t x, uint8_t y, uint16_t color, uint8_t width, uint8_t height){
  Blox_OLED_Send(OLED_DRAW_CHAR_GRAPHICS); 
	Blox_OLED_Send(character); 
	Blox_OLED_Send(x);
	Blox_OLED_Send(y);
	Blox_OLED_Send(color >> 8);			
	Blox_OLED_Send(color & 0xFF);
	Blox_OLED_Send(width);
	Blox_OLED_Send(height);
  Blox_OLED_Receive();
}

/********************************************/ 
/*            SD Commands                   */
/********************************************/

/**
 * @brief Displays bitmap image icon stored in SD card onto OLED screen. 
 * @param x X-coordinate of top left corner of rectangle.
 * @param y Y-coordinate of top left corner of rectangle.
 * @param width Width of rectangle.
 * @param height Height of rectangle.  
 * @param sector the sector where the icon is located 
 * @retval None
 */
void Blox_OLED_SD_DisplayIcon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t sector){ 
  Blox_OLED_Send(0x40);
  Blox_OLED_Send(0x49);
  Blox_OLED_Send(x);
  Blox_OLED_Send(y);
  Blox_OLED_Send(width);
  Blox_OLED_Send(height);
  Blox_OLED_Send(8);  //color mode of all gifs are 65k

  Blox_OLED_Send((sector >> 16) & 0x0FF);
  Blox_OLED_Send((sector >> 8) & 0x0FF);
  Blox_OLED_Send(sector & 0x0FF);
  Blox_OLED_Receive();
}
