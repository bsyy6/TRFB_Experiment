
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
volatile unsigned int count = 0;
int count_old = 0;
long debouncing_time = 15; //Debouncing Time in Milliseconds

volatile unsigned long last_micros =0;
long timeFromLastIncrement = 0;

// for the LED
const byte RED = 9;
const byte WHT = 8;
const byte GRN = 7;

// for the Bush puttons
bool btnState[3] = {HIGH,HIGH,HIGH};
int  btnCount[3] = {0,0,0};
long btnMicros[3] = {0,0,0};

const int btnStp = 6;
const int btnFil = 5;
const int btnRpt  = 4; 
unsigned long btn_debouncing_time = 500;
bool cmdSent = false;

bool startBench = false; //used this to bench-test the device.
int startBench_i = 0;
unsigned int sine_arrayLength = 101;
bool writingToMemory = false;

void debounceInterrupt();
void debounceButton(unsigned int nPin);
bool checkIfPause(void);

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
  // = 16000000 / (1 * 1000) - 1 (must be <65536)
  // for 100 Hz
 
  TIMSK1 |= (1 << TOIE1); // Timeroverflow enable

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
  pinMode(WHT, OUTPUT);
  pinMode(GRN, OUTPUT);

  /* buttons */
  pinMode(btnStp,INPUT_PULLUP);
  pinMode(btnRpt,INPUT_PULLUP);
  pinMode(btnFil,INPUT_PULLUP);
  interrupts();
}


// start a tactor object
Tactor tactor;
// start sensorized object
sensorizedObj cube(A2, A3, A1, A0); // bottom left right FSR
// start TRFB method
TRFB FB;


volatile bool intFlag = false;
bool started = false;
bool failed = false;
byte u = LOW;
bool TimeOutErr = false;
bool stopStream = false;

unsigned long tempVar = 0;
char record = 0;
char err_flag = 0;
char move_robots = 0;
int counterHold = 0;
int success = 0;
bool strt = false;

unsigned int pocketNum = 0; 
unsigned int cubeLimit = 150; // was 190 

unsigned char Group = 1;
volatile unsigned int PosCmd = 0; // this is the output from TRFB. between 0-255
volatile unsigned int PosCmd2 = 0;// I map it here to other range instead. example 0 - 100

signed int err = 0;

bool manual = false;
bool pull_up = false;
bool manualFB = true;
bool aPause = false;

unsigned char x = 10;
unsigned char old_x = 10;

// unsigned int nExp [2][5] = {{30,230,330,430,455}, {2, 7, 10, 30, 35}};
// unsigned int nExp_test[5] = {30,230,330,430,455};
unsigned int nExp [2][3] = {{3,103,153}, {2, 7, 10}}; // accomdation -> train -> test 
unsigned int nExp_test[3] = {3,103,153};

void change_nExp(unsigned int t){
  for (int i = 0; i<3; i++){
     nExp_test[i] =nExp[t][i]; 
  }
}

void logData();

ISR(TIMER1_OVF_vect)
{
  // EVERY 10 msec, if the interruptFlag is set while
  if(!intFlag) {
    intFlag = true;
  }
  else {
    TimeOutErr = true; // it was started
  }
  TCNT1 =  45535;
}

