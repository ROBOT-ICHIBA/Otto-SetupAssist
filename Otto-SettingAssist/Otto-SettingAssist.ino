//--------------------------------------------------------------
//-- Otto-SettingAssist.ino
//-- Otto robot setup assistnt tool.
//--------------------------------------------------------------
//-- Auther: Takuji Kawata (takujikawata_pr), Oct 2021
//-- MIT License
//--------------------------------------------------------------

#include <Servo.h>
#include <EEPROM.h>

#include "Oscillator.h"

#define LEG_LEFT_PIN    2
#define LEG_RIGHT_PIN   3
#define FOOT_LEFT_PIN   4
#define FOOT_RIGHT_PIN  5

#define LEG_LEFT        0
#define LEG_RIGHT       1
#define FOOT_LEFT       2
#define FOOT_RIGHT      3

enum SERVO_MODE {NEUTRAL_MODE, TRIM_MODE, OSCILLATOR_MODE};

Servo       servo[4];
Oscillator  oscillator[4];
int         trims[4];
int         command = 'n';
int         prevCommand = 0;
SERVO_MODE  servoMode; 

void setup() {
  servo[LEG_LEFT].attach(LEG_LEFT_PIN);
  servo[LEG_RIGHT].attach(LEG_RIGHT_PIN);
  servo[FOOT_LEFT].attach(FOOT_LEFT_PIN);
  servo[FOOT_RIGHT].attach(FOOT_RIGHT_PIN);
  servoMode = NEUTRAL_MODE;

  for (int i = 0; i < 4; i++) {
    int servo_trim = EEPROM.read(i);
    if (servo_trim > 128) servo_trim -= 256;
    trims[i] = servo_trim;
  }
  
  Serial.begin(9600);
  
}

void setServoMode(SERVO_MODE mode) {
  if (mode != servoMode) {
    servoMode = mode;

    switch(mode) {
      case NEUTRAL_MODE:
      case TRIM_MODE:
        oscillator[LEG_LEFT].detach();
        oscillator[LEG_RIGHT].detach();
        oscillator[FOOT_LEFT].detach();
        oscillator[FOOT_RIGHT].detach();
        servo[LEG_LEFT].attach(LEG_LEFT_PIN);
        servo[LEG_RIGHT].attach(LEG_RIGHT_PIN);
        servo[FOOT_LEFT].attach(FOOT_LEFT_PIN);
        servo[FOOT_RIGHT].attach(FOOT_RIGHT_PIN);
        break;

      case OSCILLATOR_MODE:
        servo[LEG_LEFT].detach();
        servo[LEG_RIGHT].detach();
        servo[FOOT_LEFT].detach();
        servo[FOOT_RIGHT].detach();
        
        // apply latest trim 
        oscillator[LEG_LEFT].SetTrim(trims[LEG_LEFT]);
        oscillator[LEG_RIGHT].SetTrim(trims[LEG_RIGHT]);
        oscillator[FOOT_LEFT].SetTrim(trims[FOOT_LEFT]);
        oscillator[FOOT_RIGHT].SetTrim(trims[FOOT_RIGHT]);
  
        // activate servos to oscillator
        oscillator[LEG_LEFT].attach(LEG_LEFT_PIN);
        oscillator[LEG_RIGHT].attach(LEG_RIGHT_PIN);
        oscillator[FOOT_LEFT].attach(FOOT_LEFT_PIN);
        oscillator[FOOT_RIGHT].attach(FOOT_RIGHT_PIN);
        break;
    }
  }
}

void loop() {
  
  if (servoMode == NEUTRAL_MODE) {
    switch (command) {

      case 'n':
          doNeutralMode();
          doShowHelp();
          break;

      case 't':
          doTrimMode();
          doShowHelp();
          break;

      case 'h':
        doShowHelp();
        break;

      case '\n': 
        if (prevCommand == '\n') {
          doShowHelp();
        }
        break;
    }
  } else {
    switch (command) {

      case 'n':
          doNeutralMode();
          doShowHelp();
          break;

      case 't':
          doTrimMode();
          doShowHelp();
          break;
          
      case 's':
          doSaveTrims();
          break;

      case 'x':
          doPrintCurrentAdjustment();
          break;

      case 'w':
          doWalk(1,1000,1);
          break;
      
      case 'h':
        doShowHelp();
        break;

      case '\n': 
        if (prevCommand == '\n') {
          doShowHelp();
        }
        break;

      case '3':
        doAdjust(LEG_LEFT, -1);
        break;

      case 'e':
        doAdjust(LEG_LEFT, +1);
        break;

      case 'd':
        doAdjust(FOOT_LEFT, -1);
        break;

      case 'c':
        doAdjust(FOOT_LEFT, +1);
        break;

      case '1':
        doAdjust(LEG_RIGHT, -1);
        break;

      case 'q':
        doAdjust(LEG_RIGHT, +1);
        break;

      case 'a':
        doAdjust(FOOT_RIGHT, -1);
        break;

      case 'z':
        doAdjust(FOOT_RIGHT, +1);
        break;
    }
  }

  if (command != 0) {
    prevCommand = command;
    command = 0;
  }
  if (Serial.available() > 0) {
      command = Serial.read();
  } else {
    delay(100);
  }
}

