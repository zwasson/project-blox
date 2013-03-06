/**
 * @file    blox_accel.c
 * @author  Dan Cleary
 * @version V0.1
 * @date    10/20/2010
 * @brief   Drivers for Blox accelerometer
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

#include "blox_accel.h"

/**
 * @ingroup driver_accel
 * @{
 *
 * A Blox contains a MMA7631 3-axis accelerometer. This driver uses an ADC to
 * read its values and return meaningful interpretations to the caller.
 */
#define ADC1_DR_Address    ((u32)0x4001244C)

uint16_t Accel_Measurements[3];

/* Private function prototypes */
void Accel_RCC_Configuration(void);
void Accel_GPIO_Configuration(void);  
void Accel_DMA_Configuration(void);
void Accel_ADC_Configuration(void);

/**
 * @brief Initializes the Accelerometer.
 * Initializes the clocks, GPIO pins, DMA location, and ADC configuration.
 * Also sets the sleep pin to high (active low pulse).
 * @retval None
 */
void Blox_Accel_Init(void) { 
  Accel_RCC_Configuration();
  Accel_GPIO_Configuration();
  Accel_DMA_Configuration();
  Accel_ADC_Configuration();
  
  ACCEL_SLEEP_GPIO->ODR |= ACCEL_SLEEP_PIN;
}

/**
 * @brief Initializes the Accelerometer's clocks.
 * @retval None
 */
void Accel_RCC_Configuration() {
    RCC_APB2PeriphClockCmd(ACCEL_GPIO_CLK, ENABLE); 
    RCC_APB2PeriphClockCmd(ACCEL_SLEEP_GPIO_CLK, ENABLE);
    RCC_AHBPeriphClockCmd(ACCEL_DMA_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(ACCEL_ADC1_CLK, ENABLE);
}

/**
 * @brief Initializes the Accelerometer's GPIO for the ADC and sleep pins.
 * @retval None
 */
void Accel_GPIO_Configuration() {
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  //Accelerometer X-out, Y-out, and Z-out pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = ACCEL_XOUT_PIN | ACCEL_YOUT_PIN | ACCEL_ZOUT_PIN;
  GPIO_Init(ACCEL_GPIO, &GPIO_InitStructure);

  //Accelerometer Sleep Pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = ACCEL_SLEEP_PIN;
  GPIO_Init(ACCEL_SLEEP_GPIO, &GPIO_InitStructure);
}

/**
 * @brief Sets up the DMA for the Accelerometer.
 * The DMA configuration reads to the memory at Accel_Measurements.
 * @retval None
 */
void Accel_DMA_Configuration() {  
  DMA_InitTypeDef DMA_InitStructure;

  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Accel_Measurements;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

/**
 * @brief Initializes the ADC used by the Accelerometer.
 * @retval None
 */
void Accel_ADC_Configuration() {
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 3;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel 10, 11, 12 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * @brief Returns the X-value reading of the accelerometer.
 * @retval The X-value of the accelerometer's X axis.
 */
uint16_t Blox_Accel_GetX(void) {
  return Accel_Measurements[0];
}

/**
 * @brief Returns the Y-value reading of the accelerometer.
 * @retval The Y-value of the accelerometer's Y axis.
 */
uint16_t Blox_Accel_GetY(void) {
  return Accel_Measurements[1];
}

/**
 * @brief Returns the Z-value reading of the accelerometer.
 * @retval The Z-value of the accelerometer's Z axis.
 */
uint16_t Blox_Accel_GetZ(void) {
  return Accel_Measurements[2];
}

/**
 * @brief Returns the X tilt of the accelerometer.
 * @retval 0 The accelerometer isn't tilted in X.
 * @retval 1 X is tilted in the positive direction.
 * @retval 2 X is tilted in the negative direction.
 */
uint8_t Blox_Accel_GetXTilt(void) {
  float meas = (float)(Accel_Measurements[0] * 3.3/0xFFF);
  if (meas < 1.4)
    return 2;
  else if (meas < 2)
    return 1;
  else
    return 0;
  return 1;
}

/**
 * @brief Returns the Y tilt of the accelerometer.
 * @retval 0 The accelerometer isn't tilted in Y.
 * @retval 1 Y is tilted in the positive direction.
 * @retval 2 Y is tilted in the negative direction.
 */
uint8_t Blox_Accel_GetYTilt(void) {
  float meas = (float)(Accel_Measurements[1] * 3.3/0xFFF);
  if (meas < 1.3)
    return 0;
  else if (meas < 2.0)
    return 1;
  else
    return 2;
  return 1;
}

/**
 * @brief Returns the Z tilt of the accelerometer.
 * @retval 0 The accelerometer isn't tilted in Z.
 * @retval 1 Z is tilted in the positive direction.
 * @retval 2 Z is tilted in the negative direction.
 */
uint8_t Blox_Accel_GetZTilt(void) {
  float meas = (float)(Accel_Measurements[2] * 3.3/0xFFF);
  if (meas < 1.3)
    return 2;
  else if (meas < 2.0)
    return 1;
  else
    return 0;
  return 1;
}
/** @} */

