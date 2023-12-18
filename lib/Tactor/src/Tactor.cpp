/*
  Tactor.cpp - Library for communicating with the Spingatore (Celemente et. al , 2015).
  Created by Waleed Alghilan, 2021.
  SSSA - AHA lab
*/

#include "Arduino.h"
#include "Tactor.h"

#define opened  24
#define closed  28
//constructor
Tactor::Tactor()
{
  // Which Serial1 port connects to the tactors
  Serial1.begin(115200);

}

// methods

// write to tactors cutom command
void Tactor::write(char cmdName[], char nBytes)
{
  for (int i = 0; i < nBytes; i++)
  {
    Serial1.write(cmdName[i]);
  }
}

// stops motor
void Tactor::stop()
{
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x71);
  Serial1.write(_board);
}

void Tactor::vibrate(unsigned char N, unsigned char dur)
{
  if(dur>63) dur = 63;
  if(N>4) N =4;
  Serial1.write(0x5f);
  Serial1.write(_board);
  Serial1.write(0x80);  // fixed
  Serial1.write((N<<6)|dur);  // length in msec/ 10 + 64 
  Serial1.write(_board); 
}

void Tactor::getStatus(bool wait)
{
  // wait is by default false.
  // wait forcees the program to wait for the status byte.
  while(Serial1.available()){
    status = Serial1.read();
  }
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x70);
  Serial1.write(_board);
  
  if(wait)
  {  
    while(Serial1.available()==0){
      // wait for data on Serial1.
    }
    status = Serial1.read();
    if((status&12) != 12){ // ignore status when both FCA and FCB are active 
      //statusChanged = ((status&8)!=(prevStatus&8) || (status&4) != (prevStatus&4));
      //prevStatus = status;
    }
  }else{
    status = Serial1.read();
  }
}

void Tactor::setPosition(unsigned int Pos)
{
  if(Pos>=256) Pos = 255; // saturation!
  

  pos_temp = _STROKES[Pos];
  // pos_temp = Pos; // for debugging
  // grab pos from lookup table
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x21); // we never really get the 17th byte thing in this, so this is the standarad message to set the tactor position.

  // why this? Because of undefined behaviour in case the integer has is less than 255 and you shift it write by 8 bits.
  if (pos_temp> 255)
  {
    Serial1.write((pos_temp >> 8) & 0xFF); // MSB
    Serial1.write(pos_temp & 0xFF);        // LSB
  }
  else
  {
    Serial1.write((byte) 0x00);             // MSB
    Serial1.write(pos_temp & 0xFF); //LSB
  }
  Serial1.write(_board);
}

void Tactor::setPosition_benchTest(unsigned int Pos)
{
  // this takes encoder positions instead of 0-255
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x21); // we never really get the 17th byte thing in this, so this is the standarad message to set the tactor position.

  // why this? Because of undefined behaviour in case the integer has is less than 255 and you shift it write by 8 bits.
  if (Pos> 255)
  {
    Serial1.write((Pos >> 8) & 0xFF); // MSB
    Serial1.write(Pos & 0xFF);        // LSB
  }
  else
  {
    Serial1.write((byte) 0x00);             // MSB
    Serial1.write(Pos & 0xFF); //LSB
  }
  Serial1.write(_board);
}

void Tactor::getPosition(bool wait){
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x22);
  Serial1.write(_board);
  if (wait)
  {
  while(Serial1.available() < 3){}
  static unsigned char buf[3] = { 0, 0, 0};
  Serial1.readBytes(buf,3);
  position = (unsigned int)
    ((    (unsigned long) buf[0] << 16
        | (unsigned long) buf[1] << 8
        | (unsigned long) buf[2]      ));
  }
}

void Tactor::setPWM(bool opn, unsigned int PWM){
  // tactor saturates at PWM = 500 , opn controls CW o CCW 
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x74);
  if(PWM>500) PWM =500;
            // open or close    // the PWM
  static unsigned int cmd  = 0;
  cmd = ((opn<<15)&0x8000)|(PWM & 0x1FF);
  Serial1.write((cmd&0xFF00)>>8);
  Serial1.write((cmd&0xFF));
  Serial1.write(0x01);
}

void Tactor::PWM_MOTORS(bool opn,bool wait){
 // Serial1.write(0x01);
  Serial1.write(0x5F);
  Serial1.write(_board);
  Serial1.write(0x74);

  if(opn){
    // true is open so send a command to pull back the tactor 
    Serial1.write(128); //HB 1000-0001
    Serial1.write(150); //LB 1111-1111
  }else{
    Serial1.write((byte) 0x00);   //HB 0000-0001
    Serial1.write(150); //LB 1111-1111
  }
  Serial1.write(_board);

  if(wait){
    // if the second argument is true it waits for stop signal from the hall sensors. (default) 
    getStatus(true);
    
    while(!statusChanged){
        getStatus(true);
    }
    stop();
  }
}

