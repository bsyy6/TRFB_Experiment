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
//Tactor  tactor;
// st art a FIFO object
//FIFO  fifo;
//  start Mia object
Mia mia;
// start
 
// ISR 
ISR(TIMER1_OVF_vect) { // EVERY 10 msec
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

bool x =0;
bool y =0;
char result =0;
void loop(){
  Serial2.println("started!");
  result = mia.fullCalibrate();
  Serial2.print("main fullcalibrate command response:   ");
  Serial2.print((int) result);
  if (result ==1){
    digitalWrite(LED_BUILTIN,HIGH);
  }
  delay(30000);
}
    //mia.read();
    // to-do debug mia communication.
    // to-do libraray to save data.
    // to-do talk with workshop downstairs.
