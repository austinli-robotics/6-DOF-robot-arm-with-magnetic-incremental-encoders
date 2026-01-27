#ifndef ABSOLUTE_ENCODER_H
#define ABSOLUTE_ENCODER_H
#include <Arduino.h>

class Absolute_Encoder
{
  public:
    Absolute_Encoder(const int pin);
    static void configure_abs_encoders();
    volatile bool dataReady = false;
    volatile bool speedChanged = false;
    volatile double position;
    volatile double lastPosition;
    volatile double speed = 0;
    
    static void handleInterrupt2();
    static void handleInterrupt0();

    int pin;
    static const double MAX_VALUE = 920.6; //1024*90% - 1
    static const double MIN_VALUE = 101.4; //1024*10% - 1

    //data smoothing
    static const byte WINDOW = 8;
    volatile int indx = 0;
    volatile double data_values[WINDOW];

    //static array of encoders so ISR can access
    static const int MAX_ENCODERS = 3;
    static inline Absolute_Encoder* encoders[MAX_ENCODERS] = {nullptr};
    static inline int encodersCount = 0;
};



#endif