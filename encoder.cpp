#include "encoder.h"

Encoder::Encoder(const int pin1, const int pin2, const int pin3)
{
  inc_encoder = new Incremental_Encoder(pin1,pin2,pin3);
  encoder_type = 0;
}

Encoder::Encoder(const int pin)
{
  abs_encoder = new Absolute_Encoder(pin);
  encoder_type = 1;
}

Encoder::~Encoder()
{
  if (0 == encoder_type)
    delete inc_encoder;
  else
    delete abs_encoder;
}

static void Encoder::updateEncoderPositions()
{
  Absolute_Encoder::updateAllEncoders();
  Incremental_Encoder::updateAllEncoders();
}

//overwrite position of incremental encoder
void Encoder::setPosition(float pos)
{
  if (0 == encoder_type)
    inc_encoder->setPosition(pos);
}

//get position depending on encoder type
double Encoder::checkPosition() const
{
  if (0 == encoder_type)
    return inc_encoder->getPosition();
  else
    return abs_encoder->getPosition();
}

bool Encoder::limitSwitchPressed()
{
  if (0 == encoder_type)
    return inc_encoder->limitSwitchPressed();
}
