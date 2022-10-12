// /*
//   FIFO.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
//   Created by Waleed Alghilan, 2021.
//   SSSA - AHA lab
// */

#include "Arduino.h"
// #include "FIFO.h"
// #include "MIA.h"
#include "Tactor.h"
#include "TRFB.h"
#include "LOGGER.h"
#include "sensorizedObj.h"
//constructor
LOGGER::LOGGER()
{
  Serial.begin(115200);  
}

void LOGGER::logData(TACTOR tactor, TRFB Fb, sensorizedObj Sobj){
   Serial.write(sys_on);
   Serial.print(",");
   Serial.write(Record_on);
   Serial.print(",");
   Serial.write(Trial_number);
   Serial.print(",");
   Serial.print(tactor.position);
   Serial.print(",");
   Serial.write(Fb.TypeOfFeedback);
   Serial.print(",");
   Serial.write(Fb.state);
   Serial.print(",");
   Serial.print(Fb.liftedFlag);
   Serial.print(",");
   Serial.write(Fb.vibrate);
   Serial.print(",");
   Serial.write(failed);
   Serial.print(",");
   Serial.write(error);
   Serial.print("\n");
}

if(Serial.available()){
      if(Serial.read() == 1) record = 1;
      if(Serial.read() == 0) record = 0;
      if(Serial.read() == 'r') {
        // repeat this experiment
        count--;
        count_old--;
        err_flag = 1;      // flag error 
      }
      if(Serial.read() == 'p'){
        move_robots = 0;
      }
}