/**
 * @file    blox_vusart.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/01/2010
 * @brief   A virtual USART driver for the STM32F103
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

#include "blox_vusart.h"

/**
 * @ingroup driver_vusart
 * @{
 */
/* Private function prototypes */
void Blox_VUSART_RCC_Configuration(uint8_t id);
void Blox_VUSART_GPIO_Configuration(uint8_t id);

TIMER_ID VUSART1_RxDataID = INVALID_TIMER;
TIMER_ID VUSART1_TxDataID = INVALID_TIMER; 
TIMER_ID VUSART2_RxDataID = INVALID_TIMER; 
TIMER_ID VUSART2_TxDataID = INVALID_TIMER;
EXTI_ID VUSART1_RxStartID = EXTI_INVALID_LINE;
EXTI_ID VUSART2_RxStartID = EXTI_INVALID_LINE;
EXTI_ID VUSART1_RXNE_IRQ_ID = EXTI_INVALID_LINE;
uint8_t VUSART1_RXNE_IRQ_Enable = FALSE;
EXTI_ID VUSART2_RXNE_IRQ_ID = EXTI_INVALID_LINE;
uint8_t VUSART2_RXNE_IRQ_Enable = FALSE;

void VUSART1_RxStart(void);
void VUSART1_RxData(void);
void VUSART1_TxData(void);
void VUSART2_RxStart(void);
void VUSART2_RxData(void);
void VUSART2_TxData(void);

uint16_t VUSART1_BaudRate, VUSART1_DoubleBaudRate;
uint8_t VUSART1_TxDataRegister;
uint8_t VUSART1_RxDataRegister;

uint16_t VUSART2_BaudRate, VUSART2_DoubleBaudRate;
uint8_t VUSART2_TxDataRegister;
uint8_t VUSART2_RxDataRegister;

uint8_t VUSART1_TxComplete;      //TC
uint8_t VUSART1_TxEmpty;         //TXE
uint8_t VUSART1_RxNotEmpty;      //RXNE

uint8_t VUSART2_TxComplete;      //TC
uint8_t VUSART2_TxEmpty;         //TXE
uint8_t VUSART2_RxNotEmpty;      //RXNE

uint16_t VUSART1_RxError;
uint16_t VUSART2_RxError;

/**
 * @brief Initializes the virtual USART module.
 * @param id the id of the virtual USART interface.
 * @retval None
 */
void Blox_VUSART_Init(uint8_t id) {
  Blox_VUSART_RCC_Configuration(id);  
  Blox_VUSART_GPIO_Configuration(id);
  Blox_Timer_Init(VUSART_TIMx, VUSART_TIM_CLK);
  NVIC_SetPriority(VUSART_TIM_IRQn, 1);
  Blox_EXTI_Init();
  
  switch(id) {
  case 1: /* XBee */
    //initialize data and flags
    VUSART1_BaudRate = _9600bps;
    VUSART1_DoubleBaudRate = VUSART1_BaudRate / 2;
    VUSART1_TxEmpty = 1;
    VUSART1_TxComplete = 1;
    /* set Tx high while idle */
    VUSART1_GPIO->ODR |= (VUSART1_TxPin);
    VUSART1_RxDataID = Blox_Timer_Register_IRQ(VUSART_TIMx, VUSART1_DoubleBaudRate, &VUSART1_RxData, DISABLE);
    VUSART1_TxDataID = Blox_Timer_Register_IRQ(VUSART_TIMx, VUSART1_BaudRate, &VUSART1_TxData, DISABLE);
    VUSART1_RxStartID = Blox_EXTI_Register_HW_IRQ(VUSART1_RxPortSource, VUSART1_RxPinSource, &VUSART1_RxStart);
    break;
  case 2: /* OLED Display */
    //initialize data and flags
    VUSART2_BaudRate = _38400bps;
    VUSART2_DoubleBaudRate = VUSART2_BaudRate / 2;
    VUSART2_TxEmpty = 1;
    VUSART2_TxComplete = 1;
    /* set Tx high while idle */
    VUSART2_GPIO->ODR |= (VUSART2_TxPin);
    VUSART2_RxDataID = Blox_Timer_Register_IRQ(VUSART_TIMx, VUSART2_DoubleBaudRate, &VUSART2_RxData, DISABLE);
    VUSART2_TxDataID = Blox_Timer_Register_IRQ(VUSART_TIMx, VUSART2_BaudRate, &VUSART2_TxData, DISABLE);
    VUSART2_RxStartID = Blox_EXTI_Register_HW_IRQ(VUSART2_RxPortSource, VUSART2_RxPinSource, &VUSART2_RxStart);
    break;
  }
}

