#include "motor_control.h"

Motor_Control::Motor_Control(const int pin, const Encoder* enc, const double LB, const double UB)
{
  motor.attach(pin);
  encoder = enc;
  LOWER_BOUND = LB;
  UPPER_BOUND = UB;
  registerMotor(this);
}


void Motor_Control::registerMotor(Motor_Control* mot)
{
  if (motorCount < MAX_MOTORS)
  {
    motors_array[motorCount] = mot;
    motorCount++;
  }
}


void Motor_Control::moveTo(double targ)
{
  targetPos = targ;
  double delta_speed;
  double position = encoder->checkPosition();
  int direction = (position - targetPos) / abs(position - targetPos);

  if (abs(position - targetPos) < 180){
    delta_speed = (MAX_SPEED - MIN_SPEED) * (abs(position - targetPos))/180;
  }
  else{
    delta_speed = MAX_SPEED - MIN_SPEED;
  }

  speed = (MIN_SPEED + delta_speed) * direction;
  motor.write(speed);
}


bool Motor_Control::checkStatus()
{
  double position = encoder->checkPosition();
  double speed = encoder->checkSpeed();
  //targetSpeed = (targetPos - position)*P;

  if (abs(position-targetPos) < TOLERANCE){ //if reached target angle
    motor.write(NEUTRAL);
    return true;
  }

  return false;
}


void Motor_Control::initializeSpeedUpdates()
{
  cli();
  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3 = 0;

  OCR3B = 2499; //5kHz

  TCCR3B |= (1 << WGM32);
  TCCR3B |= (1 << CS32) | (1 << CS30);
  TIMSK3 |= (1 << OCIE3A);
  sei();
}


double Motor_Control::getPosition() const
{
  return encoder->checkPosition();
}


double Motor_Control::getSpeed() const
{
  return encoder->checkSpeed();
}


bool Motor_Control::inRange(float pos) const
{
  return (pos >= LOWER_BOUND && pos <= UPPER_BOUND);
}


void Motor_Control::setZero()
{
  if (encoder->encoder_type == 0)
  {
    while (digitalRead(encoder->inc_encoder->LIMIT_SWITCH) == HIGH)
    {
      motor.write(NEUTRAL + MIN_SPEED + 20);
    }
    motor.write(NEUTRAL);

    motor.write(NEUTRAL - MIN_SPEED);
    delay(2000);

    while (digitalRead(encoder->inc_encoder->LIMIT_SWITCH) == HIGH)
    {
      motor.write(NEUTRAL + MIN_SPEED);
    }
    motor.write(NEUTRAL);
    encoder->setPosition(180);
  }
}


//update speed with ISR 50Hz
ISR(TIMER3_COMPB_vect)
{
  for (int m = 0; m < Motor_Control::motorCount; m++)
  {
    Motor_Control* mot = Motor_Control::motors_array[m];

    double delta_speed;
    double position = mot->encoder->checkPosition();
    int direction = (position - mot->targetPos) / fabs(position - mot->targetPos);

    if (fabs(position - mot->targetPos) < Motor_Control::MAX_DISTANCE){
      delta_speed = (Motor_Control::MAX_SPEED - Motor_Control::MIN_SPEED) * fabs(position - mot->targetPos)/Motor_Control::MAX_DISTANCE;
    }
    else{
      delta_speed = Motor_Control::MAX_SPEED - Motor_Control::MIN_SPEED;
    }

    mot->speed = (Motor_Control::MIN_SPEED + delta_speed) * direction;
    mot->motor.write(mot->speed);
  }
  
}
