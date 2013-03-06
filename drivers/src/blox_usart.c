/**
 * @file    blox_usart.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/18/2010
 * @brief   A very basic wrapper around the USARTs on the STM32F103
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

#include "blox_usart.h"

/**
 * @ingroup driver_usart
 * @{
 */
/* Private function prototypes */
void Blox_USART_DeInit_USART(void);
void Blox_USART_DeInit_GPIO(void);
void Blox_USART_RCC_Configuration(uint8_t id);
void Blox_USART_GPIO_Configuration(uint8_t id);
void Blox_USART_NVIC_Configuration(uint8_t id);

/**
 * @brief Array of handlers to call on interrupt
 */
void (*USARTn_Handler[5])(void) = {NULL};

/**
 * @brief Initializes the USART module.
 * @param id: the id of the USART interface.
 * @retval None
 */
void Blox_USART_Init(uint8_t id) {
  USART_InitTypeDef USART_InitStructure;
  Blox_USART_RCC_Configuration(id);
  Blox_USART_GPIO_Configuration(id);  
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  switch(id) {
  case 1:
    USART_Init(USART1, &USART_InitStructure);  
    USART_Cmd(USART1, ENABLE);
    break;
  case 2: /* IR 3 */
    USART_Init(USART2, &USART_InitStructure);    
    USART_Cmd(USART2, ENABLE);
    USART_SetPrescaler(USART2, 0x1);
    USART_IrDAConfig(USART2, USART_IrDAMode_LowPower);
	  USART_IrDACmd(USART2, ENABLE);	
    break;
  case 3: /* IR 4 */
    USART_Init(USART3, &USART_InitStructure);  
    USART_Cmd(USART3, ENABLE);
    USART_SetPrescaler(USART3, 0x1);
    USART_IrDAConfig(USART3, USART_IrDAMode_LowPower);
	  USART_IrDACmd(USART3, ENABLE);
    break;
  case 4: /* IR 1 */
    USART_Init(UART4, &USART_InitStructure);  
    USART_Cmd(UART4, ENABLE);
    USART_SetPrescaler(UART4, 0x1);
    USART_IrDAConfig(UART4, USART_IrDAMode_LowPower);
	  USART_IrDACmd(UART4, ENABLE);	
    break;
  case 5: /* IR 2 */
    USART_Init(UART5, &USART_InitStructure);  
    USART_Cmd(UART5, ENABLE);
    USART_SetPrescaler(UART5, 0x1);
    USART_IrDAConfig(UART5, USART_IrDAMode_LowPower);
	  USART_IrDACmd(UART5, ENABLE);	
    break;
  }
  
  Blox_System_Register_DeInit(&RCC_DeInit);
  Blox_System_Register_DeInit(&Blox_USART_DeInit_USART);
  Blox_System_Register_DeInit(&Blox_USART_DeInit_GPIO);   
}

/**
 * @brief De-initializes all the USART interfaces. 
 * @retval None
 */
void Blox_USART_DeInit_USART(void) {
  USART_DeInit(USART1);
  USART_DeInit(USART2);
  USART_DeInit(USART3);
  USART_DeInit(UART4);
  USART_DeInit(UART5); 
}
/**
 * @brief De-initializes the GPIOs for all the USART interfaces. 
 * @retval None
 */
void Blox_USART_DeInit_GPIO(void) {
  GPIO_DeInit(USART2_GPIO);
  GPIO_DeInit(USART3_GPIO);
  GPIO_DeInit(UART4_GPIO);
  GPIO_DeInit(UART5_GPIO_RX);
  GPIO_DeInit(UART5_GPIO_TX);
}

/**
 * @brief Initializes clocks for the given the USART interface.
 * @param id the id of the USART interface.
 * @retval None
 */
