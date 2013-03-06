/**
 * @file    blox_gesture.c 
 * @author  Ankita Kaul
 * @version V0.1
 * @date    11/08/2010
 * @brief   Driver that performs touchpanel gesture detection 
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

#include "blox_gesture.h" 
#include "blox_oled.h" 
#include "blox_led.h" 
 
/**
 * @ingroup feature_gesture
 * @{
 */
TIMER_ID touch1ID, touch2ID, touch3ID, touch4ID; 
uint16_t val[4]={0,0,0,0}; //[Touch1 #datapoints, Touch2 #datapoints, Touch3 #datapoints, Touch4 #datapoints] 
uint16_t XVals[4][50]; 
uint16_t YVals[4][50];   
GestureRecord LastGesture[4]; //Timestamp and last gesture recorded for each touchpanel 
 
//private functions/*
void Blox_touch1_isTouched(void);
void Blox_touch2_isTouched(void);
void Blox_touch3_isTouched(void);
void Blox_touch4_isTouched(void);
void Blox_gestureHandler(int touchNumber);
 
/**
 * @brief Initializes timer interrupt and array that hold touch values
 * @retval None
 */
void Blox_Gesture_Init(void){
 
  Blox_Touch_Init(); //Start out by initializing the touchpanel, before gesture handling
 
  Blox_Timer_Init(TOUCH_TIMx, TOUCH_CLK);
  touch1ID = Blox_Timer_Register_IRQ(TOUCH_TIMx, TOUCH_DETECT_FREQ, &Blox_touch1_tracker, DISABLE);
  touch2ID = Blox_Timer_Register_IRQ(TOUCH_TIMx, TOUCH_DETECT_FREQ, &Blox_touch2_tracker, DISABLE);
  touch3ID = Blox_Timer_Register_IRQ(TOUCH_TIMx, TOUCH_DETECT_FREQ, &Blox_touch3_tracker, DISABLE);
  touch4ID = Blox_Timer_Register_IRQ(TOUCH_TIMx, TOUCH_DETECT_FREQ, &Blox_touch4_tracker, DISABLE);
  
  Blox_EXTI_Init();
  Blox_EXTI_Register_HW_IRQ(GPIO_PortSourceGPIOE, GPIO_PinSource10, &Blox_touch1_isTouched);
  Blox_EXTI_Register_HW_IRQ(GPIO_PortSourceGPIOE, GPIO_PinSource11, &Blox_touch2_isTouched);
  Blox_EXTI_Register_HW_IRQ(GPIO_PortSourceGPIOE, GPIO_PinSource14, &Blox_touch3_isTouched);
  Blox_EXTI_Register_HW_IRQ(GPIO_PortSourceGPIOE, GPIO_PinSource7, &Blox_touch4_isTouched);
  
  Blox_Touch_GetY(1);//trash, to enable PENIRQ
  
  //DEBUG - REMOVE LATER
  //Blox_LED_Init();
} 
 
/**
 * @brief Clears timer interrupt and array that holds touch value
 * @retval None
 */
void Blox_Gesture_DeInit(void){
 
	Blox_Timer_Disable_IRQ(touch1ID);
	Blox_Timer_Disable_IRQ(touch2ID);
	Blox_Timer_Disable_IRQ(touch3ID);
	Blox_Timer_Disable_IRQ(touch4ID);
	
	Blox_EXTI_Disable_IRQ(GPIO_PinSource10);
	Blox_EXTI_Disable_IRQ(GPIO_PinSource11);
	Blox_EXTI_Disable_IRQ(GPIO_PinSource14);
	Blox_EXTI_Disable_IRQ(GPIO_PinSource7);
	
} 

/**
 * @brief Determines if user is touching Blox, interrupts every 0.02seconds
 * @retval none
 */
void Blox_touch1_isTouched(void){
	  
	if(Blox_Touch_GetZ1(1)> PRESSURE_THRESHOLD) //check if pressure detected and over specified threshold
	{
	  //disable gesture detection on touchpanel
	  Blox_EXTI_Disable_IRQ(GPIO_PinSource10);
	  
	  //enable timer interrupt to collect gesture data
	  Blox_Timer_Enable_IRQ(touch1ID);  
	}	
}
 
