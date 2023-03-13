/*
  Tactor.h - Library for flashing Morse code.
  Created by Waleed Alghilan, November 2, 2021.
  SSSUP - AHA
*/

#ifndef TRFB_h
#define TRFB_h

#include "Arduino.h"
#include "FIFO.h"
#include "Tactor.h"

class TRFB
{
private:
  char newState = 0;
  int activeTimeLimit = 500; //[0] can't go more than 5 second of active feedback ( assume interrupts at 100 Hz )
  int holdTimeLimit = 50;   //[1] detect a stable hold
  int mainTimer = 0;         //[2]
  int holdTimer = 0;         //[3]
  int coolDownTimer = 1;   //[4] enforced resting period
  int coolDownTimerReset = coolDownTimer;
  bool coolDownFlag = 0; //[5]
  bool droppedFlag = 0;  //[6]
  unsigned int t= 0;
  unsigned int t2=0;
  
  char waitt = 0;

  double maxThreshold = 220;
  double minThreshold = 120;
  double rangeThreshold = maxThreshold - minThreshold;
  char minTheshold_B = 1;
  char maxThreshold_B = 255;
  // parameters
  int lift = 20;
  int xlift = 10;
  int touch = 20;
  int xtouch = 15;
  //
  void feedbackOn(unsigned int Pos, Tactor tactor);
  void feedbackFade(int& Pos, Tactor tactor);
  void feedbackOff( Tactor tactor);
  void stateUpdate(FIFO fifo, FIFO fifo1,FIFO fifo2, int step);
  int flagStartFade = 0;
  int Max = 0;


public:
  char state = 0;
  volatile int fadeTime = 100; // 100 = 1 second
  TRFB();
  void reset();
  void trfb(FIFO fifo,FIFO fifo1, FIFO fifo2, int Pos, Tactor tactor);
  bool liftedFlag = 0; //[7]
  char vibrate = 0;
  char vib_state = 0; // viberates on different conditions
  // 00: not defined. 
  // 01: touched .
  // 02: lifted without touching.
  // 03: replaced without tounching.
  // 04: lifted after touching before stable grasp.
  // 05: repalced after touching before stable grasp.
  // 06: sudden drops.
  // 07: removed touch before lifitng.
  // 08: lifted the object after stable grasp.
  // 09: replaced object after stable grasp.
  // 10: remved touch from the object after stable grasp.
  unsigned char fb_type = 0; 
  /* 
     0 no feedback
     1 continuous feedback 
     2 Transitory feedback 
     3 DESC 
     4 continuous without vibration 
     5 tranistory witout vibration
  */
};

#endif