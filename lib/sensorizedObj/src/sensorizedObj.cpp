#include "sensorizedObj.h"
#include "FIFO.h"

// the constructor
sensorizedObj::sensorizedObj(int AnalogPinL, int AnalogPinB, int AnalogPinR, int AnalogPinF)
{
    analogPin_L = AnalogPinL;
    analogPin_R = AnalogPinR;
    analogPin_B = AnalogPinB;
    // FSR
    analogPin_F = AnalogPinF;
}


double sensorizedObj::LPF(double *x, int analogPin)
{
    // Low Pass filter
    // warning: these values are assuming 100Hz sampling rate if changed you need to make a new filter design.
    x[1] = x[0];                                                                                 // x[n-1] from previous call
    x[0] = analogRead(analogPin);                                                                // x[n] now
    //x[2] = 0.23906 * x[0] + 0.23906 * x[1] + 0.52189 * x[2]; //x[2] is y[n-1] for the next step.
    x[2] = 0.1367*x[0] + 0.1367*x[1] + 0.7265*x[2]; // fc @ 0.5 Hz
    return (x[2]);
}

void sensorizedObj::readForcesRaw()
{
    // not filtered, raw data.
    L[0]=analogRead(analogPin_L);
    R[0]=analogRead(analogPin_R);
    B[0]=analogRead(analogPin_B);
    // FSR
    F[0]=analogRead(analogPin_F);
    
}



void sensorizedObj::read()
{
    L_biased = LPF(L, analogPin_L) - L_bias;
    R_biased = LPF(R, analogPin_R) - R_bias; 
    B_biased = - LPF(B, analogPin_B) + B_bias; // this sensor is inverted 
    //FSR
    F_biased = LPF(F, analogPin_F) - F_bias;
}

void sensorizedObj::write2Fifo(){
  read();
  fifoL.append(sat2zero(L_biased));
  fifoR.append(sat2zero(R_biased));
  fifoT.append(sat2zero(B_biased));
  //FSR
  fifoF.append(B_biased); // without zeroing if under!
}

void sensorizedObj::biasCalculate(int Seconds, volatile bool& intFlag)
{
  static long previousTime = millis();
  // watis for 1 second
  while (millis() - previousTime < 1000)
  {
  }

  // reset bias
  L_bias = 0;
  R_bias = 0;
  B_bias = 0;
  //FSR
  F_bias = 0;

  intFlag = false;
  int i = 0;
  while (i < Seconds * 100)
  {
    if (intFlag) // 100Hz sampling
    {
      //readForcesRaw();
      
      Temp[0] += LPF(L, analogPin_L);
      Temp[1] += LPF(R, analogPin_R);
      Temp[2] += LPF(B, analogPin_B);
      //FSR
      Temp[3] += LPF(F, analogPin_F);

      i = i + 1;
      intFlag = false;
    }
  }
  L_bias = round(Temp[0] / i);
  R_bias = round(Temp[1] / i);
  B_bias = round(Temp[2] / i);
  //FSR
  F_bias = round(Temp[3] / i);
  calibrated = true;
  intFlag = false;
}

int sensorizedObj::sat2zero(signed int x){
  if(x<0){
    return(0);
  }
  else
  {
   return((int)x);
  }
}