/**
 * @brief Sets the baudrate of the given ID.
 * @param id the id of the virtual USART interface./
 * @param baudrate the baudrate to set to (_9600, etc)
 * @retval None
 */
void Blox_VUSART_SetBaudrate(uint8_t id, uint16_t baudrate) {
  switch(id) {
  case 1: /* XBee */
    VUSART1_BaudRate = baudrate;
    VUSART1_DoubleBaudRate = VUSART1_BaudRate / 2;
    break;
  case 2: /* OLED Display */
    VUSART2_BaudRate = baudrate;
    VUSART2_DoubleBaudRate = VUSART2_BaudRate / 2;
    break;
  }
}

/**
 * @brief Turns on a timer for VUSART1 which samples at the specified baud rate when a falling edge is received
 * @retval None
 */
void VUSART1_RxStart(void) {
  if(VUSART1_RxNotEmpty == 0) {
    Blox_Timer_Enable_IRQ(VUSART1_RxDataID);
    Blox_EXTI_Disable_IRQ(VUSART1_RxStartID);
  } else if (VUSART1_RXNE_IRQ_ID != EXTI_INVALID_LINE && VUSART1_RXNE_IRQ_ID != EXTI_IRQ_UNAVAILABLE && VUSART1_RXNE_IRQ_Enable == TRUE) {
    Blox_EXTI_Trigger_SW_IRQ(VUSART1_RXNE_IRQ_ID); 
  }
}

/**
 * @brief Samples incoming data at the specified baud rate for VUSART1
 * @retval None
 */
void VUSART1_RxData(void) {
  static uint8_t bit_num;
  static uint8_t data;
  if(bit_num == 0) {
    Blox_Timer_Modify_IRQ(VUSART1_RxDataID, VUSART1_BaudRate);
    /* error if start bit != 0 */
    if((VUSART1_GPIO->IDR & VUSART1_RxPin) >> VUSART1_RxPinSource) {
      VUSART1_RxError++;
      Blox_Timer_Disable_IRQ(VUSART1_RxDataID);
      Blox_Timer_Modify_IRQ(VUSART1_RxDataID, VUSART1_DoubleBaudRate);
      Blox_EXTI_Enable_IRQ(VUSART1_RxStartID);
    }
  }
  else if(bit_num <= 8) {
    uint8_t tmp = ((VUSART1_GPIO->IDR & VUSART1_RxPin)>>VUSART1_RxPinSource);
    data |= (tmp << (bit_num-1));
    if(bit_num == 8) {
      VUSART1_RxDataRegister = data;
      VUSART1_RxNotEmpty = 1;
      data = 0;
      Blox_Timer_Disable_IRQ(VUSART1_RxDataID);
      Blox_Timer_Modify_IRQ(VUSART1_RxDataID, VUSART1_DoubleBaudRate);
      Blox_EXTI_Enable_IRQ(VUSART1_RxStartID);  
      if(VUSART1_RXNE_IRQ_ID != EXTI_INVALID_LINE && VUSART1_RXNE_IRQ_ID != EXTI_IRQ_UNAVAILABLE &&
         VUSART1_RXNE_IRQ_Enable == TRUE)  
        Blox_EXTI_Trigger_SW_IRQ(VUSART1_RXNE_IRQ_ID);  
    }
  }
  bit_num = (bit_num + 1) % 9;
}

