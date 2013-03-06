/**
 * @file    blox_touch.c
 * @author  Ankita Kaul & Jesse Tannahill
 * @version V0.1
 * @date    11/01/2010
 * @brief   Driver that interacts with touchpanels over SPI.
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

#include "blox_touch.h"

/**
 * @ingroup driver_touch
 * @{
 */

/* Private function prototypes */
void Touch_RCC_Init(void);
void Touch_GPIO_Init(void);
void Touch_GPIO_DeInit(void);
void Touch_SPI_Init(void);
void Touch_SPI_DeInit(void);

/**
 * @brief Initializes the IR module. Basically a wrapper on USART
 * @retval None
 */
void Blox_Touch_Init(void) {
  Touch_RCC_Init();
  Touch_GPIO_Init();
  Touch_SPI_Init();
  
  Blox_System_Register_DeInit(&RCC_DeInit);
  Blox_System_Register_DeInit(&Touch_GPIO_DeInit);
  Blox_System_Register_DeInit(&Touch_SPI_DeInit);
}

/**
 * @brief Initializes clocks for SPI and the GPIO the SPI & BUSY Pins are on.
 * @retval None
 */
void Touch_RCC_Init() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(TOUCH_SPI_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(TOUCH_CS_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(TOUCH_SPI_CLK, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/**
 * @brief Initializes the gpio for the SPI pins, and BUSY.
 * @retval None
 */
void Touch_GPIO_Init() {
GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_SCK_PIN | TOUCH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(TOUCH_SPI_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_MISO_PIN;// | TOUCH_BUSY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TOUCH_SPI_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = TOUCH1_CS_PIN | TOUCH2_CS_PIN | TOUCH3_CS_PIN | TOUCH4_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(TOUCH_CS_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = TOUCH1_PENIRQ_PIN | TOUCH2_PENIRQ_PIN | TOUCH3_PENIRQ_PIN | TOUCH4_PENIRQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TOUCH_PENIRQ_GPIO, &GPIO_InitStructure);
  
  /*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief De-initializes the gpio for the SPI pins, and BUSY.
 * @retval None
 */
void Touch_GPIO_DeInit() {
  GPIO_DeInit(TOUCH_SPI_GPIO);
}

/**
 * @brief Initializes the SPI for the Touchpanel
 * @retval None
 */
void Touch_SPI_Init() {
	SPI_InitTypeDef SPIInitStruct;		
	SPIInitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPIInitStruct.SPI_Mode = SPI_Mode_Master;
	SPIInitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPIInitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPIInitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPIInitStruct.SPI_NSS = SPI_NSS_Soft;
	SPIInitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPIInitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPIInitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(TOUCH_SPI, &SPIInitStruct);	
	
	SPI_Cmd(TOUCH_SPI, ENABLE);
  //SPI_SSOutputCmd(TOUCH_SPI, ENABLE);
}

/**
 * @brief De-initializes the SPI for the Touchpanel
 * @retval None
 */
void Touch_SPI_DeInit() {
  SPI_I2S_DeInit(TOUCH_SPI);
}

/**
 * @brief Get the X-value of a press on the touchpanel.
 * @param numTouch ID of the touchpanel to retrieve from
 * @retval The 12-bit return value
 */
uint16_t Blox_Touch_GetX(int numTouch) {
  uint16_t ret = 0;
  //Chip-select is active low.
  TOUCH_CS_GPIO->ODR |= (TOUCH1_CS_PIN | TOUCH2_CS_PIN | TOUCH3_CS_PIN
                        | TOUCH4_CS_PIN);
  
  switch(numTouch){
  	case 1:	TOUCH_CS_GPIO->ODR &= ~TOUCH1_CS_PIN;
			break;
	case 2: TOUCH_CS_GPIO->ODR &= ~TOUCH2_CS_PIN;
			break;
	case 3: TOUCH_CS_GPIO->ODR &= ~TOUCH3_CS_PIN;
			break;
	case 4: TOUCH_CS_GPIO->ODR &= ~TOUCH4_CS_PIN;
			break;
  }
  
  if (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == SET)
    SPI_I2S_ReceiveData(TOUCH_SPI);
    
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, TOUCH_CTL_X);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Toss junk
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  ret = SPI_I2S_ReceiveData(TOUCH_SPI); //Data
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //tosss
  
  return ret;
}

/**
 * @brief Get the Y-value of a press on the touchpanel./
 * @param numTouch touchpanel id to retrieve from
 * @retval The 12-bit return value
 */
uint16_t Blox_Touch_GetY(int numTouch) {
  uint16_t ret = 0;
  //Chip-select is active low.
  TOUCH_CS_GPIO->ODR |= (TOUCH1_CS_PIN | TOUCH2_CS_PIN | TOUCH3_CS_PIN
                        | TOUCH4_CS_PIN);
  switch(numTouch){
  	case 1:	TOUCH_CS_GPIO->ODR &= ~TOUCH1_CS_PIN;
			break;
	case 2: TOUCH_CS_GPIO->ODR &= ~TOUCH2_CS_PIN;
			break;
	case 3: TOUCH_CS_GPIO->ODR &= ~TOUCH3_CS_PIN;
			break;
	case 4: TOUCH_CS_GPIO->ODR &= ~TOUCH4_CS_PIN;
			break;
	}
	
  if (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == SET)
    SPI_I2S_ReceiveData(TOUCH_SPI);
    
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, TOUCH_CTL_Y);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Toss junk
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  ret = SPI_I2S_ReceiveData(TOUCH_SPI); //Data
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Test toss
  
  return ret;
}

/**
 * @brief Get the Z1-value of a press on the touchpanel.
 * @param numTouch touchpanel id to retrieve from
 * @retval The 12-bit return value
 */
uint16_t Blox_Touch_GetZ1(int numTouch) {
  uint16_t ret = 0;
  
  //Chip-select is active low.
  TOUCH_CS_GPIO->ODR |= (TOUCH1_CS_PIN | TOUCH2_CS_PIN | TOUCH3_CS_PIN
                        | TOUCH4_CS_PIN);
   
  switch(numTouch){
  	case 1:	TOUCH_CS_GPIO->ODR &= ~TOUCH1_CS_PIN;
			break;
	case 2: TOUCH_CS_GPIO->ODR &= ~TOUCH2_CS_PIN;
			break;
	case 3: TOUCH_CS_GPIO->ODR &= ~TOUCH3_CS_PIN;
			break;
	case 4: TOUCH_CS_GPIO->ODR &= ~TOUCH4_CS_PIN;
			break;
	}
	
  if (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == SET)
    SPI_I2S_ReceiveData(TOUCH_SPI);
    
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, TOUCH_CTL_Z1);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Toss junk
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  ret = SPI_I2S_ReceiveData(TOUCH_SPI); //Data
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Test toss
  
  return ret;
}

/**
 * @brief Get the Z2-value of a press on the touchpanel.
 * @param numTouch touchpanel id to retrieve from
 * @retval The 12-bit return value
 */
uint16_t Blox_Touch_GetZ2(int numTouch) {
  uint16_t ret = 0;
  
  //Chip-select is active low.
  TOUCH_CS_GPIO->ODR |= (TOUCH1_CS_PIN | TOUCH2_CS_PIN | TOUCH3_CS_PIN
                        | TOUCH4_CS_PIN);
  
  switch(numTouch){
  	case 1:	TOUCH_CS_GPIO->ODR &= ~TOUCH1_CS_PIN;
			break;
	case 2: TOUCH_CS_GPIO->ODR &= ~TOUCH2_CS_PIN;
			break;
	case 3: TOUCH_CS_GPIO->ODR &= ~TOUCH3_CS_PIN;
			break;
	case 4: TOUCH_CS_GPIO->ODR &= ~TOUCH4_CS_PIN;
			break;
	}
	
  if (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == SET)
    SPI_I2S_ReceiveData(TOUCH_SPI);
    
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, TOUCH_CTL_Z2);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Toss junk
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  ret = SPI_I2S_ReceiveData(TOUCH_SPI); //Data
  
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, 0x0);
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  SPI_I2S_ReceiveData(TOUCH_SPI); //Test toss
  
  return ret;
}

/**
 * @brief Sends a byte out on SPI to the touchpanel.
 * @param data: the byte to send
 * @retval None.
 */
void Touch_SPI_Send(uint16_t data) {
  while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE)== RESET) ;
  SPI_I2S_SendData(TOUCH_SPI, data);
  //USB_SendPat("Sent data %d\r\n", data);
  return;
}

/**
 * @brief Receive a byte from the touchpanel
 * @retval the received byte.
 */
uint16_t Touch_SPI_Receive(void) {
  uint16_t debugger =0;
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET) ;
  debugger = SPI_I2S_ReceiveData(TOUCH_SPI);
    //USB_SendPat("Recieved data %d\r\n", debugger);
  return debugger;
}

/** @} */

