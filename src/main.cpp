
#include <Arduino.h>

#include <Tactor.h>
//#include <Mia.h>
#include <FIFO.h>
#include <TRFB.h>
//#include <logger.h>
#include <sensorizedObj.h>

// for the button
const byte interruptPin = 2;
volatile byte state = LOW;
volatile int count = 0;
int count_old = 0;
long debouncing_time = 15; //Debouncing Time in Milliseconds

volatile unsigned long last_micros =0;
long timeFromLastIncrement = 0;

// for the LED
const byte RED = 9;
const byte YEL = 8;
const byte BLU = 7;

// for the Bush puttons
bool btnState[3] = {HIGH,HIGH,HIGH};
int  btnCount[3] = {0,0,0};
long btnMicros[3] = {0,0,0};

const int btnStp = 6;
const int btnFil = 5;
const int btnRpt  = 4; 
unsigned long btn_debouncing_time = 500;

void debounceInterrupt();
void debounceButton(unsigned int nPin);
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22, OUTPUT);
  digitalWrite(22,LOW);
  noInterrupts();
  // ISR
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A =  61; // OCR1A = 61 --> 1 kHz
  OCR1B  = 50;
  // pre-scalers
  TCCR1B |= (1<< CS11);
  TCNT1 =  45535;        //100 Hz interrupt
 
  TIMSK1 |= (1 << TOIE1); // Timeroverflow enable
  // = 16000000 / (1 * 1000) - 1 (must be <65536)
  // for 100 Hz


  // TCCR1B |= (1 << CS10);  
  // TCNT1 = 63936; // 10 kHz
  // TCNT1 = 65376; // 100 kHz
  // TCNT1 = 49536; // 1 kHz
  // TCNT1 = 655    // 100 Hz

  /* for button */
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), debounceInterrupt, CHANGE);

  /* LED */
  pinMode(RED, OUTPUT);
  pinMode(YEL, OUTPUT);
  pinMode(BLU, OUTPUT);

  /* buttons */
  pinMode(btnStp,INPUT_PULLUP);
  pinMode(btnRpt,INPUT_PULLUP);
  pinMode(btnFil,INPUT_PULLUP);
  interrupts();
}

void logData();
// start a tactor object
Tactor tactor;
// start sensorized object
sensorizedObj cube(A2, A3, A1, A0); // bottom left right FSR
// start TRFB method
TRFB FB;
//LOGGER logD;
// ISR

volatile bool newData = false;
volatile bool intFlag = false;
volatile char tcks = 0;

const long period = 20;
bool started = false;
long previousTime = 0;
long currentTime = 0;
unsigned int t = 0;
int pos = 0;
double summ[3] = {0};
byte u = LOW;
bool TimeOutErr = false;
bool stopStream = false;
ISR(TIMER1_OVF_vect)
{
  // EVERY 10 msec, if the interruptFlag is set while
  if(!intFlag) {
    intFlag = true;
  }
  else {
    TimeOutErr = started;
  }
  TCNT1 =  45535;
}

unsigned char debugCounter = 0;
unsigned long debugvar = 0;
char record = 0;
char err_flag = 0;
char move_robots = 0;
int counterHold = 0;
int success = 0;
bool strt = false;
int  deg  = 0x61;
char deg2 = 0x05;
unsigned int timr =0x00;
unsigned int pocketNum = 0; 
unsigned int cubeLimit = 170; 
unsigned char buf[3] = { 0,0, 0};
unsigned char Serialstart1 = false;  // true if start falg arrived in serial 1
unsigned char Group = 0;
int PosCmd = 0;
int FrsCmd = 500;
signed int err = 0;
bool open = false;


