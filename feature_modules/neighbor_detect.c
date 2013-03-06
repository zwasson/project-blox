/**
 * @file    neighbor_detect.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/17/2010
 * @brief   A driver for the neighbor detection module
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

#include "neighbor_detect.h"

/**
 * @ingroup feature_neighbor
 * @{
 */
void IR_Ping(void);
void IR_North_Neighbor_Handler(IRFrame *frame);
void IR_East_Neighbor_Handler(IRFrame *frame);
void IR_South_Neighbor_Handler(IRFrame *frame);
void IR_West_Neighbor_Handler(IRFrame *frame);

void (*IR_North_User_Handler)(IRFrame *frame) = NULL;
void (*IR_East_User_Handler)(IRFrame *frame) = NULL;
void (*IR_South_User_Handler)(IRFrame *frame) = NULL;
void (*IR_West_User_Handler)(IRFrame *frame) = NULL;

uint8_t neighbors[4] = {FALSE};
uint8_t neighbors_id[4] = {0,0,0,0};
uint8_t neighbor_update_flag[4] = {FALSE};

/**
 * @brief Initializes the Neighbor Detection module.
 * @retval None
 */
void Neighbor_Detect_Init(void) {
  USB_Init();

  IR_Init(IR_NORTH_ID);
  IR_Init(IR_EAST_ID);
  IR_Init(IR_SOUTH_ID);
  IR_Init(IR_WEST_ID);
  Blox_Timer_Init(NEIGHBOR_TIMx, NEIGHBOR_TIM_CLK);
  
  Blox_IR_Register_RX_IRQ(IR_NORTH_ID, &IR_North_Neighbor_Handler);
  Blox_IR_Register_RX_IRQ(IR_EAST_ID, &IR_East_Neighbor_Handler);
  Blox_IR_Register_RX_IRQ(IR_SOUTH_ID, &IR_South_Neighbor_Handler);
  Blox_IR_Register_RX_IRQ(IR_WEST_ID, &IR_West_Neighbor_Handler); 
  
  Blox_Timer_Register_IRQ(NEIGHBOR_TIMx, NEIGHBOR_SAMPLE_PERIOD, &IR_Ping, ENABLE);
  
  Blox_IR_Enable_RX_IRQ(IR_NORTH_ID);
  Blox_IR_Enable_RX_IRQ(IR_EAST_ID);
  Blox_IR_Enable_RX_IRQ(IR_SOUTH_ID);
  Blox_IR_Enable_RX_IRQ(IR_WEST_ID);
}

/**
 * @brief Register a receive interrupt for user frames
 * @param id the id of the IR
 * @param IR_User_Handler the function that will be called when a user frame is received
 * @retval None
 */
void Neighbor_Register_IR_RX_IRQ(uint8_t id, void (*IR_User_Handler)(IRFrame *frame)) {
  switch(id) {
    case IR_NORTH_ID:
      IR_North_User_Handler = IR_User_Handler;
      break;
    case IR_EAST_ID:
      IR_East_User_Handler = IR_User_Handler;
      break;
    case IR_SOUTH_ID:
      IR_South_User_Handler = IR_User_Handler;
      break;
    case IR_WEST_ID:
      IR_West_User_Handler = IR_User_Handler;
      break;
  }
}

/**
 * @brief The function that Neighbor Detection registers with timer to ping IRs.
 * @retval None.
 */
void IR_Ping(void) {
  uint8_t i;
  IR_SendFrame(IR_NORTH_ID, IR_FRAME_TYPE_NEIGHBOR, "Yo", 2);
  IR_SendFrame(IR_EAST_ID, IR_FRAME_TYPE_NEIGHBOR, "Sup", 3); 
  IR_SendFrame(IR_SOUTH_ID, IR_FRAME_TYPE_NEIGHBOR, "Hey", 3); 
  IR_SendFrame(IR_WEST_ID, IR_FRAME_TYPE_NEIGHBOR, "Hi", 2); 
  for(i = 0; i < 4; i++) {
    if(neighbor_update_flag[i] == TRUE) {
      neighbor_update_flag[i] = FALSE;
    } else {
      neighbors[i] = FALSE;
      neighbors_id[i] = 0;
    }
  }
  
  for(i = 0; i < 4; i++) {
    if(neighbors[i] == TRUE) {
      USB_SendPat("N: %d\t", neighbors_id[i]);
    }
  }
}

/**
 * @brief The function that Neighbor Detection registers with IR1 to execute on frame received.
 * @retval None.
 */
void IR_North_Neighbor_Handler(IRFrame *frame) {
  if(frame->type == IR_FRAME_TYPE_NEIGHBOR) {
    neighbors[IR_NORTH_ID-1] = TRUE;
    neighbors_id[IR_NORTH_ID-1] = frame->src_id;
    neighbor_update_flag[IR_NORTH_ID-1] = TRUE;
  } else if (frame->type == IR_FRAME_TYPE_USER && IR_North_User_Handler != NULL) {
    (*IR_North_User_Handler)(frame);
  }
  free(frame->data);
  free(frame);
}

/**
 * @brief The function that Neighbor Detection registers with IR2 to execute on frame received.
 * @retval None.
 */
void IR_East_Neighbor_Handler(IRFrame *frame) {
  if(frame->type == IR_FRAME_TYPE_NEIGHBOR) {
    neighbors[IR_EAST_ID-1] = TRUE;
    neighbors_id[IR_EAST_ID-1] = frame->src_id;
    neighbor_update_flag[IR_EAST_ID-1] = TRUE;
  } else if (frame->type == IR_FRAME_TYPE_USER && IR_East_User_Handler != NULL) {
    (*IR_East_User_Handler)(frame);
  }
  free(frame->data);
  free(frame);
}

/**
 * @brief The function that Neighbor Detection registers with IR3 to execute on frame received.
 * @retval None.
 */
void IR_South_Neighbor_Handler(IRFrame *frame) {
  if(frame->type == IR_FRAME_TYPE_NEIGHBOR) {
    neighbors[IR_SOUTH_ID-1] = TRUE;
    neighbors_id[IR_SOUTH_ID-1] = frame->src_id;
    neighbor_update_flag[IR_SOUTH_ID-1] = TRUE;
  } else if (frame->type == IR_FRAME_TYPE_USER && IR_South_User_Handler != NULL) {
    (*IR_South_User_Handler)(frame);
  }
  free(frame->data);
  free(frame);
}

/**
 * @brief The function that Neighbor Detection registers with IR4 to execute on frame received.
 * @retval None.
 */
void IR_West_Neighbor_Handler(IRFrame *frame) {
  if(frame->type == IR_FRAME_TYPE_NEIGHBOR) {
    neighbors[IR_WEST_ID-1] = TRUE;
    neighbors_id[IR_WEST_ID-1] = frame->src_id;
    neighbor_update_flag[IR_WEST_ID-1] = TRUE;
  } else if (frame->type == IR_FRAME_TYPE_USER && IR_West_User_Handler != NULL) {
    (*IR_West_User_Handler)(frame);
  }
  free(frame->data);
  free(frame);
}

/**
 * @brief Gets the neighbor for a given face
 * @retval The id of a neighbor if one is present
 */
uint8_t IR_Get_Neighbor(IR_DIR face_dir) {
  return neighbors_id[(uint8_t)face_dir-1];
}
/** @} */
