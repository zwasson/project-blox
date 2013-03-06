/**
 * @file    blox_usb.c
 * @author  Jesse Tannahill
 * @version V0.1
 * @date    10/19/2010
 * @brief   A wrapper class for the USB interface that uses USART
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

#include "blox_usb.h"
/**
 * @ingroup driver_usb
 * @{
 */

/**
 * @brief Guard to prevent double init
 */
static uint8_t usb_init = 0;

/**
 * @brief Initializes the USB module. Basically a wrapper on USART
 * @retval None
 */
void USB_Init(void) {
  if(!usb_init) {
    usb_init = 1;
    Blox_USART_Init(USB_USART_ID);
  }
}

/**
 * @brief Blocking receive of a byte over USB.
 *        A wrapper around USART.
 * @retval The received byte.
 */
uint8_t USB_Receive(void) {
    return Blox_USART_Receive(USB_USART_ID);
}

/**
 * @brief Non-blocking receive of a byte over USB.
 *        A wrapper around USART.
 * @retval The received byte or -1 on failure.
 */
int16_t USB_TryReceive(void) {
    return Blox_USART_TryReceive(USB_USART_ID);
}

/**
 * @brief Sends a byte over USB. A wrapper around USART
 * @param data the byte to send
 * @retval None.
 */
void USB_Send(uint8_t data) {
  Blox_USART_Send(USB_USART_ID, data);
}

/**
 * @brief Sends len bytes over USB. A wrapper around USART
 * @param data a pointer to the buffer of data to
 * @param len number of bytes to send from the buffer
 * @retval None.
 */
void USB_SendData(uint8_t *data, uint32_t len) {
  while(len--)
    USB_Send(*data++);
}

/**
 * @brief Sends a string based on pattern passed over USB.
 *  A wrapper around USART
 * @param format a format string
 * @param ... values to replace patterns in the string
 * @retval None.
 */
void USB_SendPat(char *format, ...) {
  va_list args;
  char buffer[100];
  va_start(args, format);
  
  vsprintf(buffer, format, args);
  USB_SendData((uint8_t *)buffer, strlen(buffer));
}

/** @} */

