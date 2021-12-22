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
    unsigned char vector[10] = {0};
    unsigned char newData = 0;
    double err = 0.01; // error margin when computing values isLess/isMore ...
    
    double voteThreshold = 0.6;
    double counter;
    double sumVotes;
    FIFO();
    void append(char newData);
    bool isEqual(double number);
    bool isMore (double number);
    bool isLess (double number);
    char last();
    int FIFOsize;
    


  private:
   // look up table for stroke positions

};

#endif