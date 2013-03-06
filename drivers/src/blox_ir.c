/**
 * @file    blox_ir.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/19/2010
 * @brief   A wrapper class for the IR sensors that use a USART interface.
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

#include "blox_ir.h"

/**
 * @ingroup driver_ir
 * @{
 */

/* Private function prototypes */
void IR_RCC_Configuration(void);
void IR_GPIO_Configuration(void);

void Blox_IR1_USART_RXNE_IRQ(void);
void Blox_IR2_USART_RXNE_IRQ(void);
void Blox_IR3_USART_RXNE_IRQ(void);
void Blox_IR4_USART_RXNE_IRQ(void);

/**
 * @brief Handler for users to register a function with in IR1
 */
void (*IR1_RX_Handler)(IRFrame *frame) = NULL;
/**
 * @brief Handler for users to register a function with in IR2
 */
void (*IR2_RX_Handler)(IRFrame *frame) = NULL;
/**
 * @brief Handler for users to register a function with in IR3
 */
void (*IR3_RX_Handler)(IRFrame *frame) = NULL;
/**
 * @brief Handler for users to register a function with in IR4
 */
void (*IR4_RX_Handler)(IRFrame *frame) = NULL;


/**
 * @brief Flag to determine if a user register gets called in an interrupt for IR1
 */
uint8_t IR1_RX_Enable = FALSE;
/**
 * @brief Flag to determine if a user register gets called in an interrupt for IR2
 */
uint8_t IR2_RX_Enable = FALSE;
/**
 * @brief Flag to determine if a user register gets called in an interrupt for IR3
 */
uint8_t IR3_RX_Enable = FALSE;
/**
 * @brief Flag to determine if a user register gets called in an interrupt for IR4
 */
uint8_t IR4_RX_Enable = FALSE;

/**
 * @brief Initializes the IR module. Basically a wrapper on USART
 * @param id the id of the USART interface to initialize.
 * @retval None
 */
void IR_Init(uint8_t id) {
  IR_RCC_Configuration();
  IR_GPIO_Configuration();
  IR_Wake();
  
  switch(id) {
  case 1:
    Blox_USART_Init(IR_1_USART_ID);
    Blox_USART_Register_RXNE_IRQ(IR_1_USART_ID, &Blox_IR1_USART_RXNE_IRQ);
    break;
  case 2:
    Blox_USART_Init(IR_2_USART_ID);
    Blox_USART_Register_RXNE_IRQ(IR_2_USART_ID, &Blox_IR2_USART_RXNE_IRQ);
    break;
  case 3:
    Blox_USART_Init(IR_3_USART_ID);
    Blox_USART_Register_RXNE_IRQ(IR_3_USART_ID, &Blox_IR3_USART_RXNE_IRQ);
    break;
  case 4:
    Blox_USART_Init(IR_4_USART_ID);
    Blox_USART_Register_RXNE_IRQ(IR_4_USART_ID, &Blox_IR4_USART_RXNE_IRQ);
    break;
  }
}

/**
 * @brief Initializes clocks for IR shutdown pin.
 * @retval None
 */
void IR_RCC_Configuration() {
  RCC_APB2PeriphClockCmd(IR_SHUTDOWN_GPIO_CLK, ENABLE);
}

/**
 * @brief The function that IR1 registers with USART to execute on byte received.
 * @retval None.
 */
void Blox_IR1_USART_RXNE_IRQ(void) {
  static uint8_t num = 0;
  static uint8_t checksum = 0;
  static IRFrame frame;
  uint8_t data;
  int16_t temp;
  
  temp = Blox_USART_TryReceive(IR_1_USART_ID);
  if (temp == -1)
    return;
  
  data = (uint8_t) temp;   
  if (num == 0) {
    if (data == 0x7E) {
      num = 1; //Start of a frame
      checksum = 0;
    }
  } else if (num == 1) {
    frame.src_id = data;
    num++;
  } else if (num == 2) {
    frame.src_face_id = data;
    num++;
  } else if (num == 3) {
    frame.type = (IRFrameType)(data & 0xFF);
    num++;
  } else if (num == 4) {
    frame.len = data;
    if (frame.len > IR_MAX_FRAME_LEN) {
      num = 0;
    } else {
      frame.data = (uint8_t *)malloc(frame.len*sizeof(uint8_t));
      num++;
    }
  } else if (num > 4) {
    if (num == frame.len+5) {
      if(checksum == data) {
        if (IR1_RX_Handler != NULL && IR1_RX_Enable == TRUE) {
          IRFrame *retFrame;
          retFrame = (IRFrame *)malloc(sizeof(IRFrame));
          memcpy(retFrame, &(frame), sizeof(IRFrame));
          (*IR1_RX_Handler)(retFrame);
        } else {
          free(frame.data);
        }
      } else {
        free(frame.data);
      }
      num = 0;
    } else {
      frame.data[num-5] = data;
      checksum ^= data;
      num++;
    }
  }
}

