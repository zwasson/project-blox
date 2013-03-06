/**
 * @file    blox_xbee.c
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/27/2010
 * @brief   Driver for Blox XBee wireless module
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
 
#include "blox_xbee.h"

/**
 * @ingroup driver_xbee
 * @{
 */
/* Globals */
/**
 * @brief The function pointer called when an interrupt occurs.
 */
void (*XBee_RX_Handler)(BloxFrame *);
/**
 * @brief Flag if the RX interrupt will call the user registered function.
 */
uint8_t XBee_RX_Enable = FALSE;
/**
 * @brief Flag used to communicate information between interrupt and TxStatus
 */
uint8_t XBee_TxStatus_Flag = XBEE_TXSTATUS_NORMAL;

/* Private function prototypes */
void XBee_RCC_Configuration(void);
void XBee_GPIO_Configuration(void);
uint8_t XBee_CheckOkResponse(void);
XBEE_STATUS XBee_SendTxFrame (XBeeTxFrame *frame);
XBEE_STATUS XBee_TxStatus (void);
void Blox_XBee_VUSART_RXNE_IRQ(void);

/**
 * @brief Configures the XBee and writes the configuration to non-volatile mem.
 * @retval XBEE_OK if successfull, XBEE_INIT_FAIL on failure.
 */
XBEE_STATUS Blox_XBee_Config(void) {
  char buffer[10];
  uint8_t garbage;
  SysVar sys;
  XBee_RCC_Configuration();
  XBee_GPIO_Configuration();
  
  Blox_VUSART_Init(XBEE_VUSART_ID);
  SysTick_Init();
  Blox_System_Init();
  Blox_System_GetVars(&sys);
  
  XBEE_SLEEP_GPIO->ODR &= ~(XBEE_SLEEP_PIN);
  XBEE_RESET_GPIO->ODR |= XBEE_RESET_PIN;
  SysTick_Wait(1);
  XBEE_RESET_GPIO->ODR &= ~(XBEE_RESET_PIN);
  SysTick_Wait(1);
  XBEE_RESET_GPIO->ODR |= XBEE_RESET_PIN;
  SysTick_Wait(1100);

  Blox_VUSART_TryReceive(XBEE_VUSART_ID, &garbage);   //clear VUSART buffer before send/receive
  Blox_VUSART_Send(XBEE_VUSART_ID, 'X');           // Junk character to before init
  SysTick_Wait(1100);
  Blox_VUSART_SendData(XBEE_VUSART_ID, "+++", 3) ; //Enter Command Mode
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(1100);
  
  sprintf(buffer, "ATMY%d\r", sys.id);
  Blox_VUSART_SendData(XBEE_VUSART_ID, (uint8_t *)buffer, strlen(buffer)) ; //Enter Command Mode
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);  
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATDLFFFF\r", 9) ; //Set broadcast dest ID
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);    
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATDH0\r", 6) ; //Set high of dest ID
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATAP1\r", 6) ; //API Mode 1 (no escapes)
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATRN1\r", 6) ; //API Mode 1 (no escapes)
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATWR\r", 9) ; //Write to non-volatile memory
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATCN\r", 5) ; //Exit command mode
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  return XBEE_OK;
}

/**
 * @brief Prints out the configuration options of the XBee.
 * @retval XBEE_OK if successfull, XBEE_INIT_FAIL on failure.
 */
XBEE_STATUS Blox_XBee_Print(void) {
  uint8_t garbage;
  
  Blox_VUSART_TryReceive(XBEE_VUSART_ID, &garbage);   //clear VUSART buffer before send/receive
  Blox_VUSART_Send(XBEE_VUSART_ID, 'X');           // Junk character to before init
  SysTick_Wait(1100);
  Blox_VUSART_SendData(XBEE_VUSART_ID, "+++", 3) ; //Enter Command Mode
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(1100);
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATMY\r", 5) ; //Read source address
  Blox_DebugStr("ATMY:");
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c\r\n", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage); //\r
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATDL\r", 5) ; //Read source address
  Blox_DebugStr("ATDL:");
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c\r\n", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage); //\r
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATDH\r", 5) ; //Read source address
  Blox_DebugStr("ATDH:");
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c\r\n", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage); //\r
 
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATAP\r", 5) ; //Read source address
  Blox_DebugStr("ATAP:");
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage);
  Blox_DebugPat("%c\r\n", garbage);
  Blox_VUSART_Receive(XBEE_VUSART_ID, &garbage); //\r
  
  Blox_VUSART_SendData(XBEE_VUSART_ID, "ATCN\r", 5) ; //Exit command mode
  if (XBee_CheckOkResponse() == FALSE)
    return XBEE_INIT_FAIL;
  SysTick_Wait(20);
  
  return XBEE_OK;
}

