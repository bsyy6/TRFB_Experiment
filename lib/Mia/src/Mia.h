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
  //char _startFlag[_startFlagLength] = {0x61, 0x64, 0x63, 0x20, 0x3A, 0x20}; // start flag of package 'adc : '
  //char _endFlag[_endFlagLength] = {0x0A};
  //unsigned char _lenMsg = _totalLength - _startFlagLength - _endFlagLength;
  //unsigned char _bfr[_totalLength - _startFlagLength] = {0};  // total length - start flag length
  //unsigned char _MiaBytesCopy[_totalLength];
  int multipliers[5] = {10000, 1000, 100, 10, 1};
  int lenBuf = 100;

public:
  Mia();
  char write(char cmdName[], int nBytes);
  char readMiaBuf(int lenMsg,unsigned char *Flag, int FlagCount, unsigned char *endFlag, int endFlagCount);
  char readMiaForces();
  char fullCalibrate();
  char fullCalibrateAndBias();
  char stopStream();
  char startStream();
  char BiasForces();
  char scale2byte(int value, int max, int min);
  void setState();
  char readMiaForcesBiased();
  int stream_count = 0;
  int stream_count_old = 0;
  unsigned char *setMiaBytes(); // not used
  void read();
  void stop();                                 // not used
  unsigned char buf[100];                      //buffer for any data recieved from Mia.
  int Forces[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // last Byte is semaphore flag
  int state = 0; // hand state, vibrate each time it changes controlledd by setState.
  int oldState = 0;
  int BiasVector[9] =    {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int ForcesBiased[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  void echo();
  // easier code to read
  char Biased = 0; // true if forces bias was calculated
  char stateChanged = 0; 
  int processed = 1; // true if this reading was processed and can be flushed (semaphore), no data is read from the hand if this is false.
  int indxN = 1; // index Normal force
  int indxT = 2; // index tangantial force
  int thmbT = 3; // thumb tangantial force
  int thmbN = 4; // thumb normal force
  
};

#endif