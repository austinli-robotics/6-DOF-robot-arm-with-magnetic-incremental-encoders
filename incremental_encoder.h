#ifndef INCREMENTAL_ENCODER_H
#define INCREMENTAL_ENCODER_H
#include <Arduino.h>

struct Incremental_Encoder
{
    Incremental_Encoder(const int pin1, const int pin2, const int pin3);
    static void configure_inc_encoders();
    double update_inc_encoder();
    bool registerEncoder(Incremental_Encoder* enc);
    int hallPin1; //for increment detection
    int hallPin2; // for detecting direction
    int LIMIT_SWITCH; // for setting zero
    int checkDirection(int lastState1, int sensor1, int lastState2, int sensor2);
    
    static const int BASELINE = 512;
    volatile int sensorValue1 = 512;
    volatile int sensorValue2 = 512;
    volatile bool dataReady = false;
    volatile bool speedChanged = false;
    
    volatile int lastState1;
    volatile int lastState2;

    static const int TOLERANCE = 10;
    static const int MAX_NORTH = 160;
    static const int MAX_SOUTH = 850;
    static const int NUM_POLES = 28;
    static const int RESOLUTION = 4000;
    static const double STEP_SIZE = (MAX_SOUTH-MAX_NORTH)*NUM_POLES / RESOLUTION; //how great 1 increment of rotation is

    volatile double position = 0;
    volatile double lastSpeedPos = 0;
    volatile double speed = 0;

    //data smoothing
    static const int WINDOW = 8;
    volatile int readings1[WINDOW];
    volatile int readings2[WINDOW];
    volatile int indx = 0;

    //static array of encoders so ISR can access
    static void handleInterrupt2();
    static void handleInterrupt0();
    static const int MAX_ENCODERS = 3;
    static inline Incremental_Encoder* encoders[MAX_ENCODERS] = {nullptr};
    static inline int encoderCount = 0;


};



#endif 