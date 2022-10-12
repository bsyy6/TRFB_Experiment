/*
  FIFO.h - Library for flashing Morse code.
  Created by Waleed Alghilan, November 2, 2021.
  SSSUP - AHA
*/

#ifndef FIFO_h
#define FIFO_h

#include "Arduino.h"

class FIFO
{
  public:
    int maxFIFOsize = 10;
    int vector[11] = {0}; //I add one extra on purpose. check append method.
    int newData = 0;
    double err = 0.05; // error margin when computing values isLess/isMore ...
    
    double voteThreshold = 0.6;
    double counter;
    double sumVotes;
    FIFO();
    void append (int newData);
    bool isEqual(double number);
    bool isMore (double number);
    bool isLess (double number);
    int last();
    int movingAvg();
    long runningSum;
    int FIFOsize =0;
    bool isEqual2(double number);
    


  private:
   // nothing.

};

#endif