/**
 * @brief Outputs data at the specified baud rate for VUSART1
 * @retval None
 */
void VUSART1_TxData(void) {
  static uint8_t bit_num;
  static uint8_t data;
  //static uint8_t parity;
  if(bit_num == 0) {
    data = VUSART1_TxDataRegister;
    VUSART1_TxEmpty = 1;
    /* start bit = 0 */
    VUSART1_GPIO->ODR &= ~(VUSART1_TxPin);
  }
  else if(bit_num <= 8) {
    if((data >> (bit_num - 1)) & 0x01) {
      VUSART1_GPIO->ODR |= (VUSART1_TxPin);
    } 
    else {
      VUSART1_GPIO->ODR &= ~(VUSART1_TxPin);
    }
  }
  else if(bit_num == 9) {
    /* stop bit = 1 */
    VUSART1_GPIO->ODR |= (VUSART1_TxPin);
    VUSART1_TxComplete = 1;
    Blox_Timer_Disable_IRQ(VUSART1_TxDataID); 
  }
  bit_num = (bit_num + 1) % 10;
}

/**
 * @brief Turns on a timer for VUSART2 which samples at the specified baud rate when a falling edge is received
 * @retval None
 */
void VUSART2_RxStart(void) {
  if(VUSART2_RxNotEmpty == 0) {
    Blox_Timer_Enable_IRQ(VUSART2_RxDataID);
    Blox_EXTI_Disable_IRQ(VUSART2_RxStartID);
  }
}

/**
 * @brief Samples incoming data at the specified baud rate for VUSART2
 * @retval None
 */
void VUSART2_RxData(void) {
  static uint8_t bit_num;
  static uint8_t data;
  if(bit_num == 0) {
    Blox_Timer_Modify_IRQ(VUSART2_RxDataID, VUSART2_BaudRate);
    /* error if start bit != 0 */
    if((VUSART2_GPIO->IDR & VUSART2_RxPin) >> VUSART2_RxPinSource) {
      VUSART2_RxError++;
      Blox_EXTI_Enable_IRQ(VUSART2_RxStartID);
      Blox_Timer_Disable_IRQ(VUSART2_RxDataID);
      Blox_Timer_Modify_IRQ(VUSART2_RxDataID, VUSART2_DoubleBaudRate);
    }
  }
  else if(bit_num <= 8) {
    uint8_t tmp = ((VUSART2_GPIO->IDR & VUSART2_RxPin)>>VUSART2_RxPinSource);
    data |= (tmp << (bit_num-1));
    if(bit_num == 8) {
      VUSART2_RxDataRegister = data;
      VUSART2_RxNotEmpty = 1;
      data = 0;
      Blox_Timer_Disable_IRQ(VUSART2_RxDataID);
      Blox_Timer_Modify_IRQ(VUSART2_RxDataID, VUSART2_DoubleBaudRate);
      Blox_EXTI_Enable_IRQ(VUSART2_RxStartID);   
      if(VUSART2_RXNE_IRQ_ID != EXTI_INVALID_LINE && VUSART2_RXNE_IRQ_ID != EXTI_IRQ_UNAVAILABLE &&
         VUSART2_RXNE_IRQ_Enable == TRUE)  
        Blox_EXTI_Trigger_SW_IRQ(VUSART2_RXNE_IRQ_ID);  
    }
  }
  bit_num = (bit_num + 1) % 9;
}

/**
 * @brief Outputs data at the specified baud rate for VUSART1
 * @retval None
 */
