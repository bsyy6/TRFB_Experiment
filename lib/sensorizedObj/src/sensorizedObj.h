#ifndef sensorizedObj_h
#define sensorizedObj_h

#include "Arduino.h"
#include "FIFO.h"

class sensorizedObj
{
private:
  
int analogPin_L = 0;
int analogPin_R = 0;
int analogPin_B = 0;
// FSR
int analogPin_F = 0; 




public:

sensorizedObj(int AnalogPinL,int AnalogPinR,int AnalogPinB, int AnalogPinF);
double L[3] = {0,0,0};
double R[3] = {0,0,0};
double B[3] = {0,0,0};
// FSR
double F[3] = {0,0,0};

FIFO fifoL;
FIFO fifoT;
FIFO fifoR;
// FSR
FIFO fifoF;

signed int L_bias = 0;
signed int R_bias = 0;
signed int B_bias = 0;
// FSR
signed int F_bias = 0;

signed int L_biased = 0;
signed int R_biased = 0;
signed int B_biased = 0;
// FSR
signed int F_biased = 0;

void biasCalculate(int Seconds, volatile bool& intFlag);
double LPF(double *x, int analogPin);
bool calibrated = false;
void readForcesRaw();

// call read on 100 Hz 
void readFilteredOnly(); // low pass filter fc = 10 Hz, fs = 100 Hz
void read();
void write2Fifo();

// helper functions
int sat2zero(signed int num);
char scale2byte(int value, int max, int min);

double Temp[4] ={0,0,0,0};

};

#endif