/**
 * @brief Determines if user is touching Blox, interrupts every 0.02seconds
 * @retval none
 */
void Blox_touch2_isTouched(void){
	  
	if(Blox_Touch_GetZ1(2)> PRESSURE_THRESHOLD) //is pressure detected and over specified threshold? 
	{
	  //disable gesture detection on touchpanel
	  Blox_EXTI_Disable_IRQ(GPIO_PinSource11);
	  
	  //enable timer interrupt to collect gesture data
	  Blox_Timer_Enable_IRQ(touch2ID);  
	  
	  //DEBUG
	  //Blox_LED_Toggle(1); 

	}	
}
 
/**
 * @brief Determines if user is touching Blox, interrupts every 0.02seconds
 * @retval none
 */
void Blox_touch3_isTouched(void){
	  
	if(Blox_Touch_GetZ1(3)> PRESSURE_THRESHOLD) //is pressure detected and over specified threshold? 
	{
	  //disable gesture detection on touchpanel
	  Blox_EXTI_Disable_IRQ(GPIO_PinSource14);
	  
	  //enable timer interrupt to collect gesture data
	  Blox_Timer_Enable_IRQ(touch3ID);  
	}	
}
 
/**
 * @brief Determines if user is touching Blox, interrupts every 0.02seconds
 * @retval none
 */
void Blox_touch4_isTouched(void){
	  
	if(Blox_Touch_GetZ1(4)> PRESSURE_THRESHOLD) //is pressure detected and over specified threshold? 
	{
	  //disable gesture detection on touchpanel
	  Blox_EXTI_Disable_IRQ(GPIO_PinSource7);
	  
	  //enable timer interrupt to collect gesture data
	  Blox_Timer_Enable_IRQ(touch4ID);  
	}	
}
 
/**
 * @brief Populated array tracking gesture movement
 * @retval None
 */
void Blox_touch1_tracker(void){
 
	  if(val[0]<50 && (Blox_Touch_GetZ1(1)>PRESSURE_THRESHOLD)){ //include timeout after 50 samples collected or gesture done 
		XVals[0][val[0]]=Blox_Touch_GetX(1);
		YVals[0][val[0]]=Blox_Touch_GetY(1);
		val[0]++; 
	    }
		
	  else{ //gesture done or timeout 
	  	Blox_Timer_Disable_IRQ(touch1ID);
		  Blox_gestureHandler(1);
		  Blox_EXTI_Enable_IRQ(GPIO_PinSource10);
	  }
}
 
/**
 * @brief Populated array tracking gesture movement
 * @retval None
 */
void Blox_touch2_tracker(void){
 
//Blox_LED_Toggle(2); 
 
  if(val[1]<50 && (Blox_Touch_GetZ1(2)>PRESSURE_THRESHOLD)){ //include timeout after 50 samples collected or gesture done 
	XVals[1][val[1]]=Blox_Touch_GetX(2);
	YVals[1][val[1]]=Blox_Touch_GetY(2);
	val[1]++; 
  }
		
	 else{ //gesture done or timeout  
	 	Blox_Timer_Disable_IRQ(touch2ID);
		Blox_gestureHandler(2);
		Blox_EXTI_Enable_IRQ(GPIO_PinSource11);
	}
}
 
/**
 * @brief Populated array tracking gesture movement
 * @retval None
 */
void Blox_touch3_tracker(void){
 
	  if(val[2]<50 && (Blox_Touch_GetZ1(3)> PRESSURE_THRESHOLD)){ //include timeout after 50 samples collected or gesture done 
		XVals[2][val[2]]=Blox_Touch_GetX(3);
		YVals[2][val[2]]=Blox_Touch_GetY(3);
		val[2]++; 
	    }
		
	  else{ //gesture done or timeout 
	  	Blox_Timer_Disable_IRQ(touch3ID);
		Blox_gestureHandler(3); 
		Blox_EXTI_Enable_IRQ(GPIO_PinSource14);
	  }
}
 
/**
 * @brief Populated array tracking gesture movement
 * @retval None
 */