void Blox_USART_RCC_Configuration(uint8_t id) {
  switch(id) {
  case 1:
    RCC_APB2PeriphClockCmd(USART1_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE);
    break;
  case 2:
    RCC_APB2PeriphClockCmd(USART2_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(USART2_CLK, ENABLE);
    break;
  case 3:
    RCC_APB2PeriphClockCmd(USART3_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(USART3_CLK, ENABLE);
    break;
  case 4:
    RCC_APB2PeriphClockCmd(UART4_GPIO_CLK, ENABLE);;
    RCC_APB1PeriphClockCmd(UART4_CLK, ENABLE);
    break;
  case 5:
    RCC_APB2PeriphClockCmd(UART5_GPIO_TX_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(UART5_GPIO_RX_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(UART5_CLK, ENABLE);
    break;        
  }
}

/**
 * @brief Initializes the gpios for the given the USART interface.
 * @param id the id of the USART interface.
 * @retval None
 */
void Blox_USART_GPIO_Configuration(uint8_t id) {
GPIO_InitTypeDef GPIO_InitStructure; 
  //Set up Rx as Floating
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  switch(id) {
    case 1:
      GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
      GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
      break;
   case 2:
      GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
      GPIO_Init(USART2_GPIO, &GPIO_InitStructure);
      break;
   case 3:
      GPIO_InitStructure.GPIO_Pin = USART3_RxPin;
      GPIO_Init(USART3_GPIO, &GPIO_InitStructure);
      break;
   case 4:
      GPIO_InitStructure.GPIO_Pin = UART4_RxPin;
      GPIO_Init(UART4_GPIO, &GPIO_InitStructure);
      break;
   case 5:
      GPIO_InitStructure.GPIO_Pin = UART5_RxPin;
      GPIO_Init(UART5_GPIO_RX, &GPIO_InitStructure);
      break;
  }
  
  //Set Tx as 50Mhz and Floating
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  switch(id) {
  case 1:
    GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
    GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
    break;
  case 2:
    GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
    GPIO_Init(USART2_GPIO, &GPIO_InitStructure);
    break;
  case 3:
    GPIO_InitStructure.GPIO_Pin = USART3_TxPin;
    GPIO_Init(USART3_GPIO, &GPIO_InitStructure);
    break;
  case 4:
    GPIO_InitStructure.GPIO_Pin = UART4_TxPin;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);
    break;
  case 5:
    GPIO_InitStructure.GPIO_Pin = UART5_TxPin;
    GPIO_Init(UART5_GPIO_TX, &GPIO_InitStructure);
    break;
  }
}

/**
 * @brief Initializes the NVIC for the given the USART interface.
 * @param id the id of the USART interface.
 * @retval None
 */
void Blox_USART_NVIC_Configuration(uint8_t id) {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  switch(id) {
    case 1:
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      break;
    case 2:
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      break;
    case 3:
      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
      break;
    case 4:
      NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
      break;
    case 5:
      NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
      break;
  }
  NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief Receive a byte on the given USART.
 * @param id the USART id to use.
 * @retval The received byte.
 */
uint8_t Blox_USART_Receive(uint8_t id) {
  switch(id) {
  case 1:
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;
    return USART_ReceiveData(USART1) & 0xFF;
  case 2:
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) ;
    return USART_ReceiveData(USART2) & 0xFF;
  case 3:
    while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET) ;
    return USART_ReceiveData(USART3) & 0xFF;
  case 4:
    while(USART_GetFlagStatus( UART4, USART_FLAG_RXNE) == RESET) ;
    return USART_ReceiveData(UART4) & 0xFF;
  case 5:
    while(USART_GetFlagStatus( UART5, USART_FLAG_RXNE) == RESET) ;
    return USART_ReceiveData(UART5) & 0xFF;
  }
  return 0;
}

/**
 * @brief Receive a byte on the given USART.
 * @param id the USART id to use.
 * @retval The received byte.
 */
int16_t Blox_USART_TryReceive(uint8_t id) {
  switch(id) {
  case 1:
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
      return -1;
    else
      return USART_ReceiveData(USART1) & 0xFF;
  case 2:
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
      return -1;
    else
      return USART_ReceiveData(USART2) & 0xFF;
  case 3:
    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
      return -1;
    else
      return USART_ReceiveData(USART3) & 0xFF;
  case 4:
    if (USART_GetFlagStatus( UART4, USART_FLAG_RXNE) == RESET)
      return -1;
    else
      return USART_ReceiveData(UART4) & 0xFF;
  case 5:
    if (USART_GetFlagStatus( UART5, USART_FLAG_RXNE) == RESET)
      return -1;
    else
      return USART_ReceiveData(UART5) & 0xFF;
  }
  return -1;
}

/**
 * @brief Sends a byte out on the given USART
 * @param id the USART id to use
 * @param data the byte to send
 * @retval None.
 */
void Blox_USART_Send(uint8_t id, uint8_t data) {
  switch(id) {
  case 1:
  	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;
    USART_SendData(USART1, data);
    break;
  case 2:
  	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) ;
    USART_SendData(USART2, data);
    break;
  case 3:
	  while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) ;
    USART_SendData(USART3, data);
    break;
  case 4:
  	while (USART_GetFlagStatus( UART4, USART_FLAG_TXE) == RESET) ;
    USART_SendData(UART4, data);
    break;
  case 5:
  	while (USART_GetFlagStatus( UART5, USART_FLAG_TXE) == RESET) ;
    USART_SendData(UART5, data);
    break;
  }
}

