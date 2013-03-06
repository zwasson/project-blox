/**
 * @file    power.c
 * @author  Zach Wasson
 * @version V0.1
 * @date    11/20/2010
 * @brief   Defines power management functions
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

#include "power.h"

/**
 * @ingroup feature_power
 * @{
 */
static void (*powerWake[MAX_POWER_WAKE_FN])(void);
static void (*powerSleep[MAX_POWER_SLEEP_FN])(void);
static uint32_t numPowerWake, numPowerSleep;

/**
 * @brief Registers a new function to be called when the system wakes or sleeps.
 *        Only adds if it isn't already there.
 * @param Power_Wake a fn pointer to a module's wake function
 * @param Power_Sleep a fn pointer to a module's sleep function
 * @retval None
 */
void Blox_Power_Register_Power(void (*Power_Wake)(void), void (*Power_Sleep)(void)) {
  uint8_t i;
  if (numPowerWake == MAX_POWER_WAKE_FN) {
    Blox_DebugPat("Blox_Power_Register_Power numPowerWake reached max of:%d\r\n",
        MAX_POWER_WAKE_FN);
    while(1) ;
  }
  
  for (i = 0; i < numPowerWake; i++) {
    if(powerWake[i] == Power_Wake)
      return;
  }
  
  powerWake[numPowerWake++] = Power_Wake;
  
  if (numPowerSleep == MAX_POWER_SLEEP_FN) {
    Blox_DebugPat("Blox_Power_Register_Power numPowerSleep reached max of:%d\r\n",
        MAX_POWER_SLEEP_FN);
    while(1) ;
  }
  
  for (i = 0; i < numPowerSleep; i++) {
    if(powerSleep[i] == Power_Sleep)
      return;
  }
  
  powerSleep[numPowerSleep++] = Power_Sleep;
}

/**
 * @brief Puts the system to sleep
 * @retval None
 */
void Blox_Power_Sleep(void) {
  uint8_t i;
  for (i = 0; i < numPowerSleep; i++) {
    if(powerSleep[i] != NULL)
      (*powerSleep[i])();
  }
}

/**
 * @brief Wakes the system up
 * @retval None
 */
void Blox_Power_Wake(void) {
  uint8_t i;
  for (i = 0; i < numPowerWake; i++) {
    if(powerWake[i] != NULL)
      (*powerWake[i])();
  }
}
/** @} */
