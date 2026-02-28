#ifndef ABSOLUTE_ENCODER_H
#define ABSOLUTE_ENCODER_H
#include <Arduino.h>

class Absolute_Encoder
{
  public:
    Absolute_Encoder(const int p);
    static void updateAllEncoders();
    double getPosition();

  private:
    int pin;
    static const double MAX_VALUE = 920.6; //1024*90% - 1
    static const double MIN_VALUE = 101.4; //1024*10% - 1

    volatile double position;
    volatile double lastPosition;
    volatile double speed = 0;
    void updateEncoderPosition();

    //data smoothing
    static const int FILTER_SIZE = 18;
    static inline int filterIndex = 0;
    volatile double data_values[FILTER_SIZE];
    double positionCalc();

    //static array of encoders so ISR can access
    static const int MAX_ENCODERS = 3;
    static inline Absolute_Encoder* encodersArray[MAX_ENCODERS] = {nullptr};
    static inline int numEncoders = 0;
    void registerEncoder();
};



#endif