/**
 * @brief Registers a USART Interrupt on RXNE.
 * @param id the USART id to use.
 * @param RXNE_Handler the handler function for the USART RXNE IRQ.
 * @retval The current status of the USART.
 */
void Blox_USART_Register_RXNE_IRQ(uint8_t id, void (*RXNE_Handler)(void)) {
  if(USARTn_Handler[id-1] == NULL) {
    USARTn_Handler[id-1] = RXNE_Handler;
  } else {
    //IRQ already set
  }
  Blox_USART_Disable_RXNE_IRQ(id);
  Blox_USART_NVIC_Configuration(id);
}

/**
 * @brief Releases a USART Interrupt on RXNE.
 * @param id the USART id to use.
 * @retval The current status of the USART.
 */
void Blox_USART_Release_RXNE_IRQ(uint8_t id) {
  Blox_USART_Disable_RXNE_IRQ(id);
  USARTn_Handler[id-1] = NULL;
}

/**
 * @brief Disables the USART Interrupt on RXNE.
 * @param id the USART id to use.
 * @retval The current status of the USART.
 */
void Blox_USART_Enable_RXNE_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
      break;
    case 2:
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
      break;
    case 3:
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
      break;
    case 4:
      USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
      break;
    case 5:
      USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
      break;
  }
}

/**
 * @brief Disables the USART Interrupt on RXNE.
 * @param id the USART id to use.
 * @retval The current status of the USART.
 */
void Blox_USART_Disable_RXNE_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      break;
    case 2:
      USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
      break;
    case 3:
      USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
      break;
    case 4:
      USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
      break;
    case 5:
      USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
      break;
  }
}



/**
  * @brief  This function handles USART1 interrupt request.
  * @retval None
  */
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    if(USARTn_Handler[0] != NULL) {
      (*USARTn_Handler[0])();
    }
  }
}

/**
  * @brief  This function handles USART2 interrupt request.
  * @retval None
  */
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    if(USARTn_Handler[1] != NULL) {
      (*USARTn_Handler[1])();;
    }
  }
}

/**
  * @brief  This function handles USART3 interrupt request.
  * @retval None
  */
void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    if(USARTn_Handler[2] != NULL) {
      (*USARTn_Handler[2])();;
    }
  }
}

/**
  * @brief  This function handles UART4 interrupt request.
  * @retval None
  */
void UART4_IRQHandler(void)
{
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  {
    if(USARTn_Handler[3] != NULL) {
      (*USARTn_Handler[3])();;
    }
  }
}

/**
  * @brief  This function handles UART5 interrupt request.
  * @retval None
  */
void UART5_IRQHandler(void)
{
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  {
    if(USARTn_Handler[4] != NULL) {
      (*USARTn_Handler[4])();;
    }
  }
}
