#include <ArduinoJson.h>
#include <Servo.h>

//##########################################//
//                DEFINITIONS               //
//##########################################//

//Used for the Actuator
#define NEUTRAL 92
#define ACT_DOWN_MATCH 140
#define ACT_UP 10
#define ACT_DOWN 170
#define ACT_UP_MATCH 45

#define AUGUR_ON_OFF 28
#define AUGUR_DIRECTION 22

// Motor Encoder Light Definitions
// ZERO          - Solid Orange
// BACKWARD      - flashing or solid red   (full speed = solid)
// FORWARD       - flashing or solid green (full speed = solid)
// NOT CONNECTED - Flashing Orange (hate to see that happen)

Servo FLWheel; //This is definitely correct. Good job Tom
Servo BLWheel;
Servo FRWheel;
Servo BRWheel;
Servo ActLeft;
Servo ActRight;
Servo BallScrews;
Servo Conveyor;

//##########################################//
//                 MOVEMENT                 //
//##########################################//

// ------------ MOTOR FUNCTIONS ------------ //

// #-NOTE-# 90 is not exact 0 for all motor 
// encoders due to variation in creation. 
// Therefore, trial and error determined 
// the offset of the values read. 

void powerWheel_FL(int pwr) {
  FLWheel.write( pwr - 1 );
}

void powerWheel_FR(int pwr) {
  FRWheel.write( pwr - 2 );
}

void powerWheel_BL(int pwr) {
  BLWheel.write( pwr - 1 );
}

void powerWheel_BR(int pwr) {
  BRWheel.write( pwr - 1 );
}

//pwr range:  -90 < pwr < 90
//reverse:    -90 < pwr < 0
//forward:     0  < pwr < 90
//stop:        pwr = 0
void powerDrive_Left(int pwr) {
  //convert range to 0 < pwr < 180
  pwr = pwr+90;
  powerWheel_FL(pwr);
  powerWheel_BL(pwr);
}

//pwr range: -90 < pwr < 90
void powerDrive_Right(int pwr) {
  //convert range to 0 < pwr < 180
  pwr = pwr+90;
  powerWheel_FR(pwr);
  powerWheel_BR(pwr);
}

//pwr range: -90 < pwr < 90
void powerDrive_All(int pwr) {
  powerDrive_Left(pwr);
  powerDrive_Right(pwr);
}

void forward(){
  int pwr = 20;
  powerDrive_All(pwr);
}

void left(){
  int pwr = 25;
  powerDrive_Left(-pwr);
  powerDrive_Right(pwr);
}

void right(){
  int pwr = 25;
  powerDrive_Left(pwr);
  powerDrive_Right(-pwr);
}

void back(){
  int pwr = -20;
  powerDrive_All(pwr);
}

void halt(){
  int pwr = 0;
  powerDrive_All(pwr);
}

// ------------ ACTUATOR ------------ //

void upAct(){
  ActLeft.write(ACT_UP);
  ActRight.write(ACT_UP_MATCH);
}

void stopAct() {
  ActLeft.write(NEUTRAL);
  ActRight.write(NEUTRAL);
}

void downAct(){
  ActLeft.write(ACT_DOWN);
  ActRight.write(ACT_DOWN_MATCH);
}

// ------------ DRILL ------------ //

void drillUp(){
  ActLeft.write(ACT_DOWN);
  ActRight.write(ACT_DOWN_MATCH);
}

// ------------ BALL SCREWS------------ //

void ballsDrop(){
  BallScrews.write(10);
}
void ballsUp(){
  BallScrews.write(170);
}
void ballsHalt(){
  BallScrews.write(90);
}

// ------------ CONVEYOR ------------ //

void conveyorEmpty(){
  Conveyor.write(10);
}
void conveyorCollect(){
  Conveyor.write(170);
}
void conveyorHalt(){
  Conveyor.write(90);
}

// ------------ AUGER ------------ //

