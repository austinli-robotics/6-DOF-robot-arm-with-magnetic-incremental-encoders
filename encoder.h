#ifndef ENCODER_H
#define ENCODER_H
#include "incremental_encoder.h"
#include "absolute_encoder.h"

struct Encoder
{
  Encoder(const int pin1, const int pin2, const int pin3);
  Encoder(const int pin);
  ~Encoder();
  static void configureEncoders();
  double checkSpeed();
  double checkPosition();
  void setPosition(double pos) const;

  int encoder_type; //0 for incremental encoder; 1 for absolute encoder
  Incremental_Encoder* inc_encoder = nullptr;
  Absolute_Encoder* abs_encoder = nullptr;


};


#endif