/**
 * @file    main.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/23/2010
 * @brief   Countdown
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
#include "blox_led.h"
#include "blox_counter.h"
#include "blox_ir.h"
#include "blox_oled.h"

#include "blox_role.h"
#include "neighbor_detect.h"

#include "stdio.h"
#include "string.h"

#define CENTER_TEXT(text, width, multiplier) (64-width*multiplier*strlen(text)/2)

/**
 * @brief Message passing structure between scatterer, gatherer, and workers
 */
typedef struct {
  uint8_t type;                 /**< the type of role */
  uint32_t data;                /**< the data */
} message_t;

void Countdown_Init(void);
void Scatterer(void);
void Gatherer(void);
void Worker(void);

uint8_t workers_above, workers_below, workers_total;

int main(void)
{
  Blox_Role_Init("countdown", 9);
  Blox_Role_Add(&Worker, 1, 5);
  Blox_Role_Run();
  Countdown_Init();
  
  while (1);
}

/**
 * @brief Initializes peripherals common to all roles
 */
void Countdown_Init(void) {
  Blox_System_Init();
  SysTick_Init();
  Blox_LED_Init();
  Blox_OLED_Init();
  Blox_OLED_Clear();
  Neighbor_Detect_Init();

}


void Worker_North_IR_Handler(IRFrame *frame) {
  message_t *packet = (message_t *)(frame->data);
  if(packet->type == 'w') {
    message_t abovePacket;
    workers_above = packet->data;
    abovePacket.type = 'w';
    abovePacket.data = workers_above + 1;
    //notify below workers of the number of workers above
    IR_SendFrame(IR_SOUTH_ID, IR_FRAME_TYPE_USER, (uint8_t *)(&abovePacket), sizeof(message_t));
  }
}

void Worker_South_IR_Handler(IRFrame *frame) {
  message_t *packet = (message_t *)(frame->data);
  if(packet->type == 'w') {
    message_t belowPacket;
    workers_below = packet->data;
    belowPacket.type = 'w';
    belowPacket.data = workers_below + 1;
    //notify above workers of the number of workers below
    IR_SendFrame(IR_NORTH_ID, IR_FRAME_TYPE_USER, (uint8_t *)(&belowPacket), sizeof(message_t));
  }
}

/**
 * @brief Worker role
 */
void Worker(void) {
  char buffer[20];
  Countdown_Init();
  Neighbor_Register_IR_RX_IRQ(IR_NORTH_ID, &Worker_North_IR_Handler);
  Neighbor_Register_IR_RX_IRQ(IR_SOUTH_ID, &Worker_South_IR_Handler);
  
  Blox_OLED_SetOpaque();
  
  while(1) {
    uint8_t i;
    for(i = 1; i <= 4; i++)
      Blox_LED_Off(i);
    for(i = 1; i <= workers_total; i++)
      Blox_LED_On(i);
    
    sprintf(buffer, "%d", workers_above);
    Blox_OLED_DrawStringGraphics(CENTER_TEXT(buffer, 8, 1), 8, OLED_FONT_8X12,
                               COLOR_BLUE, 1, 1, buffer);
    
    sprintf(buffer, "%d", workers_total);
    Blox_OLED_DrawStringGraphics(CENTER_TEXT(buffer, 8, 2), 52, OLED_FONT_8X12,
                               0xF800, 2, 2, buffer);
    sprintf(buffer, "%d", workers_below);
    Blox_OLED_DrawStringGraphics(CENTER_TEXT(buffer, 8, 1), 112, OLED_FONT_8X12,
                               COLOR_BLUE, 1, 1, buffer); 
      
    if(IR_Get_Neighbor(IR_SOUTH_ID) == 0) {
      message_t belowPacket = {'w', 1};
      workers_below = 0;
      //notify above worker that you are the only one below
      IR_SendFrame(IR_NORTH_ID, IR_FRAME_TYPE_USER, (uint8_t *)(&belowPacket), sizeof(message_t));
    }
    SysTick_Wait(50);
    if(IR_Get_Neighbor(IR_NORTH_ID) == 0) {
      message_t abovePacket = {'w', 1};
      workers_above = 0;
      //notify below worker that you are the only one above
      IR_SendFrame(IR_SOUTH_ID, IR_FRAME_TYPE_USER, (uint8_t *)(&abovePacket), sizeof(message_t));
    }
    workers_total = 1 + workers_above + workers_below;
    SysTick_Wait(50);
  }
}