void VUSART2_TxData(void) {
  static uint8_t bit_num;
  static uint8_t data;
  //static uint8_t parity;
  if(bit_num == 0) {
    data = VUSART2_TxDataRegister;
    VUSART2_TxEmpty = 1;
    /* start bit = 0 */
    VUSART2_GPIO->ODR &= ~(VUSART2_TxPin);
  }
  else if(bit_num <= 8) {
    if((data >> (bit_num - 1)) & 0x01) {
      VUSART2_GPIO->ODR |= (VUSART2_TxPin);
    } 
    else {
      VUSART2_GPIO->ODR &= ~(VUSART2_TxPin);
    }
  }
  else if(bit_num == 9) {
    /* stop bit = 1 */
    VUSART2_GPIO->ODR |= (VUSART2_TxPin);
    VUSART2_TxComplete = 1;
    Blox_Timer_Disable_IRQ(VUSART2_TxDataID); 
  }
  bit_num = (bit_num + 1) % 10;
}

/**
 * @brief Initializes clocks for the given the virtual USART interface.
 * @param id the id of the virtual USART interface.
 * @retval None
 */
void Blox_VUSART_RCC_Configuration(uint8_t id) {
  switch(id) {
  case 1:
    RCC_APB2PeriphClockCmd(VUSART1_GPIO_CLK, ENABLE);
    //RCC_APB2PeriphClockCmd(VUSART1_CLK, ENABLE);
    break;
  case 2:
    RCC_APB2PeriphClockCmd(VUSART2_GPIO_CLK, ENABLE);
    //RCC_APB1PeriphClockCmd(VUSART2_CLK, ENABLE);
    break;      
  }
}

/**
 * @brief Initializes the gpios for the given the virtual USART interface.
 * @param id the id of the virtual USART interface.
 * @retval None
 */
void Blox_VUSART_GPIO_Configuration(uint8_t id) {
  GPIO_InitTypeDef GPIO_InitStructure; 
  //Set Rx as Floating input
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  switch(id) {
    case 1:
      GPIO_InitStructure.GPIO_Pin = VUSART1_RxPin;
      GPIO_Init(VUSART1_GPIO, &GPIO_InitStructure);
      break;
    case 2:
      GPIO_InitStructure.GPIO_Pin = VUSART2_RxPin;
      GPIO_Init(VUSART2_GPIO, &GPIO_InitStructure);
      break;
  }
  //Set Tx as 50Mhz output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  switch(id) {
    case 1:
      GPIO_InitStructure.GPIO_Pin = VUSART1_TxPin;
      GPIO_Init(VUSART1_GPIO, &GPIO_InitStructure);
      break;
    case 2:
      GPIO_InitStructure.GPIO_Pin = VUSART2_TxPin;
      GPIO_Init(VUSART2_GPIO, &GPIO_InitStructure);
      break;
  }
}

/**
 * @brief Tries to receive a byte on the given virtual USART.
 * @param id the virtual USART id to use.
 * @param data a pointer to the location the data will be returned
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_TryReceive(uint8_t id, uint8_t *data) {
  //uint8_t data;
  switch(id) {
    case 1:
    //return Virtual USART2 Rx FIFO
      if(VUSART1_RxNotEmpty == 1) {
        *data = VUSART1_RxDataRegister;
        VUSART1_RxNotEmpty = 0;
        return VUSART_SUCCESS;
      }
      else {
        return RX_EMPTY;
      }
    case 2:
    //return Virtual USART2 Rx FIFO
      if(VUSART2_RxNotEmpty == 1) {
        *data = VUSART2_RxDataRegister;
        VUSART2_RxNotEmpty = 0;
        return VUSART_SUCCESS;
      }
      else {
        return RX_EMPTY;
      }
    default:
      return INVALID_ID;
  } 
}

/**
 * @brief Tries to send a byte out on the given virtual USART
 * @param id the virtual USART id to use
 * @param data the byte to send
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_TrySend(uint8_t id, uint8_t data) {
  switch(id) {
    case 1:
      /* check for transmit data register empty and transmission complete */
      if(VUSART1_TxEmpty == 1 && VUSART1_TxComplete == 1) {
        VUSART1_TxDataRegister = data;
        VUSART1_TxEmpty = 0;
        VUSART1_TxComplete = 0;
        Blox_Timer_Enable_IRQ(VUSART1_TxDataID);
        return VUSART_SUCCESS;
      }
  	  else {
        return TX_BUSY;
  	  }
    case 2:
      /* check for transmit data register empty and transmission complete */
      if(VUSART2_TxEmpty == 1 && VUSART2_TxComplete == 1) {
        VUSART2_TxDataRegister = data;
        VUSART2_TxEmpty = 0;
        VUSART2_TxComplete = 0;
        Blox_Timer_Enable_IRQ(VUSART2_TxDataID);
        return VUSART_SUCCESS;
      }
  	  else {
        return TX_BUSY;
  	  }
    default:
      return INVALID_ID;
  }
}

