/**
 * @file    blox_exti.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    10/20/2010
 * @brief   A wrapper around EXTI for the STM32F103
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

#include "blox_exti.h"
 
/**
 * @ingroup driver_exti
 * @{ */
#define XBEE_EXTI_LINE    12
#define OLED_EXTI_LINE    1
#define TOUCH1_EXTI_LINE  10
#define TOUCH2_EXTI_LINE  11
#define TOUCH3_EXTI_LINE  14
#define TOUCH4_EXTI_LINE  7

/**
 * @brief Array of pointers for handlers for all 16 EXTI interrupts.
 */
void (*EXTIn_Handler[16])(void) = {NULL};

void Blox_EXTI_RCC_Configuration(void);
void Blox_EXTI_NVIC_Configuration (uint8_t line);
uint8_t isHardwareLine(uint8_t line);

/**
 * @brief Initializes EXTI.
 * @retval None
 */
void Blox_EXTI_Init(void) {
  Blox_EXTI_RCC_Configuration();
  
  Blox_System_Register_DeInit(&RCC_DeInit);
  Blox_System_Register_DeInit(&EXTI_DeInit);
}

/**
 * @brief Registers an EXTI IRQ that triggers on hardware.
 * @param GPIO_PortSource selects the GPIO port to be used as source for EXTI lines.
 *   This parameter can be GPIO_PortSourceGPIOx where x can be (A..G).
 * @param line specifies the EXTI line to be configured.
 *   This parameter can be (1,7,10,11,12,14)
 * @param EXTI_Handler the handler function for the EXTI line.
 * @retval EXTI0-EXTI15 on success.
 * @retval EXTI_INVALID_LINE if an invalid line is requested.
 * @retval EXTI_IRQ_UNAVAILABLE if an interrupt can't be used.
 */
EXTI_ID Blox_EXTI_Register_HW_IRQ(uint8_t GPIO_PortSource, uint8_t line, void (*EXTI_Handler)(void)) {
  if(isHardwareLine(line) == TRUE) {
    EXTI_InitTypeDef EXTI_InitStructure;
    Blox_EXTI_NVIC_Configuration(line);
    GPIO_EXTILineConfig(GPIO_PortSource, line);
    EXTI_InitStructure.EXTI_Line = (1<<line);
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);
    EXTIn_Handler[line] = EXTI_Handler;
    return (EXTI_ID)(line);
  }
  return EXTI_INVALID_LINE;
}

/**
 * @brief Registers an EXTI IRQ that triggers on software.
 * @param EXTI_Handler the handler function for the EXTI IRQ.
 * @retval EXTI0-EXTI15 on success.
 * @retval EXTI_INVALID_LINE if an invalid line is requested.
 * @retval EXTI_IRQ_UNAVAILABLE if an interrupt can't be used.
 */
EXTI_ID Blox_EXTI_Register_SW_IRQ(void (*EXTI_Handler)(void)) {
  uint8_t line;
  EXTI_InitTypeDef EXTI_InitStructure;
  for(line = 0; line <= 15; line++) {
    if(isHardwareLine(line) == FALSE && EXTIn_Handler[line] == NULL)
      break;
  }
  if (line > 15)
    return EXTI_IRQ_UNAVAILABLE;
  Blox_EXTI_NVIC_Configuration(line);
  EXTI_InitStructure.EXTI_Line = (1<<line);
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_Init(&EXTI_InitStructure);
  EXTIn_Handler[line] = EXTI_Handler;
  return (EXTI_ID)(line);
}

/**
 * @brief Releases a given EXTI interrupt
 * @param id specifies the id of the EXTI interrupt
 * @retval None
 */
void Blox_EXTI_Release_IRQ(EXTI_ID id) {
  Blox_EXTI_Disable_IRQ(id);
  EXTIn_Handler[id] = NULL;
}

/**
 * @brief Registers an EXTI IRQ that triggers on software.
 * @param id the ID of the EXTI IRQ that is to be triggered
 * @retval None
 */
void Blox_EXTI_Trigger_SW_IRQ(EXTI_ID id) {
  if(isHardwareLine(id) == FALSE) {
    EXTI_GenerateSWInterrupt(1<<id);
  }
}

/**
 * @brief Checks whether a line is a designated hardware line.
 * @param line specifies the EXTI line
 * @retval 1 if the line is a designated hardware line and 0 otherwise
 */
uint8_t isHardwareLine(uint8_t line) {
  return (line == XBEE_EXTI_LINE || line == OLED_EXTI_LINE || 
      line == TOUCH1_EXTI_LINE || line == TOUCH2_EXTI_LINE || 
      line == TOUCH3_EXTI_LINE || line == TOUCH4_EXTI_LINE);
}

/**
 * @brief Initializes AFIO clock for the EXTI interface.
 * @retval None
 */
