/*
  Tactor.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
  Created by Waleed Alghilan, 2021.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "Mia.h"

//constructor
Mia::Mia()
{
  // Serial1 port connects to the Mia

  Serial1.begin(115200);

  // this intitalize the start and end flags in MiaBytes array
  for (int i = 0; i < _startFlagLength; i++)
  {
    MiaBytes[i] = _startFlag[i];
  }
  for (unsigned char i = 0; i < _endFlagLength; i++)
  {
    MiaBytes[_lenMsg + _startFlagLength + i] = _endFlag[i];
  }
}

// methods

// write to Mia a cutom command
char Mia::write(char cmdName[], int nBytes)
{

  // basic send command directly and get if Mia sends back confirmation only.
  char x = 0;
  unsigned long time = micros() - 1; // overall time of the function
  unsigned long timeLocal;
  int timeout = false;
  char thisByte;

  Serial1.write(cmdName, nBytes);
  time = micros() - 10; //update time

  // wait for hand response to start (if not started yet)
  while (Serial1.available() == 0 && !timeout)
  {
    timeout = ((micros() - time) > 2000UL);
    if (timeout)
    {
      Serial2.println("timeout waiting for hand response");
      return 0;
    }
  }

  timeLocal = micros() - 100;
  //simple code searching for response within timelimit
  while (Serial1.read() != 0x3C && !timeout)
  {
    timeout = (micros() - timeLocal > 12000); // enough time for 115 bytes to check
    if (timeout)
    {
      Serial2.println("no confirmation");
      return 0;
    }
  }

  timeLocal = micros() - 100;
  //received first part, check the rest
  while ((Serial1.available() < (nBytes - 2)) && !timeout)
  {
    timeout = ((micros() - timeLocal) > 2000);
    // filling the buffer 2000 because I am not expecting more than 20 bytes.
    if (timeout)
    {
      Serial2.print(Serial1.available());
      Serial2.println(" no confimation 2");
      return 0;
    }
  }

  // read the rest of response
  for (int i = 1; i < nBytes; i++)
  {
    thisByte = Serial1.read();
    if (thisByte == cmdName[i])
    {
      x = x + 1;
    }
  }

  // check if the flag is recieved correctly.
  if (x == nBytes - 3) // expecting first charecter wrong and the last two too.
  {
    return 1; // succesful transmission and confimraiton
  }
  else
  {
    return 0;
  }
}

char Mia::fullCalibrate()
{
  // stop all movment if any
  // no stop all command yet [12-10-2021]
  // calibrate command
  char commandResp = 0;
  char temp;
  commandResp = write((char *)"@AK0000000000000*\r", 18);

  // hand keeps sending data while it is calibrating ... use that to block the code and wait
  unsigned char startFlag[5] = {0x73, 0x74, 0x61, 0x20, 0x3a};
  unsigned char endFlag[1] = {0x0A};
  int startFlagCount = 5;
  char bufIsUpdated = 1;

  while (bufIsUpdated)
  {
    bufIsUpdated = readMiaBuf(buf, 100, startFlag, startFlagCount, endFlag, 1);
    if (bufIsUpdated && buf[33] == 0x2D) //* "-" sign
    {
      return 0; // calibration was interrupted
    }
  }

  while (Serial1.available())
  {
    temp = Serial1.read();
  }

  // it will then stop sending data ... because why not.
  commandResp = write((char *)"@ADI100000000000*\r", 18);

  if (commandResp == 1)
  {
    bufIsUpdated = readMiaBuf(buf, 100, startFlag, startFlagCount, endFlag, 1);
    if (bufIsUpdated && buf[34] == 0x2B && buf[35] == 0x30)
    { //*+00
      commandResp = 0;
      commandResp = write((char *)"@ADI000000000000*\r", 18);
      if (commandResp)
      {
        return 1; // completed succefully
      }
    }
  }
  return 99;
}

char Mia::readMiaBuf(unsigned char *buf, int lenBuf, unsigned char *Flag, int FlagCount, unsigned char *endFlag, int endFlagCount)
{
  // This function looks for a flag in stream ( infinite loop until it finds it)
  // exmple call in main:
  // unsigned char buf[85] = {0}; //initilaize a buffer
  // unsigned char flag[5] =  {0x61, 0x64, 0x63, 0x20, 0x3A, 0x20}; // Flag
  // unsigned char endFlag[1] = {0x0A}
  // readMiaBuf (buf, 85, flag, 5 , endFlag, 1)
  // // now the buffer will update each time the funciton is called.
  // // it returns a zero if timeout (10000 microseconds), 99 if failed, 1 if success.
  bool handStopped = true;

  int j = 0;
  unsigned char thsByte;
  unsigned long time1;
  int count = 0;
  char temp[] = {0};
  while (handStopped)
  {
    handStopped = false;
    // infinite loop until startFlag sequence is detected
    for (j = 0; j < FlagCount; j++)
    {
      // this part enforces that the program waits for the bytes
      if (Serial1.available() < FlagCount - j + 1)
      {
        time1 = micros() - 100;
        while (Serial1.available() < FlagCount - j && !handStopped)
        {
          handStopped = ((micros() - time1) > 10000);
        }
      }

      thsByte = Serial1.read();
      
      if (thsByte != Flag[j] && !handStopped)
      {
        j = -1;
        // since it is wrong, check if it is the start of new message ?
        if (thsByte == Flag[0])
        {
          j = 0;
        }
      }
    }
    
    // if it escapees this for, it must have recieved a correct message or timed out
    if (handStopped)
    { // if timed out, return 0
      return 0;
    }
  }
  

  // flush the buffer
  for (int i = 0; i < lenBuf; i++)
  {
    buf[i] = 0;
  }

  // read all the bytes and put them in buf.
  count = 0;
  while(Serial1.available() < lenBuf-FlagCount){
    // waits for the bytes to arrive
  }
  
  Serial1.readBytes(temp,lenBuf-FlagCount);
  Serial2.write(temp,lenBuf-FlagCount);
  for (int i = lenBuf - 1; i > lenBuf - endFlagCount - 1; i--)
  {
    if (temp[i] == endFlag[i + endFlagCount - lenBuf])
    {
      count++;
    }
    else
    {
      return 0;
    }
  }

  
  if (count == endFlagCount)
  {
    for (int i = 0; i < lenBuf; i++)
    {
      if (i < FlagCount )
      {
        buf[i] = Flag[i]; // fill in the start Flag
      }
      else
      {
        buf[i] = temp[i - FlagCount]; // fill in the rest in buffer
      }
    }
     return 1;
  }
  return 99; // if you see 99, RUN!
}

void Mia::startStream()
{
  write((char*)"@ADA100000000000*\r", 18);
}

void Mia::stopStream()
{
  write((char*)"@ADA000000000000*\r", 18);
}

//char Mia::readMiaForces(unsigned char *buf, int lenBuf)
//{
//}

void Mia::read()
{
  /* read Mia byte:
    1) checks if the serial buffer has the number of bytes of startFlag or more
    2) checks the buffer, if the start flag is detected:
        it reads the expected length of the message and endFlag
          if endFlag is correct, it updates the values in MiaBytes with the new message.
    errors handling:
    1) if the start flag is not detected, it will continue reading the buffer until it finds it.
    2) if the end flag is not detected, it will discard the message and escape the function.
    3) this function waits for the start flag sequence to be detected. [stuck in for loop]
  */
  static unsigned char rcvMsg = false;
  static unsigned char j = 0;

  if (Serial1.available() == SERIAL_RX_BUFFER_SIZE)
  {
    digitalWrite(LED_BUILTIN, 1); // if you see the LED turned on, you have lost some data.
  }

  // if the buffer has the length of a start flag in it.
  if (Serial1.available() >= _startFlagLength && rcvMsg == false)
  {
    // infinite for until startFlag sequence is detected
    for (; j <= _startFlagLength - 1; j++)
    {
      static unsigned char thisByte;
      thisByte = Serial1.read();
      if (thisByte != _startFlag[j])
      {
        j = 0;
        // since it is wrong, check if it is the start of new message ?
        if (thisByte == _startFlag[0])
        {
          j = j + 1;
        }
        break;
      }
    }

    // if the start flag was correct
    if (j == _startFlagLength)
    {
      rcvMsg = true;
      j = 0;
    }
  }

  // recieved a correct start flag ... now read the rest and check endFlag
  if (Serial1.available() > 0 && rcvMsg == true)
  {

    Serial1.write(Serial1.readBytes(_bfr, _totalLength - _startFlagLength));
    // ^^ probably this is debugging stuff
    for (j = 0; j < _totalLength - _startFlagLength; j++)
    {
      Serial1.write(_bfr[j]);
    }
    //to here

    for (j = 0; j <= _endFlagLength - 1; j++)
    {
      if (_bfr[_lenMsg + j] != _endFlag[j])
      {
        j = 0;
        rcvMsg = false;
        break; // message is not correct
      }
    }
    if (j == _endFlagLength)
    {
      //correct message!
      for (unsigned char i = 0; i < (_lenMsg + _endFlagLength); i++)
      {
        MiaBytes[_startFlagLength + i] = _bfr[i];
      }
      rcvMsg = false; // message recieved correctly!
      j = 0;
    }
  }

  /* Since Mia sends string and not numbers
  this code below reads the strings in MiaBytes and converts them into signed integers
  it updates Forces[].
  if the stream counter (sent from Mia) is the same it escapes and doesn't update Forces
  */
  // pkgNumber is the index of data in Forces array, Forces[pkgNumber].
  static unsigned char pkgNumber;

  for (unsigned char i = 0; i < 85; i++)
  {
    _MiaBytesCopy[i] = MiaBytes[i];
  }

  // check if this is a new package or already seen
  int stream_count = 0;
  for (int i = 79; i < 84; i++)
  {
    stream_count = stream_count + (_MiaBytesCopy[i] - 48) * multipliers[i - 79];
  }
  // if this is new data, stop all interrupts and update Forces
  if (stream_count != Forces[8])
  {
    noInterrupts();
    pkgNumber = 0;
    for (unsigned char i = 6; i < _totalLength - 1; i = i + 9)
    {
      Forces[pkgNumber] = 0;
      // read 5 digits
      for (unsigned char j = 1; j < 6; j++)
      {
        Forces[pkgNumber] = Forces[pkgNumber] + (_MiaBytesCopy[i + j] - 48) * multipliers[j - 1];
        // substracting 48 converts a string "5" in asci to an integer of 5
      }
      // check the sign
      if ('-' == _MiaBytesCopy[i])
      {
        Forces[pkgNumber] = -Forces[pkgNumber];
      }
      // update package number
      pkgNumber = pkgNumber + 1;
    }
    interrupts();
  }
}
