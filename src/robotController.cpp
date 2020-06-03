#include <wiringPi.h>
#include "robotController.h"
#include "constants.h"
#include <iostream>

RobotController::RobotController()
{
  wiringPiSetup();
  pinMode(23, PWM_OUTPUT);
  pinMode(26, PWM_OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(28, OUTPUT);
  digitalWrite(27, 0);
  digitalWrite(28, 0);
  pwmWrite(23, 0);
  pwmWrite(26, 0);
}

RobotController::~RobotController()
{
  digitalWrite(27, 0);
  digitalWrite(28, 0);
  pwmWrite(23, 0);
  pwmWrite(26, 0);
}

void RobotController::command(char command)
{
  switch (command)
    {
    case MOVE_FORWARD:
      digitalWrite(27, 0);
      digitalWrite(28, 0);
      pwmWrite(23, 1024);
      pwmWrite(26, 1024);
      break;
    case MOVE_BACKWARDS:
      digitalWrite(27, 1);
      digitalWrite(28, 1);
      pwmWrite(23, 1024);
      pwmWrite(26, 1024);
      break;
    case MOVE_LEFT:
      digitalWrite(27, 1);
      digitalWrite(28, 0);
      pwmWrite(23, 1024);
      pwmWrite(26, 1024);
      break;
    case MOVE_RIGHT:
      digitalWrite(27, 0);
      digitalWrite(28, 1);
      pwmWrite(23, 1024);
      pwmWrite(26, 1024);
      break;
    case STOP_MOVING:
      digitalWrite(27, 0);
      digitalWrite(28, 0);
      pwmWrite(23, 0);
      pwmWrite(26, 0);
      break;
    }
}