/**
 * @brief The function that IR2 registers with USART to execute on byte received.
 * @retval None.
 */
void Blox_IR2_USART_RXNE_IRQ(void) {
  static uint8_t num = 0;
  static uint8_t checksum = 0;
  static IRFrame frame;
  uint8_t data;
  int16_t temp;
  
  temp = Blox_USART_TryReceive(IR_2_USART_ID);
  if (temp == -1)
    return;
  
  data = (uint8_t) temp;   
  if (num == 0) {
    if (data == 0x7E) {
      num = 1; //Start of a frame
      checksum = 0;
    }
  } else if (num == 1) {
    frame.src_id = data;
    num++;
  } else if (num == 2) {
    frame.src_face_id = data;
    num++;
  } else if (num == 3) {
    frame.type = (IRFrameType)(data & 0xFF);
    num++;
  } else if (num == 4) {
    frame.len = data;
    if (frame.len > IR_MAX_FRAME_LEN) {
      num = 0;
    } else {
      frame.data = (uint8_t *)malloc(frame.len*sizeof(uint8_t));
      num++;
    }
  } else if (num > 4) {
    if (num == frame.len+5) {
      if(checksum == data) {
        if (IR2_RX_Handler != NULL && IR2_RX_Enable == TRUE) {
          IRFrame *retFrame;
          retFrame = (IRFrame *)malloc(sizeof(IRFrame));
          memcpy(retFrame, &(frame), sizeof(IRFrame));
          (*IR2_RX_Handler)(retFrame);
        } else {
          free(frame.data);
        }
      } else {
        free(frame.data);
      }
      num = 0;
    } else {
      frame.data[num-5] = data;
      checksum ^= data;
      num++;
    }
  }
}

/**
 * @brief The function that IR3 registers with USART to execute on byte received.
 * @retval None.
 */
void Blox_IR3_USART_RXNE_IRQ(void) {
  static uint8_t num = 0;
  static uint8_t checksum = 0;
  static IRFrame frame;
  uint8_t data;
  int16_t temp;
  
  temp = Blox_USART_TryReceive(IR_3_USART_ID);
  if (temp == -1)
    return;
  
  data = (uint8_t) temp;   
  if (num == 0) {
    if (data == 0x7E) {
      num = 1; //Start of a frame
      checksum = 0;
    }
  } else if (num == 1) {
    frame.src_id = data;
    num++;
  } else if (num == 2) {
    frame.src_face_id = data;
    num++;
  } else if (num == 3) {
    frame.type = (IRFrameType)(data & 0xFF);
    num++;
  } else if (num == 4) {
    frame.len = data;
    if (frame.len > IR_MAX_FRAME_LEN) {
      num = 0;
    } else {
      frame.data = (uint8_t *)malloc(frame.len*sizeof(uint8_t));
      num++;
    }
  } else if (num > 4) {
    if (num == frame.len+5) {
      if(checksum == data) {
        if (IR3_RX_Handler != NULL && IR3_RX_Enable == TRUE) {
          IRFrame *retFrame;
          retFrame = (IRFrame *)malloc(sizeof(IRFrame));
          memcpy(retFrame, &(frame), sizeof(IRFrame));
          (*IR3_RX_Handler)(retFrame);
        } else {
          free(frame.data);
        }
      } else {
        free(frame.data);
      }
      num = 0;
    } else {
      frame.data[num-5] = data;
      checksum ^= data;
      num++;
    }
  }
}

/**
 * @brief The function that IR4 registers with USART to execute on byte received.
 * @retval None.
 */
