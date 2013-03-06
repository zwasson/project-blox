/********************************************************************************
 * @file    main.c
 * @author  Dan Cleary
 * @version V0.1
 * @brief   Test program for most of the Blox peripherals.
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
 
#include "stm32f10x.h"
#include "blox_exti.h"
#include "blox_fifo.h"
#include "blox_usb.h"
#include "blox_tim.h"
#include "blox_counter.h"
#include "blox_oled.h"
#include "blox_accel.h"
#include "blox_led.h"
#include "blox_touch.h"
#include "blox_gesture.h"
#include "blox_xbee.h"
#include "blox_speaker.h"

void RCC_Configuration (void);
void GPIO_Configuration (void);

TIMER_ID id = INVALID_TIMER;

int main(void)
{
  uint32_t i;
  char message[200];
  
  uint16_t Xval;
  uint16_t Yval; 
  uint16_t Z1val;
  uint16_t Z2val;
  int Dval;
  
  RCC_Configuration(); 
  GPIO_Configuration();
  SysTick_Init();


  Blox_OLED_Init();
  Blox_Accel_Init();
  Blox_LED_Init();
  Blox_Touch_Init();
  //Blox_XBee_Init();
  Blox_Speaker_Init();
  //Touch_Gesture_Init();

  Blox_OLED_SetOpaque();
  Blox_OLED_SD_DisplayIcon(0, 0, 128, 128, 0x001000);
  SysTick_Wait(1000);
  Blox_OLED_Clear();
  
  PlayMusic(); //play startup music
  
  /* Infinite loop */
  i = 0;
  while (1)
  {
    i++;
    if (i == 2) {
       Blox_LED_Toggle(LED1);
    }
    if (i == 4) {
       Blox_LED_Toggle(LED2);
    }
    if (i == 6) {
       Blox_LED_Toggle(LED3);
    }
    if (i == 8) {
       Blox_LED_Toggle(LED4);
       i = 0;
    }
    
    //OLED_Clear();
    //OLED_DrawStringGraphics(0, 16, OLED_FONT_5X7, 0, 1, 1, message1);
  	sprintf(message, "X:%4.2fV	", (float)(Blox_Accel_GetX() * 3.3 / 0xFFF));
  	Blox_OLED_DrawStringGraphics(0, 16, OLED_FONT_5X7, 44466, 1, 1, message);
  
    //OLED_DrawStringGraphics(0, 32, OLED_FONT_5X7, 0, 1, 1, message2);
  	sprintf(message, "Y:%4.2fV	", (float)(Blox_Accel_GetY() * 3.3 / 0xFFF));
  	Blox_OLED_DrawStringGraphics(0, 32, OLED_FONT_5X7, 44466, 1, 1, message);
                        
    //OLED_DrawStringGraphics(0, 48, OLED_FONT_5X7, 0, 1, 1, message3);
  	sprintf(message, "Z:%4.2fV	", (float)(Blox_Accel_GetZ() * 3.3 / 0xFFF));
  	Blox_OLED_DrawStringGraphics(0, 48, OLED_FONT_5X7, 44466, 1, 1, message);

    Xval = Blox_Touch_GetX(4);
    Yval = Blox_Touch_GetY(4); 
    Z1val = Blox_Touch_GetZ1(4);
    Z2val = Blox_Touch_GetZ2(4);
    
    //OLED_DrawStringGraphics(0, 64, OLED_FONT_5X7, 0, 1, 1, message4);
    sprintf(message, "X: %3d   Y: %3d", Xval, Yval);
    Blox_OLED_DrawStringGraphics(0, 64, OLED_FONT_5X7, 44466, 1, 1, message);
    
    //OLED_DrawStringGraphics(0, 80, OLED_FONT_5X7, 0, 1, 1, message5);
    sprintf(message, "Z1: %3d   Z2: %3d", Z1val, Z2val);
    Blox_OLED_DrawStringGraphics(0, 80, OLED_FONT_5X7, 44466, 1, 1, message);
	
	
	/*Dval = GetDirection(2);
	switch(Dval){
		case 1: sprintf(message, "TOUCH_GESTURE_LR"); 
				break; 
		case 2: sprintf(message, "TOUCH_GESTURE_RL"); 
				break;
		case 3: sprintf(message, "TOUCH_GESTURE_DU"); 	
				break;
		case 4: sprintf(message, "TOUCH_GESTURE_UD"); 	
				break;
		case 5: sprintf(message, "TOUCH_DIAG_DLUR");	
				break;
		case 6: sprintf(message, "TOUCH_DIAG_DRUL");	
				break;	
		case 7: sprintf(message, "TOUCH_DIAG_ULDR");	
				break;	
		case 8: sprintf(message, "TOUCH_DIAG_URDL");
				break;		
		case 9: sprintf(message, "TOUCH_GESTURE_TAP");	
				break;
		default: sprintf(message, "NO_GESTURE");
				break;		
		}
		
    Blox_OLED_DrawStringGraphics(0, 100, OLED_FONT_5X7, 44466, 1, 1, message);*/
    //SysTick_Wait(250);
    
   // Blox_XBee_Print();

  }
}

/* System Clock Configuration */
void RCC_Configuration (void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/* GPIO Port Configuration */
void GPIO_Configuration (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

