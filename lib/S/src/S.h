/*
  S.h - Library for string comminucation thing [ not used ] .
  Created by Waleed Alghilan, 2022.
  SSSUP - AHA
*/

#ifndef S_h
#define S_h

#include "Arduino.h"

class S
{
public:
    S();
    void recv();
    void parseData();
    void DUMPS();
    void copyData(unsigned char* buf);
    boolean newData = false;
    boolean dataIsReady = false;
   
        
private:
    boolean recvInProgress = false;
    char ndx = 0;
    int nChar = 0;
    char startMarker = '[';
    char endMarker = ']';
    char rc;
    const static int bufferSize = 20;
    unsigned char receivedChars[bufferSize];
    // variables to hold the parsed data
    unsigned char messageFromPC[bufferSize] = {0}; 
    unsigned char charFromPC[36] = {0};
};

#endif