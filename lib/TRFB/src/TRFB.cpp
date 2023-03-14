/*
  Tactor.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
  Created by Waleed Alghilan, 2021.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "TRFB.h"
#include "FIFO.h"
#include "Tactor.h"

//constructor
TRFB::TRFB()
{
  activeTimeLimit = 500; //[0] can't go more than 5 second of active feedback ( assume interrupts at 100 Hz )
  holdTimeLimit = 50;    //[1] detect a stable hold within 500msec
  coolDownTimer = 150;   //[4] enforced resting period

  mainTimer = 0; //[2]
  holdTimer = 0; //[3]
  coolDownTimerReset = coolDownTimer;
  coolDownFlag = 0; //[5]
  droppedFlag = 0;  //[6]
  liftedFlag = 0;
  t = 0;
}

// methods
void TRFB::reset()
{
  coolDownTimer = coolDownTimerReset;
  coolDownFlag = 0;
  // droppedFlag = 0;
  // liftedFlag = 0;
  mainTimer = 0;
  holdTimer = 0;
}

// main function call every 10 mseconds;
// fifo normal forces/troques
// fifo2 tangantial focres/torques
// Pos normal forces scaled down between 0-255 max
// tactor is the communication protocol for tactor made by Clemente 2015.
void TRFB::trfb(FIFO fifo, FIFO fifo1, FIFO fifo2, int Pos, Tactor tactor)
{
  t = t + 1;  
  if ( waitt > 0 ){
    waitt --;
  }

  if (vibrate > 0 && waitt == 0  )
  {
    if((fb_type <= 3 && fb_type > 0))
    {
      tactor.vibrate(4,5); 
      vibrate--;
      waitt = 15;
    }else{
      vibrate = 0;
    }
  }

  switch ((int)state)
  {
  case 0:
    feedbackOff(tactor);
    break;
    
  case 1:
    if(fb_type != 0 && fb_type != 3){
    feedbackOn(Pos, tactor);
    }
    break;

  case 2:
    if(fb_type == 2 || fb_type == 5){
     feedbackFade(Pos, tactor);
    }
    if( fb_type == 1 || fb_type == 4){
     feedbackOn(Pos, tactor);
    }
    break;

  default:
    feedbackOff(tactor);
    break;
  }
  stateUpdate(fifo, fifo1, fifo2, 0);
}

// states:
// 0 -> nothing happending
// 1 -> object is in hand (continuous feedback is on)
// 2 -> Feedback fade-away
//                          FIFOR       FIFOL       FIFOT
void TRFB::stateUpdate(FIFO fifo, FIFO fifo1, FIFO fifo2, int step)
{
  newState = state; // keep a copy
  switch (state)
  {
  case 0:
    // case 0: no feedback condition
    // fifo.isMore(touch) ||
    if ((fifo1.isMore(touch)) && (coolDownFlag == 0.0) && (droppedFlag == 0.0))
    {
      // detection of first grasp.
      vibrate++;
      vib_state = 1;
      // debug
      newState = 1;
    }
    else
    {
      if (droppedFlag == 1)
      {
        droppedFlag = 0;
      }
      newState = 0;
    }

    //  emergency scenario for cooling down
    /*(if ((coolDownFlag != 0.0) && (coolDownTimer > 1.0))
    {
      coolDownTimer--;
      if (coolDownTimer <= 1.0)
      {
        reset();
        coolDownFlag = 0;
      }
    }
    */
    // so it viberates even if no touch is detected.  
    if(liftedFlag==0.0){
      // lifted the object
      if(fifo2.isMore(lift)){
        liftedFlag = 1.0;
        vibrate++;
        vib_state= 2;
      }
    }else{
      // dropped the object
      if(fifo2.isLess(xlift)){
        liftedFlag = 0.0;
        vibrate++;
        vib_state= 3;
      }
    }

    //  so it doesnt count long light holds as dropped
    if ((droppedFlag != 0.0) && fifo1.isEqual(0))
    {
      droppedFlag = 0.0;
      newState = 0;
    }
    break;

  case 1:
    // start feedback
    mainTimer++;

    if(liftedFlag==0.0){
      // lifted the object
      if(fifo2.isMore(lift)){
        liftedFlag = 1.0;
        vibrate++;
        vib_state= 4;
      }
    }else{
      // dropped the object
      if(fifo2.isLess(xlift)){
        liftedFlag = 0.0;
        vibrate++;
        vib_state= 5;
      }
    }

    if (mainTimer > activeTimeLimit)
    {
      //reset();
      //coolDownFlag = 1.0;
      //newState = 0;
    }
    else
    {
      if ((fifo1.isEqual2(fifo1.vector[fifo1.maxFIFOsize - 1])) && (liftedFlag == 1.0)) // I used to have isLess here.
      {
        if (holdTimer > holdTimeLimit)
        {
          holdTimer = 0;
          newState = 2;
        }
        else
        {
          holdTimer++;
        }
      }
    }

    //  safety for sudden drops
    if ((fifo1.isLess(fifo1.vector[0] * 0.1)) && (liftedFlag == 1.0))
    {
      //  reset();
      //  droppedFlag = 1.0;
      //  vibrate++;
      //  vib_state= 6;
      //  newState = 0;
    }

    // safety in case small return to 0 state
    // ((fifo.isLess(xtouch)||fifo1.isLess(xtouch)) && (liftedFlag == 0.0))
    if ((fifo1.isLess(xtouch)) && (liftedFlag == 0.0) && (!droppedFlag))
    {
      reset();
      vibrate++;
      vib_state= 7;
      newState = 0;
    }

    break;

  case 2:
    if(liftedFlag==0.0){
      // lifted the object
      if(fifo2.isMore(lift)){
        liftedFlag = 1.0;
        vibrate++;
        vib_state= 8;
      }
    }else{
      // replaced the object
      if(fifo2.isLess(xlift)){
        liftedFlag = 0.0;
        vibrate++;
        vib_state= 9;
      }
    }
    //  feedback remove
    // (fifo.isLess(xtouch) || fifo1.isLess(xtouch)
    // released
    if (fifo1.isLess(xtouch))
    {
      reset();
      newState = 0;
      // coolDownFlag = 1.0;
      vibrate++;
      vib_state= 10;
    }

    break;
  }
  state = newState;
}

void TRFB::feedbackOn(unsigned int Pos, Tactor tactor)
{
  tactor.setPosition(Pos);
  tactor.On = true;
}

void TRFB::feedbackFade(int& Pos, Tactor tactor)
{
  // if we are starting the fade
  if (flagStartFade == 0)
  {
    Max = Pos;
    flagStartFade = 1;
    t2 = t + fadeTime;
  }
  // in the fade time window.
  if (t <= t2)
  {
    Pos =  (int)(Max*double(t2-t)/fadeTime); // linear
  }
  else
  {
    Pos = 0;
  }
  tactor.setPosition(Pos);
}

void TRFB::feedbackOff(Tactor tactor)
{
  if (tactor.On)
  {
    //tactor.setPosition(0);
    tactor.On = false;
  }
  flagStartFade = 0;
}