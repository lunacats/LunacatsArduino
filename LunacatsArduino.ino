#include <ArduinoJson.h>
#include <Servo.h>

//Used for the Actuator
#define NEUTRAL 92;
#define ACT_UP_MATCH 140;
#define ACT_UP 10;
#define ACT_DOWN 170;
#define ACT_UP_MATCH 45;

Servo FLWheel; //This is definitely correct. Good job Tom
Servo BLWheel;
Servo FRWheel;
Servo BRWheel;
Servo ActLeft;
Servo ActRight;


void forward()
{
  int i = 110;
  FLWheel.write(i);
  BLWheel.write(i);
  FRWheel.write(i);
  BRWheel.write(i);
}

void left()
{
  int i = 90;
  FLWheel.write(i - 25);
  BLWheel.write(i - 25);
  FRWheel.write(25 + i);
  BRWheel.write(25 + i);
}

void right()
{
  int i = 90;
  FLWheel.write(25 + i);
  BLWheel.write(25 + i);
  FRWheel.write(i - 25);
  BRWheel.write(i - 25);
}

void back()
{
  int i = 70;
  FLWheel.write(i);
  BLWheel.write(i);
  FRWheel.write(i);
  BRWheel.write(i);
}

void halt()
{
  int i = 90;
  FLWheel.write(i);
  BLWheel.write(i);
  FRWheel.write(i);
  BRWheel.write(i);
}


void upAct()
{
  ActLeft.write(ACT_UP);
  ActRight.write(ACT_UP_MATCH);
}

void stopAct {
  ActLeft.write(NEUTRAL);
  ActRight.write(NEUTRAL);
}

void downAct()
{
  ActLeft.write(ACT_DOWN);
  ActRight.write(ACT_DOWN_MATCH);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  FLWheel.attach(A0);
  BLWheel.attach(A2);
  FRWheel.attach(A1);
  BRWheel.attach(A3);
  ActLeft.attach(A5);
  ActRight.attach(A6);

  Serial.begin(115200);

  halt();
}

void parseCommand(String buff)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(buff);
  if (!root.success())
  {
    Serial.println("Bad Json String");
  }
  else
  {

    if (root["c"] == 0)
    {
      back();
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (root["c"] == 1)
    {
      forward();
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (root["c"] == 2)
    {
      left();
    }
    else if (root["c"] == 3)
    {
      right();
    } else if (root["c"] ==10)
    {
      upAct();
    } else if (root["c"] ==11)
    {
      downAct();
    }
    else
    {
      stopAct());
      halt();
    }

    Serial.println(buff);
  }
}

void loop()
{

  if (Serial.available() > 0)
  {
    parseCommand(Serial.readString());
  }
}