void loop()
{ 
  // start-up routines
  // pressing the button makes it start ..
  tactor.getStatus();
  delay(100);
  if(Serial1.available() == 0){
   Serial1.end(); 
  }else{
    while(Serial1.available()){
      debugvar = Serial1.read();
    }
  }

  while(Serial.read() != 's' &&  btnCount[btnStp-4] == 0)
  {
    digitalWrite(YEL,HIGH);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(YEL,LOW);
    delay(1000);
    debounceButton(btnStp);
  }
  
  if(!Serial1){
  Serial1.begin(115200);
  }
  digitalWrite(LED_BUILTIN,LOW);
  
  if(!strt)
  {
    // move the motor
    cube.biasCalculate(1, intFlag);
    tactor.Start();
    count = count_old;
    timeFromLastIncrement = millis();
    strt = 1;
  }
  
  // signal it is done ... 
  for ( int i = 0 ; i<4; i++){
    delay(250);
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(YEL,HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(YEL,LOW);
  }

  // reset interrupts
  TimeOutErr = false;
  intFlag = false;
  started = true;
  bool down = false;
  btnCount[btnStp] == 0;
  // flush out serial
  while(Serial1.available()){
    debugvar =  Serial1.read();
  }

  digitalWrite(LED_BUILTIN,HIGH);
  while(TimeOutErr == false){
    if(intFlag){

      digitalWrite(22,u); // for debugging
      u = !u;
      
      // routine for PID
      /*if(calPIDgaines){
        if(time < timeLimit){
          if(tactor.commandSent){
            // increment the timer
          }
        }
        else
        {
          if(tactor.hasArrived(true)){
            calPIDgains = false;
            tactor.setPosition(0x0000);
          } else {
            tryCounters++;
            if(tryCounter < 5){
            tactor.getPID();
            tactor.setPID( tactor.Kp*1.1, Ki, Kd,_error);
            time = 0;
            tactor.commandSent = false;
            }
          }
        }
        if(!tactor.commandSent){
        tactor.setPosition(0x9000);
        tactor.commandSent = true;
        }
      }
      */
      


      // scheme for experiment!
      if(count < 6 ){
        FB.fb_type = 1;
      }
      if(count >= 6 && count <=100){
        switch (Group)
        {
        case 1:
          FB.fb_type = 1;
          break;
        case 2:
          FB.fb_type = 5;
        case 3:
          FB.fb_type = 3;
        case 4:
          FB.fb_type = 2;
        default:
          FB.fb_type = 1;
          break;
        }
      }
      if(count > 100){
        FB.fb_type = 0;
      }
      //end of scheme

      cube.write2Fifo();
      // it was cub.fifoL.last()
      if(!(cube.fifoL.isEqual2(cube.fifoL.last()))){
        PosCmd = cube.fifoL.last();
      }

      err = (cube.fifoF.last() - FrsCmd)/2;
      tactor.setPWM((err>0),abs(err));
      // Serial.println(abs(err));
      //FB.trfb(cube.fifoL, cube.fifoR, cube.fifoT, PosCmd, tactor);
      

      if((cube.fifoL.last()>cubeLimit)){
        digitalWrite(RED,HIGH);
        err_flag = 1;
      }
      
      // tactor.getPosition(true); // waits for the response 
     
     if(count_old < count){
      count_old = count;
      timeFromLastIncrement = millis();
     }
     if(FB.state == 2 && counterHold < 100 ){
      counterHold++;
     }
     if(counterHold >= 100 ){
      digitalWrite(YEL,HIGH);
      counterHold = 0;
      success = 1;
     }

     if(success == 1 && FB.state == 0 && err_flag == 0){
      success = 2;
     }
     pocketNum++;

     // logger 
     //logData();
     //Serial.println(PosCmd);
     debounceButton(btnStp);
     debounceButton(btnFil);
     debounceButton(btnRpt);
     //buttons pull
     if(btnCount[btnStp-4]){
      record = !record;
      move_robots = record;
      stopStream = !record;
      btnCount[btnStp-4] = 0;
     }
     if(btnCount[btnRpt-4]){
      count--;
      count_old--;
      err_flag = 1; 
      btnCount[btnRpt-4] = 0;
     }
     if(btnCount[btnFil-4]){
      err_flag =1;
      digitalWrite(RED,HIGH);
      btnCount[btnFil -4] =0;
     }

     intFlag = false;
     
    }
    if(TimeOutErr){
      // reset!
      tactor.stop();
    }
  }

  //timeOutError! a cycle took longer than 10 msec
  Serial.println("<-- TimeOutErr-->");
  while(1){
  digitalWrite(LED_BUILTIN,LOW);
  delay(100);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(100);
  }
}


int sat2zero(signed int x){
  if(x<0){
    return(0);
  }
  else
  {
   return((int)x);
  }
}

void debounceInterrupt() {
 if((millis() - last_micros) > debouncing_time ) {
      if(state){ // from low -> High
      last_micros = millis();
      state = digitalRead(interruptPin);
      digitalWrite(BLU, LOW);
      record = 1;
      }
      else
      { // from high -> low
      if((millis()- timeFromLastIncrement)>1000 && move_robots){
      success = 0;
      err_flag =0;
      count++;
      record = 0;
      digitalWrite(RED, LOW);
      digitalWrite(YEL, LOW);
      digitalWrite(BLU, HIGH);
      } 
      last_micros = millis();
      state = digitalRead(interruptPin);
      }
  }
  else
  {
    state = digitalRead(interruptPin);
  }
}

void logData(){
  if(Serial.available()){
    switch (Serial.read())
    {
    case '1':
      // Global Start recording
      record = 1;
      move_robots = 1;
      break;
    case '2':
      // stop recording
      record = 0;
      break;
    case '3':
      // repeat this trial
      count--;
      count_old--;
      err_flag = 1;      // flag error 
      break;
    case '4':
      // stop robots
      move_robots = 0;
      record = 0;
      break;
    case '5':
      // toggle on/off streaming
      stopStream = !stopStream; 
    case '0':
      // allow the robots to move
      move_robots = 1;
      break;
    case 'R':
      count = 0;
      move_robots = 0;
      record = 0;
      break;
    case 'S':
      debugvar = Serial.read()-48;
      if(debugvar >= 0 && debugvar <= 5 ){
        FB.fb_type = debugvar;
      }
      break;
    case 'G':
      debugvar = Serial.read()-48;
      if(debugvar > 0 && debugvar <= 4){
      Group = debugvar;
      count = 0;
      }
    case 'k':
      if(Serial.read()-48 == 0){
      cubeLimit = cubeLimit-10;
      }
      else{
        cubeLimit = cubeLimit+10;
      }
          
  }
  }
  if(!stopStream){
  // general 
  Serial.print((int)move_robots);
  Serial.print("\t");
  Serial.print((int)record);
  Serial.print("\t");
  Serial.print((int)count);
  Serial.print("\t");
  // feedback information
  Serial.print(FB.fb_type);
  Serial.print("\t");
  Serial.print((unsigned int)tactor.position);
  Serial.print("\t");
  Serial.print((int)FB.state);
  Serial.print("\t");
  Serial.print((int)FB.vibrate);
  // forces
  Serial.print("\t");
  Serial.print(cube.fifoR.last());
  Serial.print("\t");
  Serial.print(cube.fifoL.last());
  Serial.print("\t");
  Serial.print(cube.fifoT.last());
  Serial.print("\t");
  //error
  Serial.print((int)err_flag);
  Serial.print("\t");
  Serial.print((int)success);
  Serial.print("\t");
  Serial.print(pocketNum);
  Serial.print("\n");
  }
}


void debounceButton(unsigned int nPin){
  if((millis() - btnMicros[nPin-4])> btn_debouncing_time ){
      if(!btnState[nPin-4] && digitalRead(nPin)){
        btnState[nPin-4] = true;
      }
      if(btnState[nPin-4] && !digitalRead(nPin) ){// high to low
        btnState[nPin-4] = digitalRead(nPin);
        btnCount[nPin-4]++;
        btnMicros[nPin-4] = millis();
      }
  }
}

bool checkCorrectStages(char stateVect){
// to-do.
}