void Blox_touch4_tracker(void){
 
  if(val[3]<50 && (Blox_Touch_GetZ1(4)>PRESSURE_THRESHOLD)){ //include timeout after 50 samples collected or gesture done 
	  XVals[3][val[3]]=Blox_Touch_GetX(4);
	  YVals[3][val[3]]=Blox_Touch_GetY(4);
	  val[3]++; 
  }
		
  else{ //gesture done or timeout 
  	Blox_Timer_Disable_IRQ(touch4ID);
  	Blox_gestureHandler(4); 
		Blox_EXTI_Enable_IRQ(GPIO_PinSource7);
	}
}
 
/**
 * @brief Determines gesture movement from pre-populated tracking gesture array
 * @retval None
 */
void Blox_gestureHandler(int touchNumber){
 
//   uint16_t  xavg_1, xavg_2, yavg_1, yavg_2 = 0; 
  int gestureNow=0; 
  int counter =0;
  int XMov =0;
  int YMov = 0; 
  int XStart = XVals[touchNumber-1][0];
  int YStart = YVals[touchNumber-1][0];
  int XEnd = XVals[touchNumber-1][val[touchNumber-1]-1]; 
  int YEnd = YVals[touchNumber-1][val[touchNumber-1]-1]; 
  
  if((XStart-XEnd)>XTHRESH) 
 		XMov = TOUCH_GESTURE_RL;
  else if((XEnd-XStart)>XTHRESH) 
 		XMov = TOUCH_GESTURE_LR;
  else if((XStart-XEnd)<XNOMOV) 
 		XMov = TOUCH_X_STABLE; 
		
   if((YStart-YEnd)>YTHRESH)
     YMov = TOUCH_GESTURE_DU;
   else if((YEnd-YStart)>YTHRESH)
     YMov = TOUCH_GESTURE_UD; 
   else if((YStart-YEnd)<YNOMOV)
     YMov = TOUCH_Y_STABLE;

   gestureNow = XMov*YMov; 
  	
   switch(gestureNow){
   	case 3:	// LR and DU = Diagonal bottom left to top right
      gestureNow = TOUCH_DIAG_DLUR; 
      break;
    case 4: // LR and UD = Diagonal top left to bottom right
      gestureNow = TOUCH_DIAG_ULDR; 
      break;
    case -1: // LR (Y Stable)
      gestureNow = TOUCH_GESTURE_LR;
      break;
    case 6: // RL and DU = Diagonal bottom right to top left
      gestureNow = TOUCH_DIAG_DRUL; 
      break;
    case 8: // RL and UD = Diagonal top right to bottom left
      gestureNow = TOUCH_DIAG_URDL;
      break;
      case -2: // RL (Y stable)
      gestureNow = TOUCH_GESTURE_RL;
      break;
    case -4: //UD (X stable)
      gestureNow = TOUCH_GESTURE_UD;
      break;
    case -3 : //DU (X stable)
      gestureNow = TOUCH_GESTURE_DU; 
      break;
    case 1: // Tap (X stable and Y stable)
      gestureNow = TOUCH_GESTURE_TAP;	
      break; 
  }
	
	//Blox_LED_Toggle(3); //DEBUG
		
	LastGesture[touchNumber-1].timestamp=SysTick_Get_Milliseconds(); 
	LastGesture[touchNumber-1].gesture=gestureNow; 
	val[0]=val[1]=val[2]=val[3]=0; //clear 
	
  for(counter=0; counter<50; counter++) //clear memory
  {
    XVals[touchNumber-1][counter]=0;
	  YVals[touchNumber-1][counter]=0; 
  }

}

/**
 * @brief Returns most recent gesture id for a specified touch panel
 * @retval The gesture id
 */
int Blox_Gesture_GetGesture(int touchNumber){
  return LastGesture[touchNumber-1].gesture; 
}

/**
 * @brief Returns most recent gesture timestamp for a specified touch panel
 * @retval The gesture timestamp
 */
int Blox_Gesture_GetGestureTime(int touchNumber){
  return LastGesture[touchNumber-1].timestamp; 
}
/** @} */