/**
 * @brief Initializes the XBees sleep and reset pins, and then resets the XBee.
 * @retval None
 */
XBEE_STATUS Blox_XBee_Init(void) {
  uint8_t garbage;
  XBee_RCC_Configuration();
  XBee_GPIO_Configuration();

  Blox_System_Init();
  SysTick_Init();

  XBEE_SLEEP_GPIO->ODR &= ~(XBEE_SLEEP_PIN);
  XBEE_RESET_GPIO->ODR |= XBEE_RESET_PIN;
  SysTick_Wait(1);
  XBEE_RESET_GPIO->ODR &= ~(XBEE_RESET_PIN);
  SysTick_Wait(1);
  XBEE_RESET_GPIO->ODR |= XBEE_RESET_PIN;
  SysTick_Wait(300);

  Blox_VUSART_Init(XBEE_VUSART_ID);
  Blox_VUSART_TryReceive(XBEE_VUSART_ID, &garbage);   //clear VUSART buffer before send/receive
  Blox_VUSART_Disable_RXNE_IRQ(XBEE_VUSART_ID);
  Blox_VUSART_Register_RXNE_IRQ(XBEE_VUSART_ID, &Blox_XBee_VUSART_RXNE_IRQ);
  Blox_VUSART_Enable_RXNE_IRQ(XBEE_VUSART_ID);

  return XBEE_OK;
}

/**
 * @brief Initializes clocks for XBee sleep and reset pins.
 * @retval None
 */
void XBee_RCC_Configuration(void) {
    RCC_APB2PeriphClockCmd(XBEE_RESET_GPIO_CLK, ENABLE);
}

/**
 * @brief Initializes the gpio for the XBee reset and sleep pins.
 * @retval None
 */
void XBee_GPIO_Configuration() {
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  //XBee Reset is push-pull, 50Mz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = XBEE_RESET_PIN;
  GPIO_Init(XBEE_RESET_GPIO, &GPIO_InitStructure);
  
  //XBee Sleep is push-pull, 50Mz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = XBEE_SLEEP_PIN;
  GPIO_Init(XBEE_SLEEP_GPIO, &GPIO_InitStructure);
}

/**
 * @brief Checks if "OK<CR>" is waiting in the buffer
 * @retval TRUE if "OK<CR>" is received, FALSE otherwise.
 */
uint8_t XBee_CheckOkResponse(void) {
  uint8_t ret;
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret);
  if (ret != 'O') {
    Blox_DebugPat("XBee_CheckOKResponse received %x instead of O\r\n", (char)ret);
    return FALSE;
  }
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret);
  if (ret != 'K') {
    Blox_DebugPat("XBee_CheckOKResponse received %x instead of K\r\n", (char)ret);
    return FALSE;
  }
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret);
  if (ret != CR) {
    Blox_DebugPat("XBee_CheckOKResponse received %x instead of <CR>\r\n", (char)ret);
    return FALSE;
  }
  return TRUE;
}


/**
 * @brief The function that XBee registers with VUSART to execute on byte received.
 * @retval None.
 */