void Tactor::Calibrate(){
  getStatus();
  while(!Serial1.available())
  {
    // forced wait for status byte
  }
  status = Serial1.read();

  PWM_MOTORS((status&8) == 8); // send a command to open if closed or close if opened.
  while(!(status == opened)) //when it reads open!
  {
    getStatus();
    while(!Serial1.available())
    {
    }
    status = Serial1.read();
  }
  stop();
  getStatus();
  while(!Serial1.available())
  {
  }
  status = Serial1.read();
  if(status == 4){
    PWM_MOTORS(false);
  }
  while(!(status == closed)) //when it reads open!
  {
    getStatus();
    while(!Serial1.available())
    {
    }
    status = Serial1.read();
  }
  stop();
  getStatus();
  if(status == 8)
  {
  unsigned char buf[3]= {0,0,0};
  getPosition();
  while(Serial1.available() < 3){}
  Serial1.readBytes(buf,3);
  bias = (unsigned int)
    ((    (unsigned long) buf[0] << 16
        | (unsigned long) buf[1] << 8
        | (unsigned long) buf[2]      )/2);
  }
}


void Tactor::Test1(){
  getStatus(true);
  
  PWM_MOTORS((status&8) != 8); // if it is open it will close and vice versa

  PWM_MOTORS((status&8) == 8);

  stop();
  // now we are sure it is in starting place...
  delay(1000); // wait 1 second
  getPosition(true);
  for( char i = 0; i < 10; i++)
  {
    PWM_MOTORS(true);
    getPosition(true);
    PWM_MOTORS(false);
    getPosition(true);
  }
}

void Tactor::Test2(){
  // stiarcase test
  getStatus(true);
  unsigned int posi = 0;
  for (int i = 0; i <11 ;i++){
    posi=i*4096;
    setPosition(posi);
    delay(1000);
    getPosition(true);
    //read from force senor;#
    // send data through serial!
  }
}

void Tactor::Test3(){
   getStatus(true);
  unsigned int posi = 0;
  for (int i = 0; i <12 ;i++){
    posi=i*4096;
    setPosition(posi);
    delay(1000);
    getPosition(true);
    //read from force senor;#
    // send data through serial!
  }

}

/*
void Tactor::Test2(){
  getStatus(true);
  
}
*/
void Tactor::statusREADER(){
  String stringa = " ";

  stringa = status_str[int(((status&224)>>5))];             // MODE
           
  stringa = stringa + status_str[int(((status&16) >>4))+ 8] // contorl ok
            + status_str[int(((status&8 ) >>3))+10]         // FCA
            + status_str[int(((status&4 ) >>2))+12]         // FCB
            + status_str[int(((status&2 ) >>1))+14]         // current
            + status_str[int(((status&1 )    ))+16];        //  moving ?
  //println(stringa);
}

void Tactor::Start(){
  //stop();
  // The command to set the position to zero
  Serial1.write(0x5f);
  Serial1.write(0x01);
  Serial1.write(0x23);
  Serial1.write(0x01); 
  delay(1);
  // calibrate command
  Serial1.write(0x5f);
  Serial1.write(0x01);
  Serial1.write((byte) 0x00 );
  Serial1.write(0x01); 
  // getStatus(false); 
  // if(status == 0){
  //   vibrate(4,60);
  //   delay(500);
  //   vibrate(4,60);
  // }else{
  //   vibrate(4,60);
  // }
}

char Tactor::setPID(char _Kp, char _Ki, char _Kd, char _err){
  stop();
  Serial1.write(0x5f);
  Serial1.write(0x01);
  Serial1.write(0x24);
  Serial1.write(_Kp);
  Serial1.write(_Ki);
  Serial1.write(_Kd);
  Serial1.write(_err);
  Serial1.write(0x01);
  while(!Serial1.available()){

  }
  return(Serial1.read() == 0xAA);
}

void Tactor::getPID(){
  // gets the PID values from the spingatore
  // flush
  stop();
  while(Serial1.available()){
    kp = Serial1.read();
  }
  Serial1.write(0x5f);
  Serial1.write(0x01);
  Serial1.write(0x25);
  Serial1.write(0x01);
  while(Serial1.available() < 4){
  }
  kp = Serial1.read();
  ki = Serial1.read();
  kd = Serial1.read();
  er = Serial1.read();
}

void Tactor::dumpPID(){
  // prints out the PID values to serial0, call getPID() first!
  Serial.print((unsigned int)kp);
  Serial.print("\t");
  Serial.print((unsigned int)ki);
  Serial.print("\t");
  Serial.print((unsigned int)kd);
  Serial.print("\t");
  Serial.print((unsigned int)er);
}