void turnAugurClockwise() {
  digitalWrite(AUGUR_ON_OFF,HIGH);
  digitalWrite(AUGUR_DIRECTION,LOW);
}
void turnAugurCounterClockwise() {
  digitalWrite(AUGUR_ON_OFF,HIGH);
  digitalWrite(AUGUR_DIRECTION,HIGH);
}
void turnAugurOff() {
  digitalWrite(AUGUR_ON_OFF,   LOW);
  digitalWrite(AUGUR_DIRECTION,LOW);
}

//##########################################//
//                   MAIN                   //
//##########################################//

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  FLWheel.attach(A0);
  BLWheel.attach(A2);
  FRWheel.attach(A1);
  BRWheel.attach(A3);
  BallScrews.attach(A4);
  ActLeft.attach(A5);
  ActRight.attach(A6);
  Conveyor.attach(A7);
  
  Serial.begin(115200);
  Serial.setTimeout(20);

  pinMode(AUGUR_ON_OFF,OUTPUT);
  pinMode(AUGUR_DIRECTION,OUTPUT);

  
  halt();
}


enum Command {
  BACKWARD = 0, FORWARD = 1, LEFT = 2, RIGHT = 3,
  UPACT = 10, DOWNACT = 11, 
  BALLDROP = 12, BALLUP = 13,
  CNVR_COLLECT = 14, CNVR_EMPTY = 15,
  AUGERTURN_CC = 16, AUGERTURN_CCW = 17,
  OFFSET_POS = 20, OFFSET_NEG = 21
} cmd;

void parseCommand(String buff)
{
  Serial.println(buff);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(buff);
  
  if (!root.success()) {
    Serial.println("Bad Json String");
  }
  else {
    int input = root["c"];
    switch(input) {
      case BACKWARD:
        back();
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case FORWARD:
        forward();
        digitalWrite(LED_BUILTIN, LOW);
        break;
      case LEFT:
        left();
        break;
      case RIGHT:
        right();
        break;
      case UPACT:
        upAct();
        break;
      case DOWNACT:
        downAct();
        break;
      case 19:
      case BALLDROP:
        ballsDrop();
        break;
      case 18:
      case BALLUP:
        ballsDrop();
        break;
      case CNVR_COLLECT:
        conveyorCollect();
        break;
      case CNVR_EMPTY:
        conveyorEmpty();
        break;
      case AUGERTURN_CC:
        turnAugurClockwise();
        break;
      case AUGERTURN_CCW:
        turnAugurCounterClockwise();
        break;
      case OFFSET_POS:
        //offset += 5;
        break;
      case OFFSET_NEG:
        //offset -= 5;
        break;
      default:
        break;
//      Serial.println("wrong command");
        stopAct();
        halt();
        turnAugurOff();
        conveyorHalt();
        ballsHalt();
    }
/*
    if (root["c"] == 0){
      back();
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (root["c"] == 1){
      forward();
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (root["c"] == 2){
      left();
    }
    else if (root["c"] == 3){
      right();
    } 
    else if (root["c"] ==10){
      upAct();
    } 
    else if (root["c"] ==11){
      downAct();
    } 
    else if (root["c"] ==12){
      ballsDrop();
    } 
    else if (root["c"] ==13){
      ballsUp();
    } 
    else if (root["c"] ==14){
      ConveyorCollect();
    } 
    else if (root["c"] ==15){
      ConveyorEmpty();
    } 
    else if (root["c"] == 16){
      turnAugurClockwise();
    }
    else if (root["c"] == 17){
      turnAugurCounterClockwise();
    }
    else if (root["c"] == 18){
      ballsUp();
    }
    else if (root["c"] == 19){
      ballsDrop();
    }
    else if (root["c"] == 20){
      //offset += 5;
    }
    else if(root["c"] == 21){
      //offset -= 5;
    }
    else {
//      Serial.println("wrong command");
      stopAct();
      halt();
      turnAugurOff();
      ConveyorHalt();
      ballsHalt();
    }
*/
  }
}

void loop()
{
  // interpretCommand();

  if (Serial.available() > 0)
  {
    String recieved = Serial.readString();
    if(recieved == "init"){
      Serial.println("success");
    }
    else{
      parseCommand(recieved);
    }
  }
  
}
