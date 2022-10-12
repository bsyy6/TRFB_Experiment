/*
  PID.h - Library for PID.
  Created by Waleed Alghilan, 2022.
  SSSUP - AHA
*/

#ifndef PID_h
#define PID_h

#include "Arduino.h"

class PID
{
public:
  PID(unsigned int kp,unsigned int ki,unsigned int kd, float dt, unsigned int N, unsigned int deadZone);
  signed int getPID(signed int SP, signed int PV);
  void resetPID(); 
        
private:
    
};

#endif