/**
 * @file    blox_speaker.c
 * @author  Ankita Kaul
 * @version V0.1
 * @date    10/19/2010
 * @brief   Device driver for the speaker
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

#include <stdio.h>
#include <blox_system.h>
#include <stm32f10x_dac.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include <misc.h>	 
#include "blox_tim.h"
#include "blox_speaker.h"
#include "blox_music.h"

/**
 * @ingroup driver_speaker
 * @{
 */
//private functions// 
void Sin_gen(void); 
void Sin_gen2(void); 
void NoteHandler(void); 
void EnvelopeGen(void); 

DAC_InitTypeDef DAC_InitStructure;
uint16_t CCR1_Val = 0;	//interrupt dependent on freq of note to be played
uint16_t CCR2_Val = 0; // "  "
uint16_t attack1, attack2, decay1, decay2 =0;
TIMER_ID score1ID, score2ID, note1ID, note2ID;
NotePtr score1note, score2note; 
static unsigned short durCount, durCount2=0;
unsigned short I0,I1, Out, Out2 = 0; 

static uint8_t VolDiv = 1; 

// 12-bit 32-element sine wave
const unsigned short wave[32] = {  
  1024,1200,1368,1524,1660,1772,1855,1907,1924,1907,1855,
  1772,1660,1524,1368,1200,1024,848,680,524,388,276,
  193,141,124,141,193,276,388,524,680,848
}; 

/**
 * @brief Initializes the speaker
 * @retval None
 */
void Blox_Speaker_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);
}

/**
 * @brief Plays the music included in blox_music.h
 * @retval None
 */
void PlayMusic(void)
{	
  CCR1_Val = score[0].noteName;
  CCR2_Val = score2[0].noteName;
  
  Blox_Timer_Init(4, SpkClock);
  score1ID = Blox_Timer_Register_IRQ(4, CCR1_Val, &Sin_gen, ENABLE);
  score2ID = Blox_Timer_Register_IRQ(4, CCR2_Val, &Sin_gen2, ENABLE);
  note1ID = Blox_Timer_Register_IRQ(4, BEAT, &NoteHandler, ENABLE); 
  note2ID = Blox_Timer_Register_IRQ(4, BEAT*4, &EnvelopeGen, ENABLE); 
  
  score1note = &score[0]; //set score to first note
  score2note = &score2[0]; //set score to first note
  
  return; 
}

/**
 * @brief Stops the music
 * @retval None
 */
void StopMusic(void)
{
  score1note = &score[0]; //reset song if it has reached the end
  score2note = &score2[0]; 
  CCR1_Val = 0;
  CCR2_Val = 0;
  TIM_Cmd(TIM4, DISABLE); //disable all interrupts
}

/**
 * @brief Generates the sine wave for the treble clef
 * @retval None
 */
void Sin_gen(void){
	if(score1note->noteName==0 && (Out==1024)) //check to see if at a rest
		Out = 1024;
	else 
	 {		
		 I0 = (I0+1)&0x1F;       // 0 to 31 - for 1024 point sine wave
		 Out = wave[I0];		
	 }
	 
	 DAC_SetChannel1Data(DAC_Align_12b_R, (Out+Out2)/VolDiv);
}

/**
 * @brief Generates the sine wave for the bass clef
 * @retval None
 */
void Sin_gen2(void){
	if(score2note->noteName==0 && (Out2==1024)) //check to see if at a rest
		Out2 = 1024;   //silence
	else 
	 {		
		 I1 = (I1+1)&0x1F;       // 0 to 31 - for 32 point sine wave
		 Out2 = wave[I1];		
	 }
	 
	 DAC_SetChannel1Data(DAC_Align_12b_R, (Out+Out2)/VolDiv);
}
	 
/**
 * @brief Creates an envelope around the output waveform
 * @retval None
 */
void EnvelopeGen(void)
{
 		if(attack1 < CCR1_Val)  //attack, sine envelope
		  {
			attack1 = attack1 + (CCR1_Val/4) ; 
			Blox_Timer_Modify_IRQ(score1ID, attack1);
		  }
		  
		 else if((attack1 >= CCR2_Val) && decay1 >= 100)  //decay, sine 
		  {
			decay1 = decay1/2; 
			Blox_Timer_Modify_IRQ(score1ID, decay1);
		  }
		  
		 if(attack2 < CCR2_Val)  //attack, sine envelope
		  {
			attack2 = attack2 + (CCR2_Val/4) ; 
			Blox_Timer_Modify_IRQ(score2ID, attack2);
		  }
		  
		 else if((attack2 >= CCR2_Val) && decay2 >= 100)  //decay, sine 
		  {
			decay2 = decay2/2; 
			Blox_Timer_Modify_IRQ(score2ID, decay2);
		  }
}

/**
 * @brief Plays each note for its specified duration
 * @retval None
 */
void NoteHandler(void){
		
		durCount++;  //increment duration counter
		durCount2++;
		
		if(score1note->duration <= durCount)  //see if note has completed its duration
		  {
		    durCount = 0;
		    score1note++;       //go to next note
			CCR1_Val = score1note->noteName; 
			Blox_Timer_Modify_IRQ(score1ID, CCR1_Val);
			attack1 = 0; 
			decay1 = CCR1_Val;
			Out = 1024;
		  }
		  
		if(score2note->duration <= durCount2)  //see if note has completed its duration
		  {
		    durCount2 = 0;
		    score2note++;       //go to next note
			CCR2_Val = score2note->noteName; 
			Blox_Timer_Modify_IRQ(score2ID, CCR2_Val);
			attack2 = 0; 
			decay2 = CCR1_Val;
		    Out2 = 1024;
		  }
		  
		if((score1note->duration==0) ||( score2note->duration==0))
		  {
		    StopMusic();       //stop if score reaches end (should occur at the same time)
		  }  
}
/** @} */