void doShowHelp() {

  switch(servoMode) {
    case NEUTRAL_MODE:
      Serial.println("Help:");
      Serial.println("  n key + enter: move all servo (pin 2-5) to neutral (90 degree) position");
      Serial.println("  t key + enter: go to trim mode");
      Serial.println("  h key + enter or enter: Show this help message");
      break;
    case TRIM_MODE:
    case OSCILLATOR_MODE:
      Serial.println("Help:");
      Serial.println("  n key + enter: move all servo (pin 2-5) to neutral (90 degree) position");
      Serial.println("  t key + enter: move all servo (pin 2-5) to the center of the adjusted position");
      Serial.println("  w key + enter: walk");
      Serial.println("  x key + enter: show current adjustment");
      Serial.println("  h key + enter or enter: Show this help message");
      Serial.println("");
      Serial.println("  Trim your Otto!");
      Serial.println("              +-----------+");
      Serial.println("              |  O     O  |");
      Serial.println("              +-----------+");
      Serial.println("              |           |");
      Serial.println("              +-----------+");
      Serial.println("        1/q    +         +   3/e");
      Serial.println("               |         |");
      Serial.println("        a/z  __|         |__ d/c");
      Serial.println("");
      Serial.println("  1/q key + enter: Trim right leg  -/+");
      Serial.println("  a/w key + enter: Trim right foot -/+");
      Serial.println("  3/e key + enter: Trim left  leg  -/+");
      Serial.println("  d/c key + enter: Trim left  foot -/+");
      Serial.println("  s key + enter: Save current position");
      break;
  }
}

void doAdjust(int servo_index, int adj) {

  setServoMode(TRIM_MODE);
  
  trims[servo_index] += adj;
  servo[servo_index].write(trims[servo_index] + 90);

  switch (servo_index) {
    case LEG_LEFT:
      Serial.print("LEFT LEG:");
      break;
    case FOOT_LEFT:
      Serial.print("LEFT FOOT:");
      break;
    case LEG_RIGHT:
      Serial.print("RIGHT LEG:");
      break;
    case FOOT_RIGHT:
      Serial.print("RIGHT FOOT:");
      break;
  }
  Serial.println(trims[servo_index]);
  delay(30);        
}

void doNeutralMode() {
  setServoMode(NEUTRAL_MODE);
  
  for (int i = 0; i < 4; i++) {
    for (int j = 90; j < 105; j += 3) {
      delay(30);      
    }
    for (int j = 105; j > 75; j -= 3) {
      servo[i].write(j);
      delay(30);      
    }
    for (int j = 75; j <= 90; j += 3) {
      servo[i].write(j);
      delay(30);      
    }
    servo[i].write(90);
  }
  for (int i = 0; i < 4; i++) {
    servo[i].write(90);
  }
}

void doTrimMode() {
  setServoMode(TRIM_MODE);
  
  for (int i = 0; i < 4; i++) {
    servo[i].write(90 + trims[i]);
  }
}

void doSaveTrims() {
  for (int i=0 ; i<=3 ; i=i+1) {
    byte v = trims[i] >= 0 ? trims[i] : trims[i] + 256;
    EEPROM.write(i,v);
  }
  Serial.println("Current position is saved!");
}

void doPrintCurrentAdjustment() {
  String left_leg = String(trims[LEG_LEFT]);
  String right_leg = String(trims[LEG_RIGHT]);
  String left_foot = String(trims[FOOT_LEFT]);
  String right_foot = String(trims[FOOT_RIGHT]);
  
  Serial.println("----------------------------------------");
  Serial.println("-- Current Adjustment                 --");
  Serial.println("----------------------------------------");
  Serial.println("              +-----------+");
  Serial.println("              |  O     O  |");
  Serial.println("              +-----------+");
  Serial.println("              |           |");
  Serial.println("              +-----------+");
  Serial.println("        (RL)   +         +   (LL)");
  Serial.println("               |         |");
  Serial.println("        (RF) __|         |__ (LF)");
  Serial.print("\nRL (RIGHT LEG) :");
  Serial.print(trims[LEG_RIGHT]);
  Serial.print("\t");
  Serial.print("LL (LEFT LEG)  :");
  Serial.println(trims[LEG_LEFT]);
  
  Serial.print("RF (RIGHT FOOT):");
  Serial.print(trims[FOOT_RIGHT]);  
  Serial.print("\t");
  Serial.print("LF (LEFT FOOT) :");
  Serial.println(trims[FOOT_LEFT]);
}

void doWalk(float steps, int T, int dir){
  //-- Oscillator parameters for walking
  //-- Hip sevos are in phase
  //-- Feet servos are in phase
  //-- Hip and feet are 90 degrees out of phase
  //--      -90 : Walk forward
  //--       90 : Walk backward
  //-- Feet servos also have the same offset (for tiptoe a little bit)
  int A[4]= {30, 30, 20, 20};
  int O[4] = {0, 0, 4, -4};
  double phase_diff[4] = {0, 0, DEG2RAD(dir * -90), DEG2RAD(dir * -90)};

  //-- Let's oscillate the servos!
  otto_execute(A, O, T, phase_diff, steps);
}

void otto_execute(int A[4], int O[4], int T, double phase_diff[4], float steps){

  int cycles=(int)steps;

  setServoMode(OSCILLATOR_MODE);

  //-- Execute complete cycles
  if (cycles >= 1)
    for(int i = 0; i < cycles; i++)
      otto_oscillateServos(A,O, T, phase_diff,1);

  //-- Execute the final not complete cycle
  otto_oscillateServos(A,O, T, phase_diff,(float)steps-cycles);  
}

void otto_oscillateServos(int A[4], int O[4], int T, double phase_diff[4], float cycle){

  for (int i=0; i<4; i++) {
    oscillator[i].SetO(O[i]);
    oscillator[i].SetA(A[i]);
    oscillator[i].SetT(T);
    oscillator[i].SetPh(phase_diff[i]);
  }
  double ref=millis();
   for (double x=ref; x<=T*cycle+ref; x=millis()){
     for (int i=0; i<4; i++){
        oscillator[i].refresh();
     }
  }
}
