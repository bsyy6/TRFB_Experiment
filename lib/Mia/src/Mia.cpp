// /*
//   Tactor.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
//   Created by Waleed Alghilan, 2021.
//   SSSA - AHA lab
// */

// #include "Arduino.h"
// #include "Mia.h"

// //constructor
// Mia::Mia()
// {
//   // Serial1 port connects to the Mia
//   //Serial1.begin(115200);
// }

// // methods

// // write to Mia a cutom command
// char Mia::write(char cmdName[], int nBytes)
// {
//   // basic send command directly and get if Mia sends back confirmation only.
//   char x = 0;
//   unsigned long time = micros() - 1; // overall time of the function
//   unsigned long timeLocal;
//   int timeout = false;
//   char thisByte;

//   Serial1.write(cmdName, nBytes);
//   time = micros() - 10; // time

//   // wait for hand response to start (if not started yet)
//   while (Serial1.available() == 0 && !timeout)
//   {
//     timeout = ((micros() - time) > 2000UL);
//     if (timeout)
//     {
//       Serial2.println("timeout waiting for hand response");
//       return 0;
//     }
//   }

//   timeLocal = micros() - 100;
//   // searching for response within timelimit
//   while (Serial1.read() != 0x3C && !timeout)
//   {
//     timeout = (micros() - timeLocal > 12000); // enough time for 115 bytes to check
//     if (timeout)
//     {
//       Serial2.println("no confirmation");
//       return 0;
//     }
//   }

//   timeLocal = micros() - 100;
//   //received first part, check the rest
//   while ((Serial1.available() < (nBytes - 2)) && !timeout)
//   {
//     timeout = ((micros() - timeLocal) > 2000);
//     // filling the buffer 2000 because I am not expecting more than 20 bytes.
//     if (timeout)
//     {
//       Serial2.print(Serial1.available());
//       Serial2.println(" no confimation 2");
//       return 0;
//     }
//   }

//   // read the rest of response
//   for (int i = 1; i < nBytes; i++)
//   {
//     thisByte = Serial1.read();
//     if (thisByte == cmdName[i])
//     {
//       x = x + 1;
//     }
//   }

//   // check if the flag is recieved correctly.
//   if (x == nBytes - 3) // expecting first charecter wrong and the last two too.
//   {
//     return 1; // succesful transmission and confimraiton
//   }
//   else
//   {
//     Serial2.print("x was : ");
//     Serial2.println((int)x);
//     return 0;
//   }
//   return 3;
// }

// char Mia::readMiaBuf(int lenMsg, unsigned char *Flag, int FlagCount, unsigned char *endFlag, int endFlagCount)
// {
//   // This function EXPECTS only <100 bytes messages
//   // the buffer buf is part of the Mia public variables.
//   // if
//   // This function looks for a flag in stream ( infinite loop until it finds it)
//   // exmple call in main, if you expect 52 bytes (including start and endflags):
//   // unsigned char flag[5] =  {0x61, 0x64, 0x63, 0x20, 0x3A, 0x20}; // start Flag
//   // unsigned char endFlag[1] = {0x0A} // end flag
//   // readMiaBuf (52, flag, 5, endFlag, 1)
//   // // now the buffer will update each time the funciton is called.
//   // // it returns a zero if timeout (10000 microseconds), 99 if failed, 1 if success.
  
//   bool handStopped = false;

//   int j = 0;
//   unsigned char thsByte;
//   static unsigned long time1;
//   static unsigned long time0;
//   int count = 0;
//   static char temp[100] = {0};
//   static char nBytes2read = 0;
//   // infinite loop until startFlag sequence is detected
//   time0 = micros() - 100;
//   for (j = 0; j < FlagCount; j++)
//   {
//     // this part enforces that the program to wait for the number of startFlag bytes
//     nBytes2read = FlagCount - j + 1;
//     if (Serial1.available() < nBytes2read)
//     {
//       time1 = micros() - 100;
//       while (Serial1.available() < nBytes2read && !handStopped)
//       {
//         handStopped = ((micros() - time1) > 10000);
//         if (handStopped)
//         {
//           //Serial2.print("first");
//         }
//       }
//     }

//     handStopped = ((micros() - time0) > 10000);
//     if (handStopped)
//     {
//       //Serial2.print("second");
//     }

//     thsByte = Serial1.read();

//     if (thsByte != Flag[j] && !handStopped)
//     {
//       j = -1;
//       // since it is wrong, check if it is the start of new message ?
//       if (thsByte == Flag[0])
//       {
//         j = 0;
//       }
//     }
//     if (handStopped)
//     {
//       j = FlagCount;
//     }
//   }