void Blox_XBee_VUSART_RXNE_IRQ(void) {
  static uint8_t num = 0;
  static uint8_t checksum = 0;
  static XBeeFrame frame;
  uint8_t data;
  
  if (Blox_VUSART_TryReceive(XBEE_VUSART_ID, &data) == RX_EMPTY)
    return;
  
  if (num == 0) {
    if (data == 0x7E) {
      num = 1; //Start of a frame
      checksum = 0;
    }
  } else if (num == 1) {
    frame.length = data << 8;
    num++;
  } else if (num == 2) {
    frame.length |= data;
    if (frame.length > 100)
      num = 0;
    else
      num++;
  } else if (num > 2) {
    if (num == frame.length+3) {
      if(0xFF-checksum != data)
        num = 0;
      else {
        XBeeTxStatusFrame *status;
        switch(frame.data[0]) {
        case API_TX_STATUS:
          status = (XBeeTxStatusFrame *)&frame;
          if (status->status == 0 && data == 0xFF-checksum)
            XBee_TxStatus_Flag = XBEE_TXSTATUS_SUCCESS;
          else
            XBee_TxStatus_Flag = XBEE_TXSTATUS_ERROR;
          break;
        case API_RX_FRAME:
          if (XBee_RX_Handler != NULL && XBee_RX_Enable == TRUE) {
            XBeeRxFrame rx_frame;
            BloxFrame *retFrame;
            rx_frame.length = frame.length;
            rx_frame.api = frame.data[0];
            rx_frame.source = frame.data[1] << 8;
            rx_frame.source = frame.data[2];
            rx_frame.rssi = frame.data[3];
            rx_frame.options = frame.data[4];
            memcpy(&(rx_frame.blox_frame), &(frame.data[5]), rx_frame.length-5);
            rx_frame.checksum = data;
            retFrame = (BloxFrame *)malloc(sizeof(BloxFrame));
            memcpy(retFrame, &(rx_frame.blox_frame), sizeof(BloxFrame));
            XBee_RX_Handler(retFrame);
            free(retFrame);
          }
          break;
        }
      }
      num = 0;
    } else {
      frame.data[num-3] = data;
      checksum += data;
      num++;
    }
  }
}

/**
 * @brief Registers a function to execute when a complete XBee frame is received.
 * @retval None.
 */
void Blox_XBee_Register_RX_IRQ(void (*RX_Handler)(BloxFrame *frame)) {
  XBee_RX_Handler = RX_Handler;
}

/**
 * @brief Enables the sw interrupt that occurs when a XBee reads a byte.
 * @retval None.
 */
void Blox_XBee_Enable_RX_IRQ(void) {
  XBee_RX_Enable = TRUE;
}

/**
 * @brief Disables the sw interrupt that occurs when a XBee reads a byte.
 * @retval None.
 */
void Blox_XBee_Disable_RX_IRQ(void) {
  XBee_RX_Enable = FALSE;
}

/**
 * @brief Receives a BloxFrame from the XBee.
 * @retval The received frame or NULL on error.
 */
BloxFrame * Blox_XBee_Receive(void) {
  XBeeRxFrame frame;
  BloxFrame *retFrame;
  uint8_t ret = 0;
  uint8_t checksum = 0;
  uint32_t i;

  if (Blox_VUSART_TryReceive(XBEE_VUSART_ID, &ret) != VUSART_SUCCESS)
    return NULL;

  if (ret != 0x7E)
    return NULL;
    
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret);
  frame.length = ret << 8;
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret);
  frame.length |= ret;
  if(frame.length > 100) {
    return NULL;
  }
  Blox_VUSART_Receive(XBEE_VUSART_ID, &(frame.api));
  checksum += frame.api;
  
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret); //Source Address MSB
  frame.source = ret << 8;
  checksum += ret;
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret); //Source Address LSB
  frame.source |= ret;
  checksum += ret;
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret); //RSSI
  frame.rssi = ret;
  checksum += ret;
  Blox_VUSART_Receive(XBEE_VUSART_ID, &ret); //Options
  frame.options = ret;
  checksum += ret;

  for(i = 0; i < frame.length-5; i++) {
    Blox_VUSART_Receive(XBEE_VUSART_ID, ((uint8_t *)&(frame.blox_frame))+i);
    checksum += ((char *)&(frame.blox_frame))[i];
  }
  Blox_VUSART_Receive(XBEE_VUSART_ID, &(frame.checksum));
  
  if (frame.checksum != 0xFF-checksum)
    return NULL;
  
  retFrame = (BloxFrame *)malloc(sizeof(BloxFrame));
  memcpy(retFrame, &(frame.blox_frame), sizeof(BloxFrame));
  
  return retFrame;
}

