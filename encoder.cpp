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
  if (0 == encoder_type){
    delete inc_encoder;
  }
  else{
    delete abs_encoder;
  }
}

void Encoder::configureEncoders()
{
  cli();
  //timer used for updating position
  TCCR2A = 0; //clearing bits
  TCCR2B = 0; //clearing bits
  TCNT2 = 0;
  OCR2A = 49; //setting to check every 5kHz
  TCCR2B |= (1 << WGM21);
  TCCR2B |= (1<<CS22);
  TIMSK2 |= (1<<OCIE2A);

  //timer used for updating speed
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;
  OCR0A = 249; //speed 1kHz
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);
  sei();

  Incremental_Encoder::configure_inc_encoders();
  Absolute_Encoder::configure_abs_encoders();
}

//overwrite position of incremental encoder
void Encoder::setPosition(double pos) const
{
  if (0 == encoder_type)
    inc_encoder->position = 0;
}

//5kHz position updater
ISR(TIMER2_COMPA_vect)
{
  Incremental_Encoder::handleInterrupt2();
  Absolute_Encoder::handleInterrupt2();
}

//1kHz speed updater
ISR(TIMER0_COMPA_vect)
{
  Incremental_Encoder::handleInterrupt0();
  Absolute_Encoder::handleInterrupt0();
}

//get position depending on encoder type
double Encoder::checkPosition()
{
  if (0 == encoder_type){
    return inc_encoder->position;
  }
  else{
    return abs_encoder->position;
  }
}

//get speed depending on encoder type
double Encoder::checkSpeed()
{
  if (0 == encoder_type){
    return inc_encoder->position;
  }
  else{
    return abs_encoder->position;
  }
}



/*

    Serial.print("Position: ");
    Serial.println(position);
*/