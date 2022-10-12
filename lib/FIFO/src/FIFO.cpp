/*
  FIFO.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
  Created by Waleed Alghilan, 2021.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "FIFO.h"


//constructor
FIFO::FIFO()
{
  FIFOsize = 0;
  newData = 0;
  counter = 0;
  for (int i = 1; i <= maxFIFOsize; i++) {
    counter += i;
  }
  sumVotes = counter;
}

// methods
void FIFO::append(int newData) {
  vector[FIFOsize] = newData;
  if(FIFOsize >= maxFIFOsize){    
    runningSum = runningSum - vector[0] + vector[FIFOsize]; //used in moving average
    // shift FIFO to the left by one step
    for (int i = 0; i < maxFIFOsize; i++)
    {
      vector[i] = vector[i + 1];
    }
    FIFOsize = maxFIFOsize;
  }else{
    runningSum += newData;
    FIFOsize+=1;
  }
}

int FIFO::last(){
    return(vector[FIFOsize-1]);
}

bool FIFO::isEqual(double number)
{
  // this function compares if the FIFO values are equal to a number within some error margin (err)
  double Lowest_threshold = number * (1.0 - err);
  double Highest_threshold = number * (1.0 + err);
  double voteCounter = 0.0;



  for (int i = 0; i < FIFOsize; i++)
  {
    double d1;
    double vote;
    double weight;


    d1 = vector[i];
    vote = (double)(d1 <= Highest_threshold) && (double)(d1 >= Lowest_threshold);
    weight = ((double)(i) + 1.0);
    voteCounter += weight * vote;


  }
  // 0.6 is the vote of 60% as a threshold
  return ((voteCounter / sumVotes) > voteThreshold);
}


bool FIFO::isMore(double number)
{
  // this function compares if the FIFO values are larger than a number within some error margin (err)
  double voteCounter = 0.0;
  double Highest_threshold = number * (1.0 + err);


  for (int i = 0; i < FIFOsize; i++)
  {
    double d1;
    double vote;
    double weight;


    d1 = vector[i];
    vote = (double)(d1 >= Highest_threshold);
    weight = ((double)(i) + 1.0);
    voteCounter += weight * vote;


  }
  return ((voteCounter / sumVotes) > voteThreshold);
}


bool FIFO::isLess(double number)
{
  // this function compares if the FIFO values are larger than a number within some error margin (err)
  double Lowest_threshold =  number * (1.0 - err);
  double voteCounter = 0.0;


  for (int i = 0; i < FIFOsize; i++)
  {
    double d1;
    double vote;
    double weight;


    d1 = vector[i];
    vote = (double)(d1 <= Lowest_threshold);
    weight = (double)(i) + 1.0;
    voteCounter += weight * vote;

  }

  return ((voteCounter / sumVotes) > voteThreshold);
}

int FIFO::movingAvg(){
 return(runningSum/FIFOsize);
}


bool FIFO::isEqual2(double number)
{
  // this function compares if the FIFO values are equal to a number within some error margin (err)
  double Lowest_threshold = 0;
  double Highest_threshold = number + 1;
  double voteCounter = 0.0;
  if(number > 1){
  Lowest_threshold = number  - 1;
  }

  for (int i = 0; i < FIFOsize; i++)
  {
    double d1;
    double vote;

    d1 = vector[i];
    vote = (double)(d1 <= Highest_threshold) && (double)(d1 >= Lowest_threshold);
    voteCounter += vote;
  }
  // 0.6 is the vote of 60% as a threshold
  return ((voteCounter / 10) > voteThreshold);
}