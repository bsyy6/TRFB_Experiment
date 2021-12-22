/* expected behaviour:

1- internal LED flashing light
2- sending through serial0 [ 0x01,0x80,0x4A,0x01 ]
3- if the tactor board is connected, vibrate every 1 second.
*/
#include <arduino.h>
#include <Tactor.h>
#include <Mia.h>
#include <FIFO.h>
#include <TRFB.h>

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  // Serial2 is used for debugging.
  Serial2.begin(115200);
  Serial.begin(115200);
  //Serial2.begin(115200);
  /*
noInter rupts();
// ISR 
TCCR1A = 0; 
TCCR1B = 0; 
//OCR1A =   61;// OCR1A = 61 --> 1 kHz
//OCR1B  = 50;
// pre- scalers
TCCR1B  |= (1 << CS10);    // 1 PRESCALE
TIMSK 1 |= (1 << TOIE1); // Timeroverflow enable
TCNT 1 = 49536;
int errupts();
*/
}

// star  t a tactor object
Tactor tactor;
// st art a FIFO object
//FIFO  fifo;
//  start Mia object
Mia mia;
// start

// ISR
ISR(TIMER1_OVF_vect)
{ // EVERY 10 msec
  /*  if (TRFB.state == 2){
    if (t <= fadeTime){
    t += 1;  
    } 
    else
    { 
    t  = fadeTime;
    } 
  }else{
     t = 0;
  } 
  TCNT1 = 49536;*/
}

/*
#define CMDBUFFER_SIZE 32

char processCharInput(char *cmdBuffer, const char c)
{
  //Store the character in the input buffer
  if (c >= 32 && c <= 126) //Ignore control characters and special ascii characters
  {
    if (strlen(cmdBuffer) < CMDBUFFER_SIZE)
    {
      strncat(cmdBuffer, &c, 1); //Add it to the buffer
    }
    else
    {
      return '\n';
    }
  }
  else if ((c == 8 || c == 127) && cmdBuffer[0] != 0) //Backspace
  {

    cmdBuffer[strlen(cmdBuffer) - 1] = 0;
  }
  return c;
}

void serialEvent()
{
  static char cmdBuffer[CMDBUFFER_SIZE] = "";
  char c;
  while (Serial.available())
  {
    c = processCharInput(cmdBuffer, Serial2.read());
    Serial2.print(c);
    if (c == '\n')
    {
      Serial.println();
      //Full command received. Do your stuff here!
      if (strcmp("test", cmdBuffer) == 0)
      {
        Serial.println("\ntest works!");
      }
      if(strcmp("state",cmdBuffer) == 0){
        Serial.println(mia.state);
      }
      if(strcmp("biased",cmdBuffer) == 0){
        for (int i = 0; i < 9; i++)
        {
          Serial.print(mia.ForcesBiased[i]);
          Serial.print("\t");
        }
        Serial.println("");
      }
      cmdBuffer[0] = 0;
    }
  }
  delay(1000);
}*/

bool x2 = 0;
char y = 0;
char result = 0;
int var = 0xF0FF;

unsigned char startFlag[6] = {0x61, 0x64, 0x63, 0x20, 0x3a, 0x20}; // 'adc : '
unsigned char endFlag[1] = {0x0A};
char result2 = 0;
int resulttt[2];

void loop()
{
  if (mia.startStream() == 1)
  {
    Serial.println("started");
    mia.Biased = mia.fullCalibrateAndBias();
    result2 = 1;
  }

  if (mia.readMiaForcesBiased())
  {
    mia.setState();
    //Serial.println((int)mia.state);
    if (mia.stateChanged)
    {
      /*for (int i = 0; i < 9; i++)
        {
          Serial2.print(mia.Forces[i]);
          Serial2.print("\t");
        }
        Serial2.println("");
        for (int i = 0; i < 9; i++)
        {
          Serial2.print(mia.BiasVector[i]);
          Serial2.print("\t");
        }
        Serial2.println("");
        /*
       for (int i = 0; i < 9; i++)
        {
          Serial.print(mia.ForcesBiased[i]);
          Serial.print("\t");
        }*/
    }

    Serial.print(mia.ForcesBiased[1]);
    Serial.print("\t");
    Serial.print(mia.ForcesBiased[2]);
    Serial.print("\t");
    Serial.println((int)mia.state);
    mia.processed = 1;
  }
}

//mia.read();
// to-do debug mia communication. -> done.
// to-do libraray to save data. -> online.
// to-do talk with workshop downstairs. -> done.
