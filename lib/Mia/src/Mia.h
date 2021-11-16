/*
  Tactor.h - Library for flashing Morse code.
  Created by Waleed Alghilan, November 2, 2021.
  SSSUP - AHA
*/

#ifndef Mia_h
#define Mia_h

#include "Arduino.h"

class Mia
{
  private:

    static const unsigned char _totalLength = 85; // total length of message (including start and end flags)
    static const unsigned char _startFlagLength = 6;
    static const unsigned char _endFlagLength = 1;
    char _startFlag[_startFlagLength] = {0x61, 0x64, 0x63, 0x20, 0x3A, 0x20}; // start flag of package 'adc : ' 
    char _endFlag[_endFlagLength] = {0x0A};                                 
    unsigned char _lenMsg = _totalLength - _startFlagLength - _endFlagLength; 
    unsigned char _bfr[_totalLength - _startFlagLength] = {0};  // total length - start flag length
    unsigned char _MiaBytesCopy[_totalLength];
    int multipliers[5] = {10000, 1000, 100, 10, 1};
    unsigned char buf[100]; //buffer for any data recieved from Mia.
    
  public:
    Mia();
    char write(char cmdName[], int nBytes);
    char readMiaBuf( unsigned char *buf, int lenBuf, unsigned char *Flag, int FlagCount, unsigned char* endFlag, int endFlagCount );
    char fullCalibrate();
    void stopStream();
  void startStream();
    unsigned char* setMiaBytes(); // not used
    void read();
    void stop(); // not used
    char MiaBytes[_totalLength] = {0};
    int Forces [8] = {};
};

#endif