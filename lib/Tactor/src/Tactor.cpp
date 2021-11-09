/*
  Tactor.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
  Created by Waleed Alghilan, 2021.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "Tactor.h"


//constructor
Tactor::Tactor()
{
  // Which Serial2 port connects to the tactors
  int _board = 0x01;
  Serial2.begin(115200);

  // look up table for stroke positions
  unsigned int _STROKES[256]={ 0 , 19 , 38 , 59 , 80 , 101 , 124 , 147 , 170 , 195 , 220 , 246 , 272 , 300 , 327 , 356 , 386 , 416 , 446 , 478 , 510 , 543 , 577 , 611 , 646 , 682 , 718 , 755 , 793 , 832 , 871 , 911 , 952 , 993 , 1036 , 1079 , 1122 , 1167 , 1212 , 1257 , 1304 , 1351 , 1399 , 1448 , 1497 , 1548 , 1599 , 1650 , 1703 , 1756 , 1810 , 1864 , 1919 , 1976 , 2032 , 2090 , 2148 , 2207 , 2267 , 2328 , 2389 , 2451 , 2514 , 2577 , 2642 , 2707 , 2772 , 2839 , 2906 , 2975 , 3043 , 3113 , 3183 , 3255 , 3327 , 3399 , 3473 , 3547 , 3622 , 3698 , 3775 , 3852 , 3930 , 4009 , 4089 , 4170 , 4251 , 4333 , 4416 , 4500 , 4584 , 4670 , 4756 , 4843 , 4931 , 5019 , 5109 , 5199 , 5290 , 5382 , 5475 , 5568 , 5663 , 5758 , 5854 , 5951 , 6049 , 6147 , 6247 , 6347 , 6448 , 6550 , 6653 , 6757 , 6861 , 6967 , 7073 , 7180 , 7288 , 7397 , 7507 , 7618 , 7730 , 7842 , 7955 , 8070 , 8185 , 8301 , 8418 , 8536 , 8655 , 8775 , 8895 , 9017 , 9139 , 9263 , 9387 , 9513 , 9639 , 9766 , 9894 , 10024 , 10154 , 10285 , 10417 , 10550 , 10684 , 10819 , 10955 , 11092 , 11230 , 11369 , 11509 , 11650 , 11792 , 11935 , 12080 , 12225 , 12371 , 12518 , 12666 , 12816 , 12966 , 13118 , 13270 , 13424 , 13579 , 13734 , 13891 , 14049 , 14208 , 14369 , 14530 , 14692 , 14856 , 15021 , 15187 , 15354 , 15522 , 15691 , 15862 , 16034 , 16207 , 16381 , 16556 , 16733 , 16910 , 17089 , 17270 , 17451 , 17634 , 17818 , 18003 , 18190 , 18377 , 18567 , 18757 , 18949 , 19142 , 19336 , 19532 , 19729 , 19928 , 20128 , 20329 , 20532 , 20736 , 20941 , 21148 , 21357 , 21566 , 21778 , 21991 , 22205 , 22421 , 22638 , 22857 , 23077 , 23299 , 23523 , 23748 , 23974 , 24203 , 24433 , 24664 , 24897 , 25132 , 25369 , 25607 , 25847 , 26089 , 26333 , 26578 , 26825 , 27074 , 27325 , 27578 , 27832 , 28088 , 28347 , 28607 , 28869 , 29133 , 29400 , 29668 , 29938 , 30211 , 30485 , 30762 , 31040 , 31321 , 31604 , 31890 , 32177 , 32467 , 32759};
  unsigned long _stroke = 0;
}

// methods

// write to tactors cutom command
void Tactor::write(char cmdName[], char nBytes){
    for (char i = 0; i <nBytes; i++)
    {
      Serial2.write(cmdName[i]);
    }
}

// stops motor
void Tactor::stop(){
      Serial2.write(0x5F);
      Serial2.write(_board);
      Serial2.write(0x71);
      Serial2.write(_board);
}

void Tactor::vibrate(){
      Serial2.write(_board);
      Serial2.write(0x80);
      Serial2.write(0x4A);
      Serial2.write(_board);
}

int Tactor::getStatus(){
      Serial2.write(0x5F);
      Serial2.write(_board);
      Serial2.write(0x70);
      Serial2.write(_board);
      _startWaitTime = millis();
      while (millis() - _startWaitTime < 1000){  
        if (Serial2.available()){
          return(Serial2.read());
        }
      }
      return(-1);
}

void Tactor::setPosition( char vars[4]){
      Serial.write(0x5F);
      Serial.write(_board);
      Serial.write(0x21+(vars[1]*16));
      Serial.write(vars[2]);
      Serial.write(vars[3]);
      Serial.write(_board);
}

void Tactor::feedbackOn(){
  //stroke = findStroke();
  //long2buf(stroke,buf);
  //checkStatus('setp', buf , board); 
  
}