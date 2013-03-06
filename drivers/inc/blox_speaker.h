/**
 * @file    blox_speaker.h
 * @author  Ankita kaul
 * @version V0.1
 * @date    10/25/2010
 * @brief   Basic device driver header for Blox speaker
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

#ifndef __BLOX_SPEAKER_H
#define __BLOX_SPEAKER_H

/**
 * @ingroup driver_speaker
 * @{
 */
//#define SpkClock 104800 //(SpkClock = 6550*Beats/Sec, at 16bps
#define SpkClock 52400
//#define SpkClock 26200 //(SpkClock = 6550*Beats/Sec, at 4bps
#define SINE_POINTS 32

//Definitions of notes: = clock frequency/pitch frequency/# of table elements 
#define C7		SpkClock/2093/SINE_POINTS
#define B6		SpkClock/1975/SINE_POINTS
#define Bb6		SpkClock/1865/SINE_POINTS
#define A6		SpkClock/1760/SINE_POINTS
#define Ab6		SpkClock/1661/SINE_POINTS
#define G6		SpkClock/1567/SINE_POINTS
#define Gb6		SpkClock/1480/SINE_POINTS
#define F6		SpkClock/1396/SINE_POINTS
#define E6		SpkClock/1319/SINE_POINTS
#define Eb6		SpkClock/1245/SINE_POINTS
#define D6		SpkClock/1175/SINE_POINTS
#define Db6		SpkClock/1108/SINE_POINTS
#define C6		SpkClock/1046/SINE_POINTS
#define B5		SpkClock/988/SINE_POINTS
#define Bb5		SpkClock/932/SINE_POINTS
#define A5		SpkClock/880/SINE_POINTS
#define Ab5		SpkClock/830/SINE_POINTS
#define G5		SpkClock/784/SINE_POINTS
#define Gb5		SpkClock/740/SINE_POINTS
#define F5		SpkClock/698/SINE_POINTS
#define E5		SpkClock/659/SINE_POINTS
#define Eb5		SpkClock/622/SINE_POINTS
#define D5		SpkClock/587/SINE_POINTS
#define Db5		SpkClock/554/SINE_POINTS
#define C5		SpkClock/523/SINE_POINTS
#define B4		SpkClock/494/SINE_POINTS
#define Bb4		SpkClock/466/SINE_POINTS
#define A4		SpkClock/440/SINE_POINTS     //middle A = 440Hz 
#define Ab4		SpkClock/415/SINE_POINTS
#define G4		SpkClock/392/SINE_POINTS
#define Gb4		SpkClock/370/SINE_POINTS
#define F4		SpkClock/349/SINE_POINTS
#define E4		SpkClock/330/SINE_POINTS
#define Eb4		SpkClock/311/SINE_POINTS
#define D4		SpkClock/294/SINE_POINTS
#define Db4		SpkClock/277/SINE_POINTS
#define C4		SpkClock/262/SINE_POINTS
#define B3		SpkClock/247/SINE_POINTS
#define Bb3		SpkClock/233/SINE_POINTS
#define A3		SpkClock/220/SINE_POINTS
#define Ab3		SpkClock/208/SINE_POINTS
#define G3		SpkClock/196/SINE_POINTS
#define Gb3   	SpkClock/185/SINE_POINTS
#define F3    	SpkClock/175/SINE_POINTS
#define E3    	SpkClock/165/SINE_POINTS
#define Eb3   	SpkClock/155/SINE_POINTS
#define D3    	SpkClock/147/SINE_POINTS
#define Db3 	SpkClock/139/SINE_POINTS
#define C3		SpkClock/131/SINE_POINTS
#define B2		SpkClock/123/SINE_POINTS
#define Bb2		SpkClock/117/SINE_POINTS
#define A2		SpkClock/110/SINE_POINTS
#define Ab2		SpkClock/104/SINE_POINTS
#define G2		SpkClock/98/SINE_POINTS
#define Gb2   	SpkClock/93/SINE_POINTS
#define F2    	SpkClock/87/SINE_POINTS
#define E2    	SpkClock/82/SINE_POINTS
#define Eb2   	SpkClock/78/SINE_POINTS
#define D2    	SpkClock/73/SINE_POINTS
#define Db2 	SpkClock/69/SINE_POINTS
#define C2		SpkClock/65/SINE_POINTS

#define BEAT	(SpkClock/16)-1	//beat duration (16 beats per sec)
//#define BEAT	(SpkClock/8)-1 	//beat duration (8 beats per sec)
//#define BEAT	(SpkClock/4)-1	//beat duration (4 beats per sec)

/**
 * @brief Defines data a note contains
 */
struct Note
{
  unsigned short noteName;    /**< the name of the note */
  unsigned short duration;    /**< the duration the note is held */
};

typedef const struct Note NoteType;
typedef NoteType * NotePtr;

void Blox_Speaker_Init(void);
void PlayMusic(void); 
void StopMusic(void); 
/** @} */
#endif 
