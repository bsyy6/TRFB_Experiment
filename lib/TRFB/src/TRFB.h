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
    int holdTimeLimit = 250;    //[1] detect a stable hold
    int mainTimer = 0;          //[2]
    int holdTimer = 0;          //[3]
    int coolDownTimer = 300;   //[4] enforced resting period
    int coolDownTimerReset = coolDownTimer;
    bool coolDownFlag = 0;       //[5]
    bool droppedFlag = 0;        //[6]
    bool liftedFlag = 0;         //[7]
    double maxThreshold = 220;
    double minThreshold = 120;
    double rangeThreshold = maxThreshold - minThreshold;
    char minTheshold_B = 1;
    char maxThreshold_B = 255;
  
  public:
    char state = 0;
    TRFB();
    void reset();
    void stateUpdate(FIFO FIFO, Tactor Tactor);

};

#endif