#include "incremental_encoder.h"

Incremental_Encoder::Incremental_Encoder(const int pin1, const int pin2, const int pin3)
{
  hallPin1 = pin1;
  hallPin2 = pin2;
  LIMIT_SWITCH = pin3;
  
  registerEncoder(this);
}

bool Incremental_Encoder::registerEncoder(Incremental_Encoder* enc)
{
  if (encoderCount < MAX_ENCODERS)
  {
    encoders[encoderCount] = enc;
    encoderCount++;
    return true;
  }
  return false;
}

void Incremental_Encoder::configure_inc_encoders()
{
  for (int e = 0; e < encoderCount; e++)
  {
    Incremental_Encoder* enc = encoders[e];

    for (byte i=0; i < WINDOW; i++)
    {
      enc->readings1[i] = analogRead(enc->hallPin1);
      enc->readings2[i] = analogRead(enc->hallPin2);
    }

    int sum1 = 0, sum2 = 0;
  
    for (byte i = 0; i < WINDOW; i++) 
    {
      sum1 += enc->readings1[i];
      sum2 += enc->readings2[i];
    }
    enc->lastState1 = sum1 / WINDOW;
    enc->lastState2 = sum2 / WINDOW;
  }
  
}

//checks and updates hall effect sensor values for all encoders
void Incremental_Encoder::handleInterrupt2()
{
  for (int e = 0; e < encoderCount; e++)
  {
    Incremental_Encoder* enc = encoders[e];

    enc->readings1[enc->indx] = analogRead(enc->hallPin1);
    enc->readings2[enc->indx] = analogRead(enc->hallPin2);
    enc->indx = (enc->indx + 1) % WINDOW; //moves to next index for next interrupt
  
    long sum1 = 0, sum2 = 0;
    for (byte i = 0; i < WINDOW; i++) 
    {
      sum1 += enc->readings1[i];
      sum2 += enc->readings2[i];
    }
    enc->lastState1 = sum1 / WINDOW;
    enc->lastState2 = sum2 / WINDOW;
    enc->dataReady = true;
  }
}

//updates speed every 1kHz
void Incremental_Encoder::handleInterrupt0()
{
  for (int e = 0; e < encoderCount; e++)
  {
    Incremental_Encoder* enc = encoders[e];

    enc->speed = (enc->position - enc->lastSpeedPos) / 0.001;
    enc->lastSpeedPos = enc->position;
    enc->speedChanged = true;
  }
}

int Incremental_Encoder::checkDirection(int lastState1, int sensor1, int lastState2, int sensor2) //returns -1 for left, 1 for right
{
  if (lastState1 > 534){ //sensor1 close to south
    if (sensor2 < lastState2) //sensor2 decreases
      return -1;
    else
      return 1;
  }
  else if (lastState1 < 490){ //sensor1 close to north
    if (sensor2 > lastState2) //sensor2 increases
      return -1;
    else
      return 1;
  }
  else{ //sensor1 is in-between
    if (lastState2 > 512){
      if (sensor1 < lastState1) //sensor1 decreases
        return -1;
      else
        return 1;
    }
    else{
      if (sensor1 > lastState1) //sensor1 increases
        return -1;
      else
        return 1;
    }
  }
}

double Incremental_Encoder::update_inc_encoder()
{
  if (dataReady)
  {
    dataReady = false;

    int delta = abs(lastState1 - sensorValue1);

    if (delta > TOLERANCE){ //filters out jitter
      int direction = checkDirection(lastState1,sensorValue1,lastState2,sensorValue2);
      
      position += (1.0 * delta * direction) / STEP_SIZE * 360 / 4000; //update postion ; (value change)/(step size) * (360 degrees)/(4000 steps per rev)

      lastState1 = sensorValue1;
      lastState2 = sensorValue2;
    }

    //loop position between 0-360
    if (position > 360)
      position = fmod(position,360);
    if (position < 0)
      position += 360;
    
    Serial.print("Sensor Value1: ");
    Serial.println(sensorValue1);
  }
  return position;
}

