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

// write to tactors a cutom command

char Mia::write(char cmdName[], int nBytes)
{

  // warning: this function blocks the execution of main loop and waits for confirmation  from mia
  //          or timeOut time to pass.
  //          THIS CODE IS *NOT* to be used in time critical scenarios.

  int counter = 0;
  int maxCounter = nBytes + 2; // I will give it nbytes +1 tries to to get the message
  char x = 0;
  char temp = 0;
  unsigned long time = micros() - 1; // overall time of the function
  int timeout = false;
  int timeoutCounter = 0;
  char dump = 0;

  // [00] clear serial buffer
  // send stop all serial stream command
  Serial1.write("@Ad0000000000000*\r", 18);
  while (Serial1.available() == 0 && !timeout)
  {
    timeout = ((micros() - time) > 2000); // waits for hand response.
    if (timeout)
    {
      Serial2.println("1st while timedout!");
      return 0;
    }
  }

  // first stop all incoming data streams from the hand
  while (Serial1.available() && !timeout)
  {
    timeout = ((micros() - time) > 2000);
    dump = Serial1.read();
    //hand is not stopping the streaming, quit write.
    if (timeout)
    {
      Serial2.println("1st while timedout!");
      return 0;
    }
  }

  // stopped streaming .. send command
  Serial1.write(cmdName, nBytes);

  time = micros() - 10; //update time
  unsigned long deltaTime = micros() - time;

  while ((Serial1.available() == 0) && !timeout)
  {
    deltaTime = micros() - time;
    timeout = (deltaTime > 10000UL); // @ 115200 baud rate a byte shouldn't take more than 10 microsecond, we will give it 50.
  }

  if (timeout)
  {
    Serial2.println("timeout_1: sent command and dind't get response.");
    Serial2.println(deltaTime);
    return 0;
  }

  while (!timeout && (counter < maxCounter))
  {

    Serial2.print("in to first while \r");
    // force wait for each byte
    while (Serial1.available() == 0 && !timeout)
    {
      timeout = ((micros() - time) > 100);
      if (timeout)
      {
        Serial2.print("timeout_2! \r");
        return 0;
      }
    }

    // check first byte
    if (Serial1.read() == 0x3C)
    {
      counter = counter + 1;
      x = 1;
      for (int i = 1; i < nBytes; i++)
      {
        while (Serial1.available() == 0 && !timeout)
        {
          timeout = ((micros() - time) > 50000); // 18 bytes should take about 180 Micros + 50Micros bytes for the first byte, we will give this 250 microsecond
        }
        if (timeout)
        {
          Serial2.print((int)(micros() - time));
          Serial2.print("timeout line 110");
          return 0;
        }
        temp = Serial1.read();
        if (temp == cmdName[i])
        {
          x = x + 1;
        }
        if (temp == 0x3C)
        {
          i = 0;
          x = 1;
          counter = counter + 1;
        }
      }
    }
    else
    {                                       // what is recieved was not "<"
      timeout = ((micros() - time) > 2000); // this part of code should not take more than 2 msec
      counter = counter + 1;
      Serial2.print("got ");
      Serial2.print((int)temp);
      Serial2.print("instead of ");
      Serial2.println(0x3C);
    }

    if (x == nBytes - 2)
    { // last byte is not equal to message
      Serial2.print("correct message");
      return 1; // succesful transmission and confimraiton
    }
  }
  //Serial2.write(x);
  return 0;
}

