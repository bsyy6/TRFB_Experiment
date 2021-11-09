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
  holdTimeLimit = 250;    //[1] detect a stable hold
  coolDownTimer = 300;   //[4] enforced resting period
  
  mainTimer = 0;          //[2]
  holdTimer = 0;          //[3]
  coolDownTimerReset = coolDownTimer;
  coolDownFlag = 0;       //[5]
  droppedFlag = 0;        //[6]
  liftedFlag = 0;

}
  
// methods

void TRFB::reset()
{
    coolDownTimer  = coolDownTimerReset;
    coolDownFlag = 0;       
    droppedFlag = 0;        
    liftedFlag = 0;
    mainTimer = 0;
    holdTimer = 0;
}

void TRFB::stateUpdate(FIFO FIFO, Tactor Tactor)
{
  newState = state; // keep a copy
  switch (state) {
    case 0:
      // case 0: no feedback condition
      if (FIFO.isMore(1) && (coolDownFlag  == 0.0) && (droppedFlag == 0.0)) {
        Tactor.vibrate();
        newState = 1;
      }
      else {
        if (droppedFlag == 1){
          droppedFlag = 0;
        }
        newState = 0;
      }


      //  emergency scenario for cooling down
      if ((coolDownFlag  != 0.0) && (coolDownTimer  > 1.0)) {
        coolDownTimer--;
        if (coolDownTimer  <= 1.0) {
          reset();
          coolDownFlag = 0;
        }
      }


      //  so it doesnt count long holds as dropped
      if ((droppedFlag != 0.0) && FIFO.isEqual(minThreshold)) {
        droppedFlag = 0.0;
        newState = 0;
      }
      break;


    case 1:
      // start feedback
      mainTimer++;


      //  making sure it is a 1 lift
      if (FIFO.isMore(4) & liftedFlag == 0.0) {
        liftedFlag = 1.0;
      }


      if (mainTimer > activeTimeLimit ) {
        reset(); 
        coolDownFlag  = 1.0;
        newState = 0;
      }
      else {
        if ((FIFO.isEqual(FIFO.vector[FIFO.maxFIFOsize - 1]) || FIFO.isLess(FIFO.vector[FIFO.maxFIFOsize - 1])) && (liftedFlag == 1.0))
        {
          if (holdTimer  > holdTimeLimit)
          {
            holdTimer = 0;
            newState = 2;
          }
          else {
            holdTimer ++;
            newState = 1;
          }
        }


        //  safety for sudden drops
        if ((FIFO.isLess(FIFO.vector[0] * 0.1) || FIFO.isLess(5)) && (liftedFlag == 1.0))
        {
          reset();
          droppedFlag = 1.0;
          newState = 0;
        }
      }


      // safety in case small return to 0 state
      if (FIFO.isLess(2)&&(liftedFlag == 0.0) ){
        reset();
        newState = 0;
      }
      break;


    case 2:
      
      //  feedback remove
      if (FIFO.isLess(5))
      {
        reset();
        // flagStartState2 = 0; // forgot what it did...ops
        newState = 0;
        Tactor.vibrate();
      }
      break;
  }
  state = newState;
}