/**
 * @brief Receives a blocking byte on the given virtual USART.
 * @param id the virtual USART id to use.
 * @param data a pointer to the location the data will be returned
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_Receive(uint8_t id, uint8_t *data) {
  while (Blox_VUSART_TryReceive(id, data) == RX_EMPTY);
  return VUSART_SUCCESS;
}

/**
 * @brief Sends a blocking byte out on the given virtual USART
 * @param id the virtual USART id to use
 * @param data the byte to send
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_Send(uint8_t id, uint8_t data) {
  while (Blox_VUSART_TrySend(id, data) == TX_BUSY);
  return VUSART_SUCCESS;
}

/**
 * @brief Sends a blocking byte out on the given virtual USART
 * @param id the virtual USART id to use
 * @param data the byte to send
 * @param len the length of the data
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_SendData(uint8_t id, uint8_t *data, uint32_t len) {
  VUSART_STATUS ret;
  int i;
  for (i = 0; i < len; i++) {
    if ((ret = Blox_VUSART_Send(id, data[i])) != VUSART_SUCCESS)
      return ret;
  }
  return VUSART_SUCCESS;
}

/**
 * @brief Registers a function to be called in the SWInterrupt that occurs when a receive happens.
 * @param id the virtual USART id to use.
 * @param RXNE_Handler the function to be called.
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_Register_RXNE_IRQ(uint8_t id, void (*RXNE_Handler)(void)) {
  switch(id) {
    case 1:
      VUSART1_RXNE_IRQ_ID = Blox_EXTI_Register_SW_IRQ(RXNE_Handler);
      if(VUSART1_RXNE_IRQ_ID == EXTI_IRQ_UNAVAILABLE)
        return RXNE_IRQ_UNAVAILABLE;
      return VUSART_SUCCESS;
    case 2:
      VUSART2_RXNE_IRQ_ID = Blox_EXTI_Register_SW_IRQ(RXNE_Handler);
      if(VUSART2_RXNE_IRQ_ID == EXTI_IRQ_UNAVAILABLE)
        return RXNE_IRQ_UNAVAILABLE;
      return VUSART_SUCCESS;
    default:
      return INVALID_ID;
  }
}

/**
 * @brief Enables the SW Interrupt on RXNE.
 * @param id the virtual USART id to use.
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_Enable_RXNE_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      VUSART1_RXNE_IRQ_Enable = TRUE;
      return VUSART_SUCCESS;
    case 2:
       VUSART2_RXNE_IRQ_Enable = TRUE;
      return VUSART_SUCCESS;
    default:
      return INVALID_ID;
  }
}

/**
 * @brief Disables the SW Interrupt on RXNE.
 * @param id the virtual USART id to use.
 * @retval The current status of the VUSART.
 */
VUSART_STATUS Blox_VUSART_Disable_RXNE_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      VUSART1_RXNE_IRQ_Enable = FALSE;
      return VUSART_SUCCESS;
    case 2:
       VUSART2_RXNE_IRQ_Enable = FALSE;
      return VUSART_SUCCESS;
    default:
      return INVALID_ID;
  }
}
/** @} */
