/********************************************************************************
 * @file    role_test.c
 * @author  Jesse
 * @version V0.1
 * @date    11/3/2010
 * @brief   Tests the role management feature module. Blinks different LEDs
 *          based on roles.
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
 
#include "blox_system.h"
#include "blox_role.h"
#include "blox_led.h"

//Blinks the east LED
void role_1(void) {
  Blox_LED_Init();
  SysTick_Init();			     
  while(1) {
    SysTick_Wait(500);
    Blox_LED_Toggle(LED1);
  }
}

//Blinks the south LED
void role_2(void) {
  Blox_LED_Init();
  SysTick_Init(); 
  while(1) {
    Blox_LED_Toggle(LED2);
    SysTick_Wait(1000);
  }
}

void GPIO_Configuration (void);
void RCC_Configuration (void);

int main(void)
{
  Blox_Role_Init("role_test", 9);
  Blox_Role_Add(&role_1, 1, 2); //1 minimum, 2 max
  Blox_Role_Add(&role_2, 1, 5); 
  Blox_Role_Run();
  return 0; //Should never get here.
}
