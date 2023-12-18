/*
  Tactor.h - Library for flashing Morse code.
  Created by Waleed Alghilan, November 2, 2021.
  SSSUP - AHA
*/

#ifndef Tactor_h
#define Tactor_h

#include "Arduino.h"

class Tactor
{
public:
  Tactor();
  void write(char cmdName[], char nBytes = 0);
  void setPosition(unsigned int Pos);
  void setPosition_benchTest(unsigned int Pos); // takes encoder positions instead
  void stop();
  void vibrate(unsigned char N, unsigned char dur);
  void feedbackOn();
  void getStatus(bool wait = false);
  void getPosition(bool wait = false);
  void setPWM(bool opn, unsigned int PWM);
  void PWM_MOTORS(bool opn, bool wait = true);
  char setPID(char Kp, char Ki, char Kd, char _err);
  void getPID();
  void dumpPID();
  void Calibrate();
  void statusREADER();
  void Start();
  void Test1();
  void Test2();
  void Test3();
  //int readTactor()
  bool On = false;
  unsigned char status;
  unsigned long stroke;
  unsigned int position;
  bool statusChanged;
  bool Ready = false;
  char prevStatus = 0;
  char status_str[18][10] = {"000-STOP","001-PWM","010-Pos","011-Ten",
                               "100-Cur"," "      ,"110-CrP","111-ERR",
                               "-ctrl!k", "-ctrlok","-FCAOff","-FCAOn",
                               "-FCBOff","-FCBOn","-I<Imax","-I>Imax",
                               "-moving","-steady"};
  
unsigned int _STROKES[256] = {0, 19, 38, 59, 80, 101, 124, 147, 170, 195, 220, 246, 272, 300, 327, 356, 386, 416, 446, 478, 510, 543, 577, 611, 646, 682, 718, 755, 793, 832, 871, 911, 952, 993, 1036, 1079, 1122, 1167, 1212, 1257, 1304, 1351, 1399, 1448, 1497, 1548, 1599, 1650, 1703, 1756, 1810, 1864, 1919, 1976, 2032, 2090, 2148, 2207, 2267, 2328, 2389, 2451, 2514, 2577, 2642, 2707, 2772, 2839, 2906, 2975, 3043, 3113, 3183, 3255, 3327, 3399, 3473, 3547, 3622, 3698, 3775, 3852, 3930, 4009, 4089, 4170, 4251, 4333, 4416, 4500, 4584, 4670, 4756, 4843, 4931, 5019, 5109, 5199, 5290, 5382, 5475, 5568, 5663, 5758, 5854, 5951, 6049, 6147, 6247, 6347, 6448, 6550, 6653, 6757, 6861, 6967, 7073, 7180, 7288, 7397, 7507, 7618, 7730, 7842, 7955, 8070, 8185, 8301, 8418, 8536, 8655, 8775, 8895, 9017, 9139, 9263, 9387, 9513, 9639, 9766, 9894, 10024, 10154, 10285, 10417, 10550, 10684, 10819, 10955, 11092, 11230, 11369, 11509, 11650, 11792, 11935, 12080, 12225, 12371, 12518, 12666, 12816, 12966, 13118, 13270, 13424, 13579, 13734, 13891, 14049, 14208, 14369, 14530, 14692, 14856, 15021, 15187, 15354, 15522, 15691, 15862, 16034, 16207, 16381, 16556, 16733, 16910, 17089, 17270, 17451, 17634, 17818, 18003, 18190, 18377, 18567, 18757, 18949, 19142, 19336, 19532, 19729, 19928, 20128, 20329, 20532, 20736, 20941, 21148, 21357, 21566, 21778, 21991, 22205, 22421, 22638, 22857, 23077, 23299, 23523, 23748, 23974, 24203, 24433, 24664, 24897, 25132, 25369, 25607, 25847, 26089, 26333, 26578, 26825, 27074, 27325, 27578, 27832, 28088, 28347, 28607, 28869, 29133, 29400, 29668, 29938, 30211, 30485, 30762, 31040, 31321, 31604, 31890, 32177, 32467, 32759};
unsigned int pos_temp = 0;
unsigned char kp;
unsigned char ki;
unsigned char kd;
unsigned char er;
unsigned int _Sine[101] ={20480 , 22215 , 23934 , 25622 , 27265 , 28848 , 30356 , 31777 , 33098 , 34306 , 35392 , 36345 , 37158 , 37822 , 38333 , 38685 , 38876 , 38903 , 38767 , 38468 , 38010 , 37396 , 36632 , 35725 , 34682 , 33513 , 32229 , 30840 , 29360 , 27800 , 26176 , 24501 , 22790 , 21059 , 19323 , 17597 , 15896 , 14236 , 12632 , 11097 ,  9646 ,  8291 ,  7044 ,  5916 ,  4917 ,  4057 ,  3342 ,  2780 ,  2374 ,  2130 ,  2048 ,  2130 ,  2374 ,  2780 ,  3342 ,  4057 ,  4917 ,  5916 ,  7044 ,  8291 ,  9646 , 11097 , 12632 , 14236 , 15896 , 17597 , 19323 , 21059 , 22790 , 24501 , 26176 , 27800 , 29360 , 30840 , 32229 , 33513 , 34682 , 35725 , 36632 , 37396 , 38010 , 38468 , 38767 , 38903 , 38876 , 38685 , 38333 , 37822 , 37158 , 36345 , 35392 , 34306 , 33098 , 31777 , 30356 , 28848 , 27265 , 25622 , 23934 , 22215 ,20480 };
private:
  // look up table for stroke positions
  unsigned long _stroke = 0;
  unsigned int bias = 0;
  int _board = 1;
  unsigned long _startWaitTime = 0;
};

#endif