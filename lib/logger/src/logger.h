// /*
//   FIFO.h - Library for flashing Morse code.
//   Created by Waleed Alghilan, November 2, 2021.
//   SSSUP - AHA
// */

#ifndef logger_h
#define logger_h

#include "Arduino.h"
#include "Arduino.h"
#include "FIFO.h"
#include "MIA.h"
#include "TRFB.h"

class LOGGER
{
  public:
    LOGGER();
    void logData(Mia mia, TRFB Fb);

};

#endif