//   // if it escapees this for, it must have recieved a correct message or timed out
//   if (handStopped)
//   { // if timed out, return 0
//     for (int i = 0; i < lenBuf; i++)
//     {
//       buf[i] = 0;
//     }
//     //Serial2.print("1hs"); // 1 - hand stopped
//     return 0;
//   }
  
  
//   // zero the buffer
//   for (int i = 0; i < lenBuf; i++)
//   {
//     buf[i] =0;
//   }
//   // read all the bytes and put them in buf.
//   count = 0;
//   while (Serial1.available() < lenMsg - FlagCount - 10)
//   {
//     /*handStopped = ((micros() - time0) > 10000);
//     if (handStopped)
//     {
//       //Serial2.print("2hs"); // 2 - hand stopped
//       return 0;
//     }*/
//     // waits for the bytes to arrive
//   }
//   Serial1.readBytes(temp, lenMsg - FlagCount);

//   for (int i = (lenMsg - FlagCount - 1); i > (lenMsg - FlagCount - endFlagCount - 1); i--)
//   {
//     if (temp[i] == endFlag[endFlagCount - lenMsg + FlagCount + i])
//     {
//       count++;
//     }
//     else
//     {
//       //Serial2.print("1bf"); // 1 - buffer endflag
//       return 0;
//     }
//   }

//   if (count == endFlagCount)
//   {
//     for (int i = 0; i < lenMsg; i++)
//     {
//       if (i < FlagCount)
//       {
//         buf[i] = Flag[i]; // fill in the start Flag
//       }
//       else
//       {
//         buf[i] = temp[i - FlagCount]; // fill in the rest in buffer
//       }
//     }
//     return 1;
//   }
//   Serial2.println("criticalerror");
//   return 99; // if you see 99, RUN!
 
// }

// char Mia::readMiaForces()
// {
  
//   unsigned char startFlag[6] = {0x61, 0x64, 0x63, 0x20, 0x3a, 0x20}; // 'adc : '
//   unsigned char endFlag[1] = {0x0A};
//   if (readMiaBuf(85, startFlag, 6, endFlag, 1) == 1)
//   {
    
//     stream_count = 0;
//     for (int i = 79; i < 84; i++)
//     {
//       stream_count = stream_count + (buf[i] - 48) * multipliers[i - 79];
//     }

//     // if this is new data, stop all interrupts and update Forces
//     if (stream_count != stream_count_old)
//     {
//       noInterrupts();
//       char pkgNumber = 0;
//       if (processed == 1)
//       {
//         for (unsigned char i = 6; i < 85 - 1; i = i + 9)
//         {
//           Forces[pkgNumber] = 0;
//           // read 5 digits
//           for (unsigned char j = 1; j < 6; j++)
//           {
//             Forces[pkgNumber] = Forces[pkgNumber] + (buf[i + j] - 48) * multipliers[j - 1];
//             // substracting 48 converts a string "5" in asci to an integer of 5
//           }
//           // check the sign
//           if ('-' == buf[i])
//           {
//             Forces[pkgNumber] = -Forces[pkgNumber];
//           }

//           // update package number
//           pkgNumber = pkgNumber + 1;
//         }
//         processed = 0;
//         Forces[8] = Forces[8] % 1000;
//       }
//       interrupts();
//       stream_count_old = stream_count;
//       return 1;
//     }
//     else
//     {
//       return 0;
//     }
//   }
//   else
//   {
//     return 0;
//   }
// }

// char Mia::fullCalibrate()
// {
//   // stop all movment if any
//   // no stop all command yet [12-10-2021]
//   // calibrate command
//   char commandResp = 0;
//   char temp;
//   commandResp = write((char *)"@AK0000000000000*\r", 18);
//   // hand keeps sending data while it is calibrating ... use that to block the code and wait
//   unsigned char startFlag[5] = {0x73, 0x74, 0x61, 0x20, 0x3a}; // "sta :"
//   unsigned char endFlag[1] = {0x0A};
//   int startFlagCount = 5;
//   char bufIsUpdated = 1;
//   Serial2.print("here19");
//   while (bufIsUpdated)
//   {
//     bufIsUpdated = readMiaBuf(52, startFlag, startFlagCount, endFlag, 1);
//     if (buf[33] == 0x2D) //* "-" sign
//     {
//       Serial2.print("interrupted! seen a negative");
//       return 0; // calibration was interrupted
//     }
//     delay(10);
//   }
//   Serial2.print("here20");