void Blox_IR4_USART_RXNE_IRQ(void) {
  static uint8_t num = 0;
  static uint8_t checksum = 0;
  static IRFrame frame;
  uint8_t data;
  int16_t temp;
  
  temp = Blox_USART_TryReceive(IR_4_USART_ID);
  if (temp == -1)
    return;
  
  data = (uint8_t) temp;   
  if (num == 0) {
    if (data == 0x7E) {
      num = 1; //Start of a frame
      checksum = 0;
    }
  } else if (num == 1) {
    frame.src_id = data;
    num++;
  } else if (num == 2) {
    frame.src_face_id = data;
    num++;
  } else if (num == 3) {
    frame.type = (IRFrameType)(data & 0xFF);
    num++;
  } else if (num == 4) {
    frame.len = data;
    if (frame.len > IR_MAX_FRAME_LEN) {
      num = 0;
    } else {
      frame.data = (uint8_t *)malloc(frame.len*sizeof(uint8_t));
      num++;
    }
  } else if (num > 4) {
    if (num == frame.len+5) {
      if(checksum == data) {
        if (IR4_RX_Handler != NULL && IR4_RX_Enable == TRUE) {
          IRFrame *retFrame;
          retFrame = (IRFrame *)malloc(sizeof(IRFrame));
          memcpy(retFrame, &(frame), sizeof(IRFrame));
          (*IR4_RX_Handler)(retFrame);
        } else {
          free(frame.data);
        }
      } else {
        free(frame.data);
      }
      num = 0;
    } else {
      frame.data[num-5] = data;
      checksum ^= data;
      num++;
    }
  }
}

/**
 * @brief Initializes the gpio for the IR shutdown pin.
 * @retval None
 */
void IR_GPIO_Configuration() {
GPIO_InitTypeDef GPIO_InitStructure; 
  //Set up Rx as Floating
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = IR_SHUTDOWN_PIN;
  GPIO_Init(IR_SHUTDOWN_GPIO, &GPIO_InitStructure);
}

/**
 * @brief Registers a function to execute when a complete IR frame is received.
 * @param id the IR id
 * @param RX_Handler the user function to call on interrupt
 * @retval None.
 */
void Blox_IR_Register_RX_IRQ(uint8_t id, void (*RX_Handler)(IRFrame *frame)) {
  switch(id) {
    case 1:
      IR1_RX_Handler = RX_Handler;
      break;
    case 2:
      IR2_RX_Handler = RX_Handler;
      break;
    case 3:
      IR3_RX_Handler = RX_Handler;
      break;
    case 4:
      IR4_RX_Handler = RX_Handler;
      break;
  }
}

/**
 * @brief Enables the sw interrupt that occurs when a XBee reads a byte.
 * @param id the IR id
 * @retval None.
 */
void Blox_IR_Enable_RX_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      Blox_USART_Enable_RXNE_IRQ(IR_1_USART_ID);
      IR1_RX_Enable = TRUE;
      break;
    case 2:
      Blox_USART_Enable_RXNE_IRQ(IR_2_USART_ID);
      IR2_RX_Enable = TRUE;
      break;
    case 3:
      Blox_USART_Enable_RXNE_IRQ(IR_3_USART_ID);
      IR3_RX_Enable = TRUE;
      break;
    case 4:
      Blox_USART_Enable_RXNE_IRQ(IR_4_USART_ID);
      IR4_RX_Enable = TRUE;
      break;
  }
}

/**
 * @brief Disables the sw interrupt that occurs when a XBee reads a byte.
 * @param id the IR id
 * @retval None.
 */
void Blox_IR_Disable_RX_IRQ(uint8_t id) {
  switch(id) {
    case 1:
      Blox_USART_Disable_RXNE_IRQ(IR_1_USART_ID);
      IR1_RX_Enable = FALSE;
      break;
    case 2:
      Blox_USART_Disable_RXNE_IRQ(IR_2_USART_ID);
      IR2_RX_Enable = FALSE;
      break;
    case 3:
      Blox_USART_Disable_RXNE_IRQ(IR_3_USART_ID);
      IR3_RX_Enable = FALSE;
      break;
    case 4:
      Blox_USART_Disable_RXNE_IRQ(IR_4_USART_ID);
      IR4_RX_Enable = FALSE;
      break;
  }
}

/**
 * @brief Receive a byte on the given IR. A wrapper around USART
 * @param id the IR id to use.
 * @retval The received command or 0 on error.
 */
uint8_t IR_Receive(uint8_t id) {
  switch(id) {
  case 1:
    return Blox_USART_Receive(IR_1_USART_ID);
  case 2:
    return Blox_USART_Receive(IR_2_USART_ID);
  case 3:
    return Blox_USART_Receive(IR_3_USART_ID);
  case 4:
    return Blox_USART_Receive(IR_4_USART_ID);
  }
  return 0;
}

/**
 * @brief Receive a byte on the given IR. A wrapper around USART
 * @param id the IR id to use.
 * @retval The received command or 0 on error.
 */