char Mia::write2(char cmdName[], int nBytes)
{

  // basic send command directly and get if Mia sends back confirmation only.

  int counter = 0;
  int maxCounter = 50; // I will give it nbytes +1 tries to to get the message
  char x = 0;
  char temp = 0;
  unsigned long time = micros() - 1; // overall time of the function
  int timeout = false;
  char bytex;
  Serial1.write(cmdName, nBytes);

  time = micros() - 10; //update time

  // wait for hand response to start (if not started yet)
  while (Serial1.available() == 0 && !timeout)
  {
    timeout = ((micros() - time) > 5000UL);
  }
  if (timeout)
  {
    Serial2.println("timeout waiting for hand response");
  }

  // skim through the stream looking for the first "<"
  while (!timeout && (counter < maxCounter))
  {

    // enforce wait for each byte
    while (Serial1.available() == 0 && !timeout)
    {
      timeout = ((micros() - time) > 100);
    }

    bytex = Serial1.read();
    // check first byte
    if (bytex == 0x3C)
    {
      counter = counter + 1;
      x = 1;
      // read the rest of response
      for (int i = 1; i < nBytes; i++)
      {
        while (Serial1.available() == 0 && !timeout)
        {
          timeout = ((micros() - time) > 50000); // 18 bytes should take about 180 Micros + 50Micros bytes for the first byte, we will give this 250 microsecond
        }
        if (timeout)
        {
          return 0;
        }

        bytex = Serial1.read();
        if (bytex == cmdName[i])
        {
          x = x + 1;
        }
        if (bytex == 0x3C)
        {
          i = 0;
          x = 1;
          counter = counter + 1;
        }
      }
    }
    else
    { 
      Serial2.print((int)bytex, HEX);                                      // what is recieved was not "<"
      timeout = ((micros() - time) > 2000); // this part of code should not take more than 2 msec
      counter = counter + 1;
    }
    // a flag is recieved correctly.
    if (x == nBytes - 2)
    {           // last byte is not equal to message
      return 1; // succesful transmission and confimraiton
    }
  }
  Serial2.print("Failed counter -> ");
  Serial2.print((int) counter);
  Serial2.print(" time out -> ");
  Serial2.print((int) timeout);
  Serial2.print (" ");
  Serial2.println(cmdName);
  return 0;
}

char Mia::fullCalibrate()
{
  // stop all movment if any
  // no stop all command yet [12-10-2021]
  // calibrate command
  char commandResp = 0;
  char dump = 0;
  commandResp = write2("@AK0000000000000*\r", 18);

  // hand keeps sendind data while it is calibrating ... use that to block the code and wait

  if (commandResp == 1)
  { // if mia got the calibration command
    while (Serial1.available() < 50)
    {
    }
  }else{
    Serial2.write( "not sent correctly.");
    delay(5000);
  }

  long time1 = micros();
  char wait = true;

  while (Serial1.available() > 0 || wait) // wait for Mia to finish calibration
  {                        
    dump = Serial1.read(); // no flush command from Arduino :(
    wait = (micros() - time1) < 1000;
  }

  bool handReady = false;
  
  while (!handReady)
  {
    commandResp = write2("@ADI100000000000*\r", 18);
    // get status of hand
    if (commandResp == 1)
    {
      // infinite for until startFlag sequence is detected
      int j = 0;
      char startFlag[5] = {0x73, 0x74, 0x61, 0x20, 0x3a};
      static unsigned char thsByte;

      // fill buffer first
      while (Serial1.available() < 50)
      {
      }

      wait = true;
      for (j = 0; j <= 4; j++)
      {
        // this part enforces that the program waits for the bytes
        if (Serial1.available() < 5 - j + 1)
        {
          time1 = micros() - 100;
          while (Serial1.available() < 50 && wait)
          {
            wait = ((micros() - time1) < 10000);
          }
        }

        thsByte = Serial1.read();
        if (thsByte != startFlag[j])
        {
          j = -1;
          // since it is wrong, check if it is the start of new message ?
          if (thsByte == startFlag[0])
          {
            j = 0;
          }
        }
      }
    }
    // read all the empty bytes
    char bfr[46] = {0};
    Serial1.readBytes(bfr,46);
    if (bfr[45] == 0x0A)
    {
      if (bfr[33] == 0x2B && bfr[34] == 0x30 && bfr[35] == 0x30)//*+00
      {           
        return 1; // calibration was succeful
      }
    }
  }
  return 0;
}

// void Mia::startStream(){
//   write("@ADA100000000000*\r",18);
// }

// void Mia::stopStream(){
//   write("@ADA000000000000*\r",18);
// }