//   while (Serial1.available() != 0)
//   {
//     temp = Serial1.read();
//     if (Serial1.available() == 0)
//     {
//       delay(100);
//     }
//   }
//   // it will then stop sending data ... because why not.
//   commandResp = write((char *)"@ADI100000000000*\r", 18);
//   Serial2.write((int)commandResp);
//   if (commandResp == 1)
//   {
//     bufIsUpdated = readMiaBuf(52, startFlag, startFlagCount, endFlag, 1);
//     if (bufIsUpdated && buf[34] == 0x2B && buf[35] == 0x30)
//     { //*+00
//       commandResp = 0;
//       commandResp = write((char *)"@ADI000000000000*\r", 18);
//       if (commandResp)
//       {
//         return 1; // completed succefully
//       }
//     }
//   }
//   Serial2.print("here!");
//   return 99;
// }

// /*
// char Mia::readMiaBuf2(int lenMsg, unsigned char *Flag, int FlagCount, unsigned char *endFlag, int endFlagCount)
// {
// if(Serial1.available>FlagCount){
//   while(!firstByteFound){
//    firstByteFound = (Flag[0]==Serial1.read());

//   }
//   }
// }*/

// char Mia::fullCalibrateAndBias()
// {
//   char response = 0;
//   response = 1; //startStream(); // calibrate manually.
//   Serial.print((int)response);
//   if (response == 1)
//   {
//     Serial.print("started bias process");
//     long ForcesSum[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//     // bias all sensors and save the values[]
//     for (int i = 0; i < 100; i++)
//     {
//       Serial.print((int)processed);
//       Serial.print("->");
//       if (processed == 1)
//       {
//         // read 100 data points
//         readMiaForces();
//         if (Forces[3] != 0)
//         {
//           for (int j = 0; j < 8; j++)
//           {
//             ForcesSum[j] = ForcesSum[j] + (long)Forces[j];
//           }
//           processed = 1;
//         }
//         else
//         {
//           i = i - 1;
//         }
//       }
//       else
//       {
//         i = i - 1;
//       }
//       //debugging
//       Serial.print(i);
//       Serial.print("->");
//       Serial.println((int)Forces[3]);
//     }

//     for (int j = 0; j < 8; j++)
//     {
//       BiasVector[j] = (int)(ForcesSum[j] / 100);
//     }
//   }
//   else
//   {
//     Serial2.write("Calibration error");
//     return 0;
//   }
//   return 1;
// }

// void Mia::echo()
// {
//   Serial1.readBytes(buf, 100);
//   // debugging
//   for (int i = 0; i < 100; i++)
//   {
//     Serial.write(buf[i]);
//   }
// }

// char Mia::readMiaForcesBiased()
// {
//   if (!Biased)
//   {
//     Biased = fullCalibrateAndBias();
//   }
  
//   readMiaForces();
//   if (Biased && !processed)
//   { // && !Forces[processed]
//     for (int i = 0; i < 8; i++)
//     {
//       ForcesBiased[i] = ((int)Forces[i] - (int)BiasVector[i]);
//       ForcesBiased[i] = abs(ForcesBiased[i]);
//     }
//     return (1);
//   }
//   else
//   {
//     return (0);
//   }
// }

// void Mia::setState()
// {
//   // to-do
//   oldState = state;
//   if (ForcesBiased[indxN] < 5 || ForcesBiased[thmbN] < 5)
//   {
//     state = 0; // dropped nothing to see
//   }
//   if (ForcesBiased[indxN] > 5 || ForcesBiased[thmbN] > 5)
//   {
//     state = 1;
//     if (ForcesBiased[indxT] > 35)
//     {
//       state = 2;
//     }
//   }
//   stateChanged = (oldState != state);
// }

// char Mia::startStream()
// {
//   return (write((char *)"@ADA100000000000*\r", 18));
// }

// char Mia::stopStream()
// {
//   return (write((char *)"@ADA000000000000*\r", 18));
// }

// char Mia::scale2byte(int value, int max, int min)
// {
//   // maps a range of numbers (max - min ) to (255-0) instead.
//   // Saturates if less or more to 0 or 255.
//   if (max - min >= 255)
//   {
//     if (value < min)
//     {
//       return 0;
//     }
//     if (value > max)
//     {
//       return 255;
//     }
//     return ((value - min)*(255/(max - min)));
//   }
//   else
//   {
//     return -1;
//   }
// }
