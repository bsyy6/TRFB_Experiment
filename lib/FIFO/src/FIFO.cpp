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
void FIFO::append(char newData) {
  // if FIFO is still empty
  if (FIFOsize < maxFIFOsize - 1) {
    vector[FIFOsize] = newData;
    FIFOsize += 1;
  }
  // once full ...
  else
  {
    // shift FIFO to the left by one step
    for (int i = 0; i < (maxFIFOsize - 1); i++)
    {
      vector[i] = vector[i + 1];
    }
    // add the latest data point;`
    vector[FIFOsize] = newData;
  }
}

char FIFO::last(){
 if (FIFOsize < maxFIFOsize - 1) {
    return(vector[FIFOsize-1]);
  }
  // once full ...
  else
  {
    return( vector[maxFIFOsize - 1]);
  }
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