/**
 * @brief Sends data out of a specific type on the XBee.
 * @param data: the data to be sent.
 * @param len: the amount of data being sent.
 * @param type: the type of the BloxFrame.
 * @param dst_id: the dest xbee id to send to.
 * @retval XBEE_OK if successful, XBEE_INIT_FAIL on failure.
 */
XBEE_STATUS Blox_XBee_Send (uint8_t *data, uint32_t len, BloxFrameType type, uint32_t dst_id) {
  XBeeTxFrame frame;
  uint16_t i;
  if(len > BLOX_FRAME_DATA_LEN)
    return XBEE_TX_FAIL;
  
  frame.start = 0x7E;
  frame.api = 0x01;
  frame.length = sizeof(BloxFrame) + 5; // length includes checksum
  frame.dest_addr = 0xFFFF;
  frame.options = 0x04;
  frame.id = 1;
  frame.blox_frame.src_id = Blox_System_GetId();
  frame.blox_frame.dst_id = dst_id;
  frame.blox_frame.type = type;
  frame.blox_frame.len = len;
  for (i = 0; i < len; i++)
    frame.blox_frame.data[i] = data[i];
  
  //checksum split up for hand debugging
  frame.checksum = 0xFF;
  frame.checksum -= frame.api;
  frame.checksum -= frame.id;
  frame.checksum -= (frame.dest_addr >> 8) & 0xFF;
  frame.checksum -= frame.dest_addr & 0xFF;
  frame.checksum -= frame.options;
  for (i = 0; i < sizeof(BloxFrame); i++)
    frame.checksum -= ((uint8_t *)&(frame.blox_frame))[i];
  
  return XBee_SendTxFrame(&frame);
}

/**
 * @brief Sends data out of a specific type on the XBee for a period of time.
 * @param data: the data to be sent.
 * @param len: the amount of data being sent.
 * @param type: the type of the BloxFrame.
 * @param dst_id: the dest xbee id to send to.
 * @param millis: the amount to continue transmission.
 * @retval None.
 */
void Blox_XBee_Send_Period (uint8_t *data, uint32_t len, BloxFrameType type, uint32_t dst_id, uint32_t millis) {
  uint32_t cur_time = SysTick_Get_Milliseconds();
  while (SysTick_Get_Milliseconds() < cur_time+millis) {
    Blox_XBee_Send(data, len, type, dst_id);
  }
}

/**
 * @brief Sends a XBeeTxFrame.
 * @param frame: the frame to be sent.
 * @retval XBEE_OK if successful, XBEE_INIT_FAIL on failure.
 */
XBEE_STATUS XBee_SendTxFrame (XBeeTxFrame *frame) {
  uint8_t i;
  uint8_t len = frame->length-5;
  
  XBee_TxStatus_Flag = XBEE_TXSTATUS_NORMAL;

  Blox_VUSART_Send(XBEE_VUSART_ID, frame->start);
	Blox_VUSART_Send(XBEE_VUSART_ID, (uint8_t)(frame->length >> 8));
	Blox_VUSART_Send(XBEE_VUSART_ID, (uint8_t)frame->length);
	Blox_VUSART_Send(XBEE_VUSART_ID, frame->api);
	Blox_VUSART_Send(XBEE_VUSART_ID, frame->id);
	Blox_VUSART_Send(XBEE_VUSART_ID, (uint8_t)(frame->dest_addr >> 8));
	Blox_VUSART_Send(XBEE_VUSART_ID, (uint8_t)frame->dest_addr);
	Blox_VUSART_Send(XBEE_VUSART_ID, frame->options);
	for (i = 0; i < len; i++)
	  Blox_VUSART_Send(XBEE_VUSART_ID, ((uint8_t *)&(frame->blox_frame))[i]);
	Blox_VUSART_Send(XBEE_VUSART_ID, frame->checksum);
  
  SysTick_Wait(1); //Give it a chance to send.
  if(XBee_TxStatus_Flag == XBEE_TXSTATUS_SUCCESS)
    return XBEE_OK;

  return XBEE_TX_FAIL;
}
/** @} */

