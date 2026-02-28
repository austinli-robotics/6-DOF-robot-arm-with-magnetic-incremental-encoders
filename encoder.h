#ifndef ENCODER_H
#define ENCODER_H
#include "incremental_encoder.h"
#include "absolute_encoder.h"

class Encoder
{
  public:
    Encoder(const int, const int, const int);
    Encoder(const int);
    ~Encoder();
    static void updateEncoderPositions();
    double checkPosition() const;
    void setPosition(float);
    bool limitSwitchPressed();
    int encoder_type; //0 for incremental encoder; 1 for absolute encoder

  private:
    Incremental_Encoder* inc_encoder = nullptr;
    Absolute_Encoder* abs_encoder = nullptr;


};


#endif