void loop()
{ 
  // start-up routines
  // pressing the button makes it start ..

  /* check if the tactor board is live, if not turn off the serial */
  tactor.getStatus(false);
  delay(100);
  if(Serial1.available() == 0){
    Serial1.end(); 
  }else{
    while(Serial1.available()){
      tempVar = Serial1.read();
    }
  }


  // start up button or s
  while(Serial.read() != 's' &&  btnCount[btnStp-4] == 0)
  {
    digitalWrite(WHT,HIGH);
    digitalWrite(RED,HIGH);
    digitalWrite(GRN,HIGH);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(WHT,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(GRN,LOW);
    delay(1000);
    debounceButton(btnStp);
  }
  
  
  Serial1.begin(115200);
  
  digitalWrite(LED_BUILTIN,LOW);

  // start-up routine
  if(!strt)
  {
    cube.biasCalculate(1, intFlag); //bias the force sensor 
    //tactor.Start(); // pulls the motor down    
    tactor.getStatus(false); // was false
    count = count_old;
    timeFromLastIncrement = millis();
    strt = 1;
  }
  
  // signal it is done with lights
  for ( int i = 0 ; i<4; i++){
    delay(250);
    digitalWrite(WHT,HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(WHT,LOW);
    digitalWrite(GRN,LOW);
    digitalWrite(WHT,LOW);
  }

  // reset interrupts
  TimeOutErr = false;
  intFlag = false;
  started = true;
  bool down = false;
  btnCount[btnStp] = 0;

  // flush out serial
  while(Serial1.available()){
    tempVar =  Serial1.read();
  }

  digitalWrite(LED_BUILTIN,HIGH);

  // this is the while(1)
  while(TimeOutErr == false){
    if(intFlag){

      digitalWrite(22,u); // for debugging
      u = !u;
      
      if(checkIfPause() && !aPause && move_robots){
       for ( int i = 0 ; i<4; i++){
        delay(250);
        digitalWrite(WHT,HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN,LOW);
        digitalWrite(WHT,LOW);
        digitalWrite(GRN,LOW);
        digitalWrite(WHT,LOW);
        }
        aPause = true;
        move_robots = false;
        record = 0;
        TimeOutErr = false;
        intFlag = false;
      }

      // [2] read force from sensorized object
      cube.write2Fifo();
      // [3] set what is the command sent 
      PosCmd = cube.fifoL.isEqual3(cube.fifoL.last()); // jumps to zero if really small forces are in
      if(PosCmd>=160){ 
        PosCmd2 = 255;
      }else{
        PosCmd2 = (PosCmd*(unsigned int) 255)/(unsigned int)160;
      }

      // [3] move motor
      if(!manual && !startBench){
       FB.trfb(cube.fifoR, cube.fifoL, cube.fifoT, PosCmd2, tactor);
      }else{
        if(!cmdSent){
          if(pull_up){
            tactor.setPosition(255); // pull it down.
            cmdSent = true;
          }else{
            tactor.setPosition(50); // push it pull_up.
            cmdSent = true;
          }
        }
      }

      if(startBench){
        
        if(startBench_i == 0){
          tactor.setPosition_benchTest(tactor._Sine[0]);
        }else{
          tactor.setPosition_benchTest(tactor._Sine[startBench_i]);
        }
        startBench_i++;
        if(startBench_i>100){
          startBench = false;
          startBench_i = 0;
        }
      }
      // force control idea ...  ( not used now 12/10/2022)
      //err = (cube.fifoF.last() - FrsCmd)/2;
      //tactor.setPWM((err>0),abs(err));      
      
      // [4] red for broken box
      if((cube.fifoL.last()>cubeLimit)){
        if(!failed){
        digitalWrite(RED,HIGH);
        digitalWrite(GRN,LOW);
        digitalWrite(WHT,LOW);
        failed = true;
        counterHold = 0;
        err_flag = 1;
        }
      }
      else
      {
        if(FB.state == 2 && counterHold < 300 && !failed  && !success){
          counterHold++;
        }
        if(counterHold >= 300 && !failed && !success ){
          digitalWrite(WHT,HIGH);
          counterHold = 0;
          success = 1;
        }
      }
      
      
     tactor.getPosition(true); // waits for the response 
     

     if(count_old < count){
      count_old = count;
      timeFromLastIncrement = millis();
     }
     
     if(success == 1 && FB.state == 0 && err_flag == 0){
      success = 2;
     }
     
     pocketNum++;

     // logger 
     if(!startBench){
      logData();
     }else{
      Serial.write(0xFF);
      Serial.write(((unsigned int)tactor.position>> 8) & 0xff);
      Serial.write((unsigned int)tactor.position & 0xff);
      Serial.write(((unsigned int)tactor._Sine[startBench_i-1]>> 8) & 0xff);
      Serial.write((unsigned int)tactor._Sine[startBench_i-1] & 0xff);
     }

     intFlag = false;
     

     if(writingToMemory){
      // Read data into the array
        for (unsigned int i = 0; i < sine_arrayLength; i++) {
          // Read 2 bytes at a time as uint16_t
          uint8_t buffer[2];
          Serial.readBytes(buffer, 2);
          tactor._Sine[i] = uint16_t(buffer[1]) | uint16_t(buffer[0]) << 8;
        } 
        TimeOutErr = false;
        intFlag = false;
        writingToMemory = false;
     }


    }

    if(TimeOutErr){
      tactor.stop();
    }
  
  }

  //timeOutError! a cycle took longer than 10 msec // blocks everything
  Serial.println("<-- TimeOutErr-->");
  while(1){
  digitalWrite(LED_BUILTIN,LOW);
  delay(100);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(100);
  }
}

// communication loggin data
void logData(){
  if(Serial.available()){
    switch (Serial.read())
    {
    case '1':
      // stop//start robots
      move_robots = !move_robots;
      break;
    case '2':
      // stop/start recording
      record = !record;
      break;
    case '3':
      // repeat this trial
      record = 1;
      count--;
      count_old--;
      err_flag = 3;      // flag error 
      break;
    case '5':
      // toggle on/off streaming
      stopStream = !stopStream;
      break;
    // testing system
    case '0': // start manual
      manual = !manual;
      pull_up = false;
      break;
    case '9': // go up or down, manual must be set first.
      pull_up = !pull_up;
      cmdSent = false;
      break;
    case 'R':
      count = 0;
      move_robots = 0;
      record = 0;
      break;
    case 'S':
      tempVar = Serial.read()-48;
      if(tempVar >= 0 && tempVar <= 5 ){
        FB.fb_type = tempVar;
        manualFB = true;
      }
      if (tempVar == 6){
        manualFB = false;
      }
      break;
    case 'G':
      // sets Group.
      tempVar = Serial.read()-48;
      if(tempVar > 0 && tempVar <= 4){
      Group = (char) tempVar;
      count = 0;
      }
      break;
    case 'M':
      // sets experimental setup.
      tempVar = Serial.read()-48;
      if(tempVar > 0 && tempVar <= 4){      
      change_nExp((int) tempVar -1);
      count = 0;
      }
      break;
    case 'C':
      {
      // manually sets the count.
      unsigned char i = 0;
      tempVar=0;
      char nChar [3] = {0,0,0};
      
      while(Serial.available()){
       nChar[i]= Serial.read();
       i++;
      }
      count = atoi(nChar);
      }
      break;
    case 'Z':
      // resets the tactor
      tactor.Start();
      break;
    case 'O': // oh not zero      
      tactor.setPosition_benchTest(0x5000);
      startBench = true;
      
      break;
    case 'A':
      writingToMemory = true;
        
      break;
    }

  }

  if(!stopStream){ 
  // general 
  Serial.print((int)move_robots);
  Serial.print("\t");
  Serial.print((unsigned int)record); // record
  Serial.print("\t");
  // strokes pos command
  Serial.print((unsigned int)count);
  Serial.print("\t");
  // feedback information
  Serial.print((unsigned int) FB.fb_type);
  Serial.print("\t");
  // feedback position
  Serial.print((unsigned int)tactor.position);
  Serial.print("\t");
  Serial.print((int)FB.state);
  Serial.print("\t");
  Serial.print((int)FB.vib_state);
  // forces
  Serial.print("\t");
  Serial.print(cube.fifoR.last());
  Serial.print("\t");
  Serial.print(cube.fifoL.last());
  Serial.print("\t");
  Serial.print(cube.fifoF.last());
  Serial.print("\t");
  //error
  Serial.print((int)err_flag);
  Serial.print("\t");
  Serial.print((int)success);
  Serial.print("\t");
  Serial.print((int) x);
  Serial.print("\t");
  Serial.print((int) Group);
  Serial.print("\t");
  Serial.print(pocketNum);
  Serial.print("\n");
  }
  if(err_flag == 3){
    record= 0;
    err_flag =0;
  }
}




void debounceInterrupt() {
 if((millis() - last_micros) > debouncing_time && move_robots) {
      if(state){ // from low -> High
      last_micros = millis();
      state = digitalRead(interruptPin);
      digitalWrite(GRN, LOW);
      digitalWrite(RED, LOW);
      digitalWrite(WHT, LOW);
      record = 0;
      failed = 0;
      success = 0;
      count++;
      aPause = false;
      }
      else
      { // from high -> low
      if((millis()- timeFromLastIncrement) > 100 ){
      success = 0;
      err_flag =0;
      failed = 0;
      if (count > 0) record = 1;
      else record =0;
      digitalWrite(RED, LOW);
      digitalWrite(WHT, LOW);
      digitalWrite(GRN, HIGH);
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


bool checkIfPause(void){
  // if it arrived to marker
  for(unsigned char i = 0; i<3;i++){
    if (count == nExp_test[i]+1) return true;
  }
  // if you want to pause on purpose

  return false;
}
