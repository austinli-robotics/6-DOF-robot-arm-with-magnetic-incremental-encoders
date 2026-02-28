#include "absolute_encoder.h"

Absolute_Encoder::Absolute_Encoder(const int p)
{
  pin = p;
  registerEncoder();
}

void Absolute_Encoder::registerEncoder()
{
  if (numEncoders < MAX_ENCODERS) {
    encodersArray[numEncoders] = this;
    numEncoders ++;

    for (int i = 0; i < FILTER_SIZE; i++) {
      data_values[i] = analogRead(pin);
    }

    position = positionCalc();
  }
}

double Absolute_Encoder::getPosition()
{
  return position;
} 

double Absolute_Encoder::positionCalc()
{
  double sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += data_values[i];
  }

  return map(sum/FILTER_SIZE, MIN_VALUE, MAX_VALUE, 0, 359.999);
}

static void Absolute_Encoder::updateAllEncoders()
{
  for (int e = 0; e < numEncoders; e++) {
    Absolute_Encoder* enc = encodersArray[e];

    enc->updateEncoderPosition();
  }
}

void Absolute_Encoder::updateEncoderPosition()
{
  data_values[filterIndex] = analogRead(pin);
  filterIndex = (filterIndex+1) % FILTER_SIZE;

  position = positionCalc();
}