// void Mia::read()
// {
//   /* read Mia byte:
//     1) checks if the serial buffer has the number of bytes of startFlag or more
//     2) checks the buffer, if the start flag is detected:
//         it reads the expected length of the message and endFlag
//           if endFlag is correct, it updates the values in MiaBytes with the new message.
//     errors handling:
//     1) if the start flag is not detected, it will continue reading the buffer until it finds it.
//     2) if the end flag is not detected, it will discard the message and escape the function.
//     3) this function waits for the start flag sequence to be detected. [stuck in for loop]
//   */
//   static unsigned char rcvMsg = false;
//   static unsigned char j = 0;

//   if (Serial1.available() == SERIAL_RX_BUFFER_SIZE)
//   {
//     digitalWrite(LED_BUILTIN, 1); // if you see the LED turned on, you have lost some data.
//   }

//   // if the buffer has the length of a start flag in it.
//   if (Serial1.available() >= _startFlagLength && rcvMsg == false)
//   {
//     // infinite for until startFlag sequence is detected
//     for (; j <= _startFlagLength - 1; j++)
//     {
//       static unsigned char thisByte;
//       thisByte = Serial1.read();
//       if (thisByte != _startFlag[j])
//       {
//         j = 0;
//         // since it is wrong, check if it is the start of new message ?
//         if (thisByte == _startFlag[0])
//         {
//           j = j + 1;
//         }
//         break;
//       }
//     }

//     // if the start flag was correct
//     if (j == _startFlagLength)
//     {
//       rcvMsg = true;
//       j = 0;
//     }
//   }

//   // recieved a correct start flag ... now read the rest and check endFlag
//   if (Serial1.available() > 0 && rcvMsg == true)
//   {

//     Serial1.write(Serial1.readBytes(_bfr, _totalLength - _startFlagLength));
//     // ^^ probably this is debugging stuff
//     for (j = 0; j < _totalLength - _startFlagLength; j++)
//     {
//       Serial1.write(_bfr[j]);
//     }
//     //to here

//     for (j = 0; j <= _endFlagLength - 1; j++)
//     {
//       if (_bfr[_lenMsg + j] != _endFlag[j])
//       {
//         j = 0;
//         rcvMsg = false;
//         break; // message is not correct
//       }
//     }
//     if (j == _endFlagLength)
//     {
//       //correct message!
//       for (unsigned char i = 0; i < (_lenMsg + _endFlagLength); i++)
//       {
//         MiaBytes[_startFlagLength + i] = _bfr[i];
//       }
//       rcvMsg = false; // message recieved correctly!
//       j = 0;
//     }
//   }

//   /* Since Mia sends string and not numbers
//   this code below reads the strings in MiaBytes and converts them into signed integers
//   it updates Forces[].
//   if the stream counter (sent from Mia) is the same it escapes and doesn't update Forces
//   */
//   // pkgNumber is the index of data in Forces array, Forces[pkgNumber].
//   static unsigned char pkgNumber;

//   for (unsigned char i = 0; i < 85; i++)
//   {
//     _MiaBytesCopy[i] = MiaBytes[i];
//   }

//   // check if this is a new package or already seen
//   int stream_count = 0;
//   for (int i = 79; i < 84; i++)
//   {
//     stream_count = stream_count + (_MiaBytesCopy[i] - 48) * multipliers[i - 79];
//   }
//   // if this is new data, stop all interrupts and update Forces
//   if (stream_count != Forces[8])
//   {
//     noInterrupts();
//     pkgNumber = 0;
//     for (unsigned char i = 6; i < _totalLength-1; i = i + 9)
//     {
//       Forces[pkgNumber] = 0;
//       // read 5 digits
//       for (unsigned char j = 1; j < 6; j++)
//       {
//         Forces[pkgNumber] = Forces[pkgNumber] + (_MiaBytesCopy[i + j] - 48) * multipliers[j - 1];
//         // substracting 48 converts a string "5" in asci to an integer of 5
//       }
//       // check the sign
//       if ('-' == _MiaBytesCopy[i])
//       {
//         Forces[pkgNumber] = -Forces[pkgNumber];
//       }
//       // update package number
//       pkgNumber = pkgNumber + 1;
//     }
//     interrupts();
//   }
// }