uint8_t IR_TryReceive(uint8_t id) {
  switch(id) {
  case 1:
    return Blox_USART_TryReceive(IR_1_USART_ID);
  case 2:
    return Blox_USART_TryReceive(IR_2_USART_ID);
  case 3:
    return Blox_USART_TryReceive(IR_3_USART_ID);
  case 4:
    return Blox_USART_TryReceive(IR_4_USART_ID);
  }
  return 0;
}

/**
 * @brief Sends a byte out on the given IR. Wrapper around USART
 * @param id the IR id to use
 * @param data the byte to send
 * @retval None.
 */
void IR_Send(uint8_t id, uint8_t data) {
  switch(id) {
  case 1:
    Blox_USART_Send(IR_1_USART_ID, data);
    break;
  case 2:
    Blox_USART_Send(IR_2_USART_ID, data);
    break;
  case 3:
    Blox_USART_Send(IR_3_USART_ID, data);
    break;
  case 4:
    Blox_USART_Send(IR_4_USART_ID, data);
    break;
  }
}

/**
 * @brief Sends a IRFrame out on the given IR
 * @param id the IR id to use
 * @param type the type of the IRFrame
 * @param data a pointer to the data to be sent
 * @param len the length of the data to be sent
 * @retval None.
 */
void IR_SendFrame(uint8_t id, IRFrameType type, uint8_t *data, uint8_t len) {
  uint8_t i;
  uint8_t checksum = 0;
  switch(id) {
    case 1:
      Blox_USART_Send(IR_1_USART_ID, 0x7E);
      Blox_USART_Send(IR_1_USART_ID, (uint8_t)Blox_System_GetId());
      Blox_USART_Send(IR_1_USART_ID, 1);
      Blox_USART_Send(IR_1_USART_ID, type);
      Blox_USART_Send(IR_1_USART_ID, len);
      for(i = 0; i < len; i++) {
        Blox_USART_Send(IR_1_USART_ID, data[i]);
        checksum ^= data[i];
      }
      Blox_USART_Send(IR_1_USART_ID, checksum);
      break;
    case 2:
      Blox_USART_Send(IR_2_USART_ID, 0x7E);
      Blox_USART_Send(IR_2_USART_ID, (uint8_t)Blox_System_GetId());
      Blox_USART_Send(IR_2_USART_ID, 2);
      Blox_USART_Send(IR_2_USART_ID, type);
      Blox_USART_Send(IR_2_USART_ID, len);
      for(i = 0; i < len; i++) {
        Blox_USART_Send(IR_2_USART_ID, data[i]);
        checksum ^= data[i];
      }
      Blox_USART_Send(IR_2_USART_ID, checksum);
      break;
    case 3:
      Blox_USART_Send(IR_3_USART_ID, 0x7E);
      Blox_USART_Send(IR_3_USART_ID, (uint8_t)Blox_System_GetId());
      Blox_USART_Send(IR_3_USART_ID, 3);
      Blox_USART_Send(IR_3_USART_ID, type);
      Blox_USART_Send(IR_3_USART_ID, len);
      for(i = 0; i < len; i++) {
        Blox_USART_Send(IR_3_USART_ID, data[i]);
        checksum ^= data[i];
      }
      Blox_USART_Send(IR_3_USART_ID, checksum);
      break;
    case 4:
      Blox_USART_Send(IR_4_USART_ID, 0x7E);
      Blox_USART_Send(IR_4_USART_ID, (uint8_t)Blox_System_GetId());
      Blox_USART_Send(IR_4_USART_ID, 4);
      Blox_USART_Send(IR_4_USART_ID, type);
      Blox_USART_Send(IR_4_USART_ID, len);
      for(i = 0; i < len; i++) {
        Blox_USART_Send(IR_4_USART_ID, data[i]);
        checksum ^= data[i];
      }
      Blox_USART_Send(IR_4_USART_ID, checksum);
      break;
  }
}

/**
 * @brief Wakes all the IRs from sleep.
 * @retval None.
 */
void IR_Wake(void) {
  IR_SHUTDOWN_GPIO->ODR &= ~(1 << IR_SHUTDOWN_PIN_NUM);
}

/**
 * @brief Put all the IRs to sleep.
 * @retval None.
 */
void IR_Sleep(void) {
  IR_SHUTDOWN_GPIO->ODR |= (1 << IR_SHUTDOWN_PIN_NUM);
}

/** @} */

