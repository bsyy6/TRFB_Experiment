/*
  S.cpp - Library for string comminucation thing [ not used ] ..
  Created by Waleed Alghilan, 2022.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "S.h"

S::S(){
    // constructor
    if(!Serial){
        Serial.begin(115200); // if user forgot to initialize the serial.
    }
}

void S::recv() {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        

        if (recvInProgress == true) {
            if(rc == startMarker){ // in case of double start 
                ndx = 0;
                dataIsReady = false;
            }
            if (rc != endMarker) {
              if(rc != startMarker){
                receivedChars[ndx] = rc;
                ndx++;
              }
                
                if (ndx >= bufferSize) {
                    ndx = bufferSize - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                newData = true;
            }
            
        }
        else if (rc == startMarker) {
            dataIsReady = false;
            recvInProgress = true;
            ndx = 0;
        }
    }
}

//============

void S::parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index
    nChar = 0 ;
    strtokIndx = strtok(receivedChars," ");
    if(strtokIndx != NULL){ // to avoid empty strings to trigger the re
      charFromPC[nChar] = (unsigned char) (atoi(strtokIndx));     // convert this part to an integer
      nChar++;
      dataIsReady = true;
    }
    while( strtokIndx != NULL ) {
      strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
      charFromPC[nChar] = (unsigned char)atoi(strtokIndx);     // convert this part to an integer
      nChar++;
    }
    nChar--;
}

//============

void S::DUMPS() {
  if(dataIsReady){
    for(int i = 0; i < nChar; i++){
      Serial.print((unsigned int)charFromPC[i]);
      Serial.print("\t");
    }
    Serial.println("");
  }
}

void S::copyData(unsigned char* buf){
  if(dataIsReady){
    memcpy(buf, &charFromPC, nChar);   
  }
}