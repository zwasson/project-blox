 /**
 * @file    blox_xbee.h
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/27/2010
 * @brief   Contains function prototypes for the XBee interface.
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

#ifndef __BLOX_XBEE_H
#define __BLOX_XBEE_H

#include "blox_system.h"
#include "blox_vusart.h"
#include "blox_counter.h"

#include "stdio.h"
#include "string.h"

/**
 * @ingroup driver_xbee
 * @{
 */

#define XBEE_VUSART_ID 1

#define XBEE_RESET_GPIO 		  GPIOB
#define XBEE_RESET_GPIO_CLK 	RCC_APB2Periph_GPIOB
#define XBEE_RESET_PIN			  GPIO_Pin_14
#define XBEE_RESET_PIN_NUM		14

#define XBEE_SLEEP_GPIO 		  GPIOB
#define XBEE_SLEEP_GPIO_CLK 	RCC_APB2Periph_GPIOB
#define XBEE_SLEEP_PIN			  GPIO_Pin_15
#define XBEE_SLEEP_PIN_NUM		15

#define MESSAGE_SIZE 100

// standard ASCII symbols 
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20       
#define DEL  0x7F

//XBee API IDs
#define API_TX_STATUS 0x89
#define API_RX_FRAME  0x81

/**
 * @brief Enum for possible XBee statuses
 */
typedef enum {
  XBEE_TX_STATUS_FAIL = -3,
  XBEE_INIT_FAIL,
  XBEE_TX_FAIL,
  XBEE_OK
} XBEE_STATUS;

/**
 * @brief Enum for possible XBee frame types
 */
typedef enum {
  FRAME_TYPE_BASE,
  FRAME_TYPE_ROLE,
  FRAME_TYPE_USER
} BloxFrameType;

/**
 * @brief Enum for possible XBee TX statuses
 */
typedef enum {
  XBEE_TXSTATUS_ERROR = 0,
  XBEE_TXSTATUS_NORMAL,
  XBEE_TXSTATUS_SUCCESS
} XBEE_TXSTATUS;

#define BLOX_FRAME_DATA_LEN 75
#define XBEE_BLOX_BROADCAST_ID 0xFFFFFFFF
#define XBEE_HOLD_PERIOD 1000

/**
 * @brief App-level frame that is parsed from a XBeeFrame
 */
typedef struct {
  uint32_t src_id;                    /**< the id of the sending Blox */
  uint32_t dst_id;                    /**< the id of the receiving Blox */
  uint8_t len;                        /**< the number of bytes of good data */
  BloxFrameType type;                 /**< the type of BloxFrame */
  uint8_t data[BLOX_FRAME_DATA_LEN];  /**< the array of raw data */
} BloxFrame;

/**
 * @brief the XBee Transmission Frame struct
 */
typedef struct {
  uint8_t start;          /**< the start delimiter */
  uint16_t length;        /**< the number of bytes in cmdData */
  uint8_t api;            /**< the api code for this command */
  uint8_t id;             /**< the id of the packet */
  uint16_t dest_addr;     /**< the xbee dest_addr to send to */
  uint8_t options;        /**< byte to set ACK/etc */
  BloxFrame blox_frame;   /**< the blox_frame to send inside the xbee frame */
  uint8_t checksum;       /**< the checksum to verify the sent data */
} XBeeTxFrame;

/**
 * @brief A general xbee frame to parse in any command
 */
typedef struct {
  uint16_t length;      /**< the length of the xbee frame */
  uint8_t data[125];    /**< the data of the xbee frame */
} XBeeFrame;

/**
 * @brief Struct for reading in the TxStatus frame format.
 */
typedef struct {
  uint16_t length;    /**<  */
  uint8_t api;        /**< the number of bytes of cmdData */
  uint8_t frame_id;   /**< the api of the command */
  uint8_t status;     /**< the id of the packet */
} XBeeTxStatusFrame;  /**< the status from the XBeeSend */

/**
 * @brief Struct for reading in the Rx frame format.
 */
typedef struct {
  uint16_t length;        /**< the number of bytes of cmdData */
  uint8_t api;            /**< the api of the command */
  uint16_t source;        /**< the source xbee id of the sender */
  uint8_t rssi;           /**< the signal strength in dB */
  uint8_t options;        /**< Bytes the sender can set to block ACKs */
  BloxFrame blox_frame;   /**< the frame the sender originally sent */
  uint8_t checksum;       /**< checksum a checksum to detect errors */
} XBeeRxFrame;

XBEE_STATUS Blox_XBee_Config(void);
XBEE_STATUS Blox_XBee_Print(void);
XBEE_STATUS Blox_XBee_Init (void);
XBEE_STATUS Blox_XBee_Send (uint8_t *data, uint32_t len, BloxFrameType type, uint32_t dst_id);
void Blox_XBee_Send_Period(uint8_t *data, uint32_t len, BloxFrameType type, uint32_t dst_id, uint32_t millis);
void Blox_XBee_Register_Read(void (*Read_Handler)(void));
BloxFrame *Blox_XBee_Receive(void);
void Blox_XBee_Register_RX_IRQ(void (*RX_Handler)(BloxFrame *));
void Blox_XBee_Enable_RX_IRQ(void);
void Blox_XBee_Disable_RX_IRQ(void);
/** @} */
#endif
