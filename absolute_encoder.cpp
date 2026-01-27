#include "absolute_encoder.h"

Absolute_Encoder::Absolute_Encoder(const int pin)
{
  this->pin = pin;
  encoders[encodersCount] = this;
  encodersCount++;

  for (int i=0; i<WINDOW; i++)
  {
    data_values[i] = map(analogRead(pin), MIN_VALUE, MAX_VALUE, 0, 359);
  }
}

//populate encoder arrays with initial values;
void Absolute_Encoder::configure_abs_encoders()
{
  for (int e = 0; e < encodersCount; e++)
  {
    Absolute_Encoder* enc = encoders[e];

    double sum = 0;
    for (byte i=0; i<WINDOW; i++)
    {
      enc->data_values[i] = map(analogRead(enc->pin), MIN_VALUE, MAX_VALUE, 0, 359.99);
      sum += map(analogRead(enc->pin), MIN_VALUE, MAX_VALUE, 0, 359.99);
    }

    enc->position = sum/WINDOW;
  }
}

//update position
void Absolute_Encoder::handleInterrupt2()
{
  for (int e = 0; e < encodersCount; e++)
  {
    Absolute_Encoder* enc = encoders[e];
    enc->data_values[enc->indx] = map(analogRead(enc->pin), MIN_VALUE, MAX_VALUE, 0, 359.99);
    enc->indx = (enc->indx + 1) % WINDOW;

    double sum = 0;
    for (int i=0; i<WINDOW; i++)
    {
      sum += enc->data_values[i];
    }

    enc->position = sum/WINDOW;
    enc->dataReady = true;
  }
}

//update speed
void Absolute_Encoder::handleInterrupt0()
{
  for (int e = 0; e < encodersCount; e++)
  {
    Absolute_Encoder* enc = encoders[e];

    enc->speed = (enc->position - enc->lastPosition) / 0.001;
    enc->lastPosition = enc->position;
    enc->speedChanged = true;
  }
}