void Blox_EXTI_RCC_Configuration(void) {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

/**
 * @brief Initializes NVIC for the EXTI interface.
 * @param line: specifies the EXTI line to be configured.
 *   This parameter can be (0..15).
 * @retval None
 */
void Blox_EXTI_NVIC_Configuration (uint8_t line) {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  switch(line) {
    case 0:
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
      NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
      break;
    case 1:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
      break;
    case 2:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
      break;
    case 3:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
      break;
    case 4:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
      break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
      break;
    case 10:
    case 11:
    case 13:
    case 12:
    case 14:
    case 15:
      NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
      break;
  }
  NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief 	Enables a given EXTI IRQ.
 * @param 	id the EXTI_ID for the given EXTI IRQ
 * @retval 	None
 */
void Blox_EXTI_Enable_IRQ(EXTI_ID id) {
  EXTI_ClearITPendingBit(1<<id); 
  EXTI->IMR |= (1<<id);
}

/**
 * @brief 	Disables a given EXTI IRQ.
 * @param 	id the EXTI_ID for the given EXTI IRQ
 * @retval 	None
 */
void Blox_EXTI_Disable_IRQ(EXTI_ID id) {
  EXTI->IMR &= ~(1<<id);
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @retval None
  */
void EXTI0_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
    if(EXTIn_Handler[0] != NULL) {
      (*EXTIn_Handler[0])();
    }
    EXTI_ClearITPendingBit(EXTI_Line0);          
  } 
}

/**
  * @brief  This function handles External line 1 interrupt request.
  * @retval None
  */
void EXTI1_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
    if(EXTIn_Handler[1] != NULL) {
      (*EXTIn_Handler[1])();
    }
    EXTI_ClearITPendingBit(EXTI_Line1);          
  } 
}

/**
  * @brief  This function handles External line 2 interrupt request.
  * @retval None
  */
void EXTI2_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line2) != RESET) {
    if(EXTIn_Handler[2] != NULL) {
      (*EXTIn_Handler[2])();
    }
    EXTI_ClearITPendingBit(EXTI_Line2);          
  } 
}

/**
  * @brief  This function handles External line 3 interrupt request.
  * @retval None
  */
void EXTI3_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
    if(EXTIn_Handler[3] != NULL) {
      (*EXTIn_Handler[3])();
    }
    EXTI_ClearITPendingBit(EXTI_Line3);          
  } 
}

/**
  * @brief  This function handles External line 4 interrupt request.
  * @retval None
  */
void EXTI4_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line4) != RESET) {
    if(EXTIn_Handler[4] != NULL) {
      (*EXTIn_Handler[4])();
    }
    EXTI_ClearITPendingBit(EXTI_Line4);          
  } 
}

/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @retval None
  */
void EXTI9_5_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
    if(EXTIn_Handler[5] != NULL) {
      (*EXTIn_Handler[5])();
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
  }
  else if(EXTI_GetITStatus(EXTI_Line6) != RESET) {
    if(EXTIn_Handler[6] != NULL) {
      (*EXTIn_Handler[6])();
    }
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
  else if(EXTI_GetITStatus(EXTI_Line7) != RESET) {
    if(EXTIn_Handler[7] != NULL) {
      (*EXTIn_Handler[7])();
    }
    EXTI_ClearITPendingBit(EXTI_Line7);
  }
  else if(EXTI_GetITStatus(EXTI_Line8) != RESET) {
    if(EXTIn_Handler[8] != NULL) {
      (*EXTIn_Handler[8])();
    }
    EXTI_ClearITPendingBit(EXTI_Line8);
  }
  else if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
    if(EXTIn_Handler[9] != NULL) {
      (*EXTIn_Handler[9])();
    }
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}
 
/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @retval None
  */
void EXTI15_10_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line10) != RESET) {
    if(EXTIn_Handler[10] != NULL) {
      (*EXTIn_Handler[10])();
    }
    EXTI_ClearITPendingBit(EXTI_Line10);
  }
  else if(EXTI_GetITStatus(EXTI_Line11) != RESET) {
    if(EXTIn_Handler[11] != NULL) {
      (*EXTIn_Handler[11])();
    }
    EXTI_ClearITPendingBit(EXTI_Line11); 
  }
  else if(EXTI_GetITStatus(EXTI_Line12) != RESET) {
    if(EXTIn_Handler[12] != NULL) {
      (*EXTIn_Handler[12])();
    }
    EXTI_ClearITPendingBit(EXTI_Line12); 
  }
  else if(EXTI_GetITStatus(EXTI_Line13) != RESET) {
    if(EXTIn_Handler[13] != NULL) {
      (*EXTIn_Handler[13])();
    }
    EXTI_ClearITPendingBit(EXTI_Line13); 
  }
  else if(EXTI_GetITStatus(EXTI_Line14) != RESET) {
    if(EXTIn_Handler[14] != NULL) {
      (*EXTIn_Handler[14])();
    }
    EXTI_ClearITPendingBit(EXTI_Line14); 
  }
  else if(EXTI_GetITStatus(EXTI_Line15) != RESET) {
    if(EXTIn_Handler[15] != NULL) {
      (*EXTIn_Handler[15])();
    }
    EXTI_ClearITPendingBit(EXTI_Line15); 
  }
}
/** @} */

