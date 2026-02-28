#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include "encoder.h"
#include <Arduino.h>
#include <Servo.h>

class Motor_Control
{
  public:
    Motor_Control(const int pin, const Encoder* enc, const double LB, const double UB);
    void moveTo(double targ);
    bool checkStatus();
    //static void initializeSpeedUpdates();
    static void initializePositionUpdates();
    void registerMotor();
    double getPosition() const;
    bool inRange(float pos) const;
    void setZero();
    
    Servo motor;

    double LOWER_BOUND; //lower limit of rotation
    double UPPER_BOUND; //upper limit of rotation
    Encoder* encoder = nullptr;
    double targetPos;
    double targetSpeed;
    static const double TOLERANCE = 1.5;

    static const int NEUTRAL = 90;
    static const int MAX_SPEED = 80;
    static const int MIN_SPEED = 20;
    static const int MAX_DISTANCE = 180;
    volatile double speed = 0;

    //pid control
    //static const double P = 5;
    //static const double I = 5;
    //static const double D = 5;

    static const int MAX_MOTORS = 6;
    static inline Motor_Control* motors_array[MAX_MOTORS] = {nullptr};
    static inline int motorCount = 0;
    
};


#endif