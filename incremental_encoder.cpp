#include "incremental_encoder.h"

Incremental_Encoder::Incremental_Encoder(const int p1, const int p2, const int p3)
{
  sensorPin1 = p1;
  sensorPin2 = p2;
  limitSwitchPin = p3;
  registerEncoder();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::registerEncoder()
{
  if (numEncoders < MAX_ENCODERS) {
    pinMode(limitSwitchPin, INPUT_PULLUP); //setup limit switch

    encodersArray[numEncoders] = this;
    numEncoders++;

    for (int i = 0; i < FILTER_SIZE; i++) {
      filterArray_sensor1[i] = analogRead(sensorPin1);
      filterArray_sensor2[i] = analogRead(sensorPin2);
    }

    currValue_sensor1 = filterCalc(filterArray_sensor1);
    currValue_sensor2 = filterCalc(filterArray_sensor2);
    position = 180;
    count = 0;

    //initialize current step
    float range = TRUE_NORTH + INTERVAL_SIZE/2;
    int step = 1;
    bool sensor1_done = false, sensor2_done = false;
    while (currValue_sensor1 >= range - INTERVAL_SIZE || currValue_sensor2 >= range - INTERVAL_SIZE) {
      if (!sensor1_done && currValue_sensor1 < range) {
        currentStep_sensor1 = step;
        sensor1_done = true;
      }
      if (!sensor2_done && currValue_sensor2 < range) {
        currentStep_sensor2 = step;
        sensor2_done = true;
      }
      
      step ++;
      range += INTERVAL_SIZE;
    }

    //define intervals
    bool sensor1_defined = false, sensor2_defined = false;

    //sensor 1
    if (currentStep_sensor1 == 1) {
      currentInterval_sensor1[0] = currentInterval_sensor1[1] = TRUE_NORTH;
      cwInterval_sensor1[0] = ccwInterval_sensor1[0] = TRUE_NORTH;
      cwInterval_sensor1[1] = ccwInterval_sensor1[1] = TRUE_NORTH + INTERVAL_SIZE;
      sensor1_defined = true;
    }
    else if (currentStep_sensor1 == RESOLUTION_MULTIPLIER + 1) {
      currentInterval_sensor1[0] = currentInterval_sensor1[1] = TRUE_SOUTH;
      cwInterval_sensor1[0] = ccwInterval_sensor1[0] = TRUE_SOUTH - INTERVAL_SIZE;
      cwInterval_sensor1[1] = ccwInterval_sensor1[1] = TRUE_SOUTH;
      sensor1_defined = true;
    }
    else {

      currentInterval_sensor1[0] = TRUE_NORTH + (currentStep_sensor1 - 2)*INTERVAL_SIZE;
      currentInterval_sensor1[1] = currentInterval_sensor1[0] + INTERVAL_SIZE;

      if (currentStep_sensor2 < RESOLUTION_MULTIPLIER/2) { //on upwards slope
        if (currentStep_sensor1 == 2) {
          cwInterval_sensor1[0] = cwInterval_sensor1[1] = TRUE_NORTH;
          ccwInterval_sensor1[0] = currentInterval_sensor1[1];
          ccwInterval_sensor1[1] = ccwInterval_sensor1[0] + INTERVAL_SIZE;
          sensor1_defined = true;
        }
        else if (currentStep_sensor1 == RESOLUTION_MULTIPLIER) {
          cwInterval_sensor1[1] = currentInterval_sensor1[0];
          cwInterval_sensor1[0] = cwInterval_sensor1[1] - INTERVAL_SIZE;
          ccwInterval_sensor1[0] = ccwInterval_sensor1[1] = TRUE_SOUTH;
          sensor1_defined = true;
        }
        else {
          cwInterval_sensor1[1] = currentInterval_sensor1[0];
          cwInterval_sensor1[0] = cwInterval_sensor1[1] - INTERVAL_SIZE;
          ccwInterval_sensor1[0] = currentInterval_sensor1[1];
          ccwInterval_sensor1[1] = ccwInterval_sensor1[0] + INTERVAL_SIZE;
          sensor1_defined = true;
        }
      }
      else { //on downwards slope
        if (currentStep_sensor1 == 2) {
          ccwInterval_sensor1[0] = ccwInterval_sensor1[1] = TRUE_NORTH;
          cwInterval_sensor1[0] = currentInterval_sensor1[1];
          cwInterval_sensor1[1] = cwInterval_sensor1[0] + INTERVAL_SIZE;
          sensor1_defined = true;
        }
        else if (currentStep_sensor1 == RESOLUTION_MULTIPLIER) {
          ccwInterval_sensor1[1] = currentInterval_sensor1[0];
          ccwInterval_sensor1[0] = ccwInterval_sensor1[1] - INTERVAL_SIZE;
          cwInterval_sensor1[0] = cwInterval_sensor1[1] = TRUE_SOUTH;
          sensor1_defined = true;
        }
        else {
          ccwInterval_sensor1[1] = currentInterval_sensor1[0];
          ccwInterval_sensor1[0] = ccwInterval_sensor1[1] - INTERVAL_SIZE;
          cwInterval_sensor1[0] = currentInterval_sensor1[1];
          cwInterval_sensor1[1] = cwInterval_sensor1[0] + INTERVAL_SIZE;
          sensor1_defined = true;
        }
      }
    }

    //sensor 2
    if (currentStep_sensor2 == 1) {
      currentInterval_sensor2[0] = currentInterval_sensor2[1] = TRUE_NORTH;
      cwInterval_sensor2[0] = ccwInterval_sensor2[0] = TRUE_NORTH;
      cwInterval_sensor2[1] = ccwInterval_sensor2[1] = TRUE_NORTH + INTERVAL_SIZE;
    }
    else if (currentStep_sensor2 == RESOLUTION_MULTIPLIER + 1) {
      currentInterval_sensor2[0] = currentInterval_sensor2[1] = TRUE_SOUTH;
      cwInterval_sensor2[0] = ccwInterval_sensor2[0] = TRUE_SOUTH - INTERVAL_SIZE;
      cwInterval_sensor2[1] = ccwInterval_sensor2[1] = TRUE_SOUTH;
    }
    else {

      currentInterval_sensor2[0] = TRUE_NORTH + (currentStep_sensor2 - 2)*INTERVAL_SIZE;
      currentInterval_sensor2[1] = currentInterval_sensor2[0] + INTERVAL_SIZE;

      if (currentStep_sensor1 > RESOLUTION_MULTIPLIER/2) { //on upwards slope
        if (currentStep_sensor2 == 2) {
          cwInterval_sensor2[0] = cwInterval_sensor2[1] = TRUE_NORTH;
          ccwInterval_sensor2[0] = currentInterval_sensor2[1];
          ccwInterval_sensor2[1] = ccwInterval_sensor2[0] + INTERVAL_SIZE;
        }
        else if (currentStep_sensor2 == RESOLUTION_MULTIPLIER) {
          cwInterval_sensor2[1] = currentInterval_sensor2[0];
          cwInterval_sensor2[0] = cwInterval_sensor2[1] - INTERVAL_SIZE;
          ccwInterval_sensor2[0] = ccwInterval_sensor2[1] = TRUE_SOUTH;
        }
        else {
          cwInterval_sensor2[1] = currentInterval_sensor2[0];
          cwInterval_sensor2[0] = cwInterval_sensor2[1] - INTERVAL_SIZE;
          ccwInterval_sensor2[0] = currentInterval_sensor2[1];
          ccwInterval_sensor2[1] = ccwInterval_sensor2[0] + INTERVAL_SIZE;
        }
      }
      else { //on downwards slope
        if (currentStep_sensor2 == 2) {
          ccwInterval_sensor2[0] = ccwInterval_sensor2[1] = TRUE_NORTH;
          cwInterval_sensor2[0] = currentInterval_sensor2[1];
          cwInterval_sensor2[1] = cwInterval_sensor2[0] + INTERVAL_SIZE;
        }
        else if (currentStep_sensor2 == RESOLUTION_MULTIPLIER) {
          ccwInterval_sensor2[1] = currentInterval_sensor2[0];
          ccwInterval_sensor2[0] = ccwInterval_sensor2[1] - INTERVAL_SIZE;
          cwInterval_sensor2[0] = cwInterval_sensor2[1] = TRUE_SOUTH;
        }
        else {
          ccwInterval_sensor2[1] = currentInterval_sensor2[0];
          ccwInterval_sensor2[0] = ccwInterval_sensor2[1] - INTERVAL_SIZE;
          cwInterval_sensor2[0] = currentInterval_sensor2[1];
          cwInterval_sensor2[1] = cwInterval_sensor2[0] + INTERVAL_SIZE;
        }
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Incremental_Encoder::getCurrentSensorValue(int sensorNum) const
{
  if (sensorNum == 1) {
    return currValue_sensor1;
  }
  else if (sensorNum == 2) {
    return currValue_sensor2;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float Incremental_Encoder::getPosition() const
{
  return position;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Incremental_Encoder::filterCalc(int arr[FILTER_SIZE])
{
  int sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += arr[i];
  }

  return sum/FILTER_SIZE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::updateEncoderCount()
{
  /*
  //condition 1
  bool sensor1_changed_c1 = (currentInterval_sensor1[0] == TRUE_SOUTH && currValue_sensor1 < TRUE_SOUTH);
  sensor1_changed_c1 |= (currentInterval_sensor1[1] == TRUE_NORTH && currValue_sensor1 > TRUE_NORTH);
  bool sensor2_changed_c1 |= (currentInterval_sensor2[0] == TRUE_SOUTH && currValue_sensor2 < TRUE_SOUTH);
  sensor2_changed_c1 |= (currentInterval_sensor2[0] == TRUE_NORTH && currValue_sensor2 < TRUE_NORTH);
  bool positionChanged_c1 = sensor1_changed && sensor2_changed;

  //condition 2
  bool sensor1_changed_c2 = !(currValue_sensor1 > currentInterval_sensor1[0] && currValue_sensor1 < currentInterval_sensor1[1]);
  bool sensor2_changed_c2 = !(currValue_sensor2 > currentInterval_sensor2[0] && currValue_sensor2 < currentInterval_sensor2[1]);
  bool positionChanged_c2 = sensor1_changed_c2 && sensor2_changed_c2;

  if (positionChanged_c1) { 
    if (positionChanged_c2) {
  */

  //if sensor1 step is reasonably in the middle: 
  if ((RESOLUTION_MULTIPLIER+1) / 4 > abs(currentStep_sensor1 - (RESOLUTION_MULTIPLIER+1) / 2)) {
    bool clockwise = (cwInterval_sensor1[0] == TRUE_SOUTH && currValue_sensor1 > TRUE_SOUTH);
    clockwise |= (cwInterval_sensor1[1] == TRUE_NORTH && currValue_sensor1 < TRUE_NORTH);
    clockwise |= currValue_sensor1 > cwInterval_sensor1[0] && currValue_sensor1 < cwInterval_sensor1[1];

    bool counterClockwise = (ccwInterval_sensor1[0] == TRUE_SOUTH && currValue_sensor1 > TRUE_SOUTH);
    counterClockwise |= (ccwInterval_sensor1[1] == TRUE_NORTH && currValue_sensor1 < TRUE_NORTH);
    counterClockwise |= currValue_sensor1 > ccwInterval_sensor1[0] && currValue_sensor1 < ccwInterval_sensor1[1];

    if (clockwise) {
      change_cwInterval(1);
      change_cwInterval(2);
      count --;
    }
    else if (counterClockwise) {
      change_ccwInterval(1);
      change_ccwInterval(2);
      count ++;
    }
  }
  //else check sensor2 for the direction
  else {
    bool clockwise = (cwInterval_sensor2[0] == TRUE_SOUTH && currValue_sensor2 > TRUE_SOUTH);
    clockwise |= (cwInterval_sensor2[1] == TRUE_NORTH && currValue_sensor2 < TRUE_NORTH);
    clockwise |= currValue_sensor2 > cwInterval_sensor2[0] && currValue_sensor2 < cwInterval_sensor2[1];

    bool counterClockwise = (ccwInterval_sensor2[0] == TRUE_SOUTH && currValue_sensor2 > TRUE_SOUTH);
    counterClockwise |= (ccwInterval_sensor2[1] == TRUE_NORTH && currValue_sensor2 < TRUE_NORTH);
    counterClockwise |= currValue_sensor2 > ccwInterval_sensor2[0] && currValue_sensor2 < ccwInterval_sensor2[1];

    if (clockwise) {
      change_cwInterval(1);
      change_cwInterval(2);
      count --;
    }
    else if (counterClockwise) {
      change_ccwInterval(1);
      change_ccwInterval(2);
      count ++;
    }
  }


  //  }
  //}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::updateEncoderPosition()
{
  const int TOTAL_STEPS = RESOLUTION_MULTIPLIER * 28;
  
  position = 1.0*count/TOTAL_STEPS*360;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::updateSensorValues()
{
  filterArray_sensor1[filterIndex] = analogRead(sensorPin1);
  filterArray_sensor2[filterIndex] = analogRead(sensorPin2);

  currValue_sensor1 = filterCalc(filterArray_sensor1);
  currValue_sensor2 = filterCalc(filterArray_sensor2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::updateAllEncoders()
{
  for (int e = 0; e < numEncoders; e++) {
    Incremental_Encoder* enc = encodersArray[e];

    enc->updateSensorValues();
    enc->updateEncoderCount();
    enc->updateEncoderPosition();
  }

  filterIndex = (filterIndex + 1) % FILTER_SIZE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::setPosition(float pos)
{
  count = 0;
  position = pos;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Incremental_Encoder::limitSwitchPressed()
{
 return digitalRead(limitSwitchPin) == LOW;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::change_cwInterval(int sensor)
{
  //sensor1
  if (sensor == 1) {
    bool increasing = currentInterval_sensor1[0] < cwInterval_sensor1[0]; //clockwise interval is higher than current interval

    ccwInterval_sensor1[0] = currentInterval_sensor1[0];
    ccwInterval_sensor1[1] = currentInterval_sensor1[1];

    currentInterval_sensor1[0] = cwInterval_sensor1[0];
    currentInterval_sensor1[1] = cwInterval_sensor1[1];

    if (cwInterval_sensor1[0] == TRUE_NORTH && cwInterval_sensor1[1] == TRUE_NORTH) {
      cwInterval_sensor1[0] = ccwInterval_sensor1[0];
      cwInterval_sensor1[1] = ccwInterval_sensor1[1];
      currentStep_sensor1 = 1;
    }
    else if (cwInterval_sensor1[0] == TRUE_SOUTH && cwInterval_sensor1[1] == TRUE_SOUTH) {
      cwInterval_sensor1[0] = ccwInterval_sensor1[0];
      cwInterval_sensor1[1] = ccwInterval_sensor1[1];
      currentStep_sensor1 = RESOLUTION_MULTIPLIER + 1;
    }
    else if (1 > abs(cwInterval_sensor1[0] - TRUE_NORTH) && ccwInterval_sensor1[0] > cwInterval_sensor1[0]) {
      cwInterval_sensor1[0] = TRUE_NORTH;
      cwInterval_sensor1[1] = TRUE_NORTH;
      currentStep_sensor1 = 2;
    }
    else if (1 > abs(cwInterval_sensor1[1] - TRUE_SOUTH) && ccwInterval_sensor1[0] < cwInterval_sensor1[0]) {
      cwInterval_sensor1[0] = TRUE_SOUTH;
      cwInterval_sensor1[1] = TRUE_SOUTH;
      currentStep_sensor1 = RESOLUTION_MULTIPLIER;
    }
    else {
      if (increasing || (cwInterval_sensor1[0] == TRUE_NORTH && ccwInterval_sensor1[0] == TRUE_NORTH)) {
        cwInterval_sensor1[0] += INTERVAL_SIZE;
        cwInterval_sensor1[1] += INTERVAL_SIZE;
        currentStep_sensor1 ++;
      }
      else if (!increasing || (cwInterval_sensor1[1] == TRUE_SOUTH && ccwInterval_sensor1[1] == TRUE_SOUTH)) {
        cwInterval_sensor1[0] -= INTERVAL_SIZE;
        cwInterval_sensor1[1] -= INTERVAL_SIZE;
        currentStep_sensor1 --;
      }
    }
  }
  //sensor2
  else {
    bool increasing = currentInterval_sensor2[0] < cwInterval_sensor2[0]; //clockwise interval is higher than current interval

    ccwInterval_sensor2[0] = currentInterval_sensor2[0];
    ccwInterval_sensor2[1] = currentInterval_sensor2[1];

    currentInterval_sensor2[0] = cwInterval_sensor2[0];
    currentInterval_sensor2[1] = cwInterval_sensor2[1];

    if (cwInterval_sensor2[0] == TRUE_NORTH && cwInterval_sensor2[1] == TRUE_NORTH) {
      cwInterval_sensor2[0] = ccwInterval_sensor2[0];
      cwInterval_sensor2[1] = ccwInterval_sensor2[1];
      currentStep_sensor2 = 1;
    }
    else if (cwInterval_sensor2[0] == TRUE_SOUTH && cwInterval_sensor2[1] == TRUE_SOUTH) {
      cwInterval_sensor2[0] = ccwInterval_sensor2[0];
      cwInterval_sensor2[1] = ccwInterval_sensor2[1];
      currentStep_sensor2 = RESOLUTION_MULTIPLIER + 1;
    }
    else if (1 > abs(cwInterval_sensor2[0] - TRUE_NORTH) && ccwInterval_sensor2[0] > cwInterval_sensor2[0]) {
      cwInterval_sensor2[0] = TRUE_NORTH;
      cwInterval_sensor2[1] = TRUE_NORTH;
      currentStep_sensor2 = 2;
    }
    else if (1 > abs(cwInterval_sensor2[1] - TRUE_SOUTH) && ccwInterval_sensor2[0] < cwInterval_sensor2[0]) {
      cwInterval_sensor2[0] = TRUE_SOUTH;
      cwInterval_sensor2[1] = TRUE_SOUTH;
      currentStep_sensor2 = RESOLUTION_MULTIPLIER;
    }
    else {
      if (increasing || (cwInterval_sensor2[0] == TRUE_NORTH && ccwInterval_sensor2[0] == TRUE_NORTH)) {
        cwInterval_sensor2[0] += INTERVAL_SIZE;
        cwInterval_sensor2[1] += INTERVAL_SIZE;
        currentStep_sensor2 ++;
      }
      else if (!increasing || (cwInterval_sensor2[1] == TRUE_SOUTH && ccwInterval_sensor2[1] == TRUE_SOUTH)) {
        cwInterval_sensor2[0] -= INTERVAL_SIZE;
        cwInterval_sensor2[1] -= INTERVAL_SIZE;
        currentStep_sensor2 --;
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Incremental_Encoder::change_ccwInterval(int sensor)
{
  if (sensor == 1) {
    bool increasing = currentInterval_sensor1[0] < ccwInterval_sensor1[0];

    cwInterval_sensor1[0] = currentInterval_sensor1[0];
    cwInterval_sensor1[1] = currentInterval_sensor1[1];

    currentInterval_sensor1[0] = ccwInterval_sensor1[0];
    currentInterval_sensor1[1] = ccwInterval_sensor1[1];

    if (ccwInterval_sensor1[0] == TRUE_NORTH && ccwInterval_sensor1[1] == TRUE_NORTH) {
      ccwInterval_sensor1[0] = cwInterval_sensor1[0];
      ccwInterval_sensor1[1] = cwInterval_sensor1[1];
      currentStep_sensor1 = 1;
    }
    else if (ccwInterval_sensor1[0] == TRUE_SOUTH && ccwInterval_sensor1[1] == TRUE_SOUTH) {
      ccwInterval_sensor1[0] = cwInterval_sensor1[0];
      ccwInterval_sensor1[1] = cwInterval_sensor1[1];
      currentStep_sensor1 = RESOLUTION_MULTIPLIER + 1;
    }
    else if (1 > abs(ccwInterval_sensor1[0] - TRUE_NORTH) && ccwInterval_sensor1[0] < cwInterval_sensor1[0]) {
      ccwInterval_sensor1[0] = TRUE_NORTH;
      ccwInterval_sensor1[1] = TRUE_NORTH;
      currentStep_sensor1 = 2;
    }
    else if (1 > abs(ccwInterval_sensor1[1] - TRUE_SOUTH) && ccwInterval_sensor1[0] > cwInterval_sensor1[0]) {
      ccwInterval_sensor1[0] = TRUE_SOUTH;
      ccwInterval_sensor1[1] = TRUE_SOUTH;
      currentStep_sensor1 = RESOLUTION_MULTIPLIER;
    }
    else {
      if (increasing || (ccwInterval_sensor1[0] == TRUE_NORTH && cwInterval_sensor1[0] == TRUE_NORTH)) {
        ccwInterval_sensor1[0] += INTERVAL_SIZE;
        ccwInterval_sensor1[1] += INTERVAL_SIZE;
        currentStep_sensor1 ++;
      }
      else if (!increasing || (ccwInterval_sensor1[1] == TRUE_SOUTH && cwInterval_sensor1[1] == TRUE_SOUTH)) {
        ccwInterval_sensor1[0] -= INTERVAL_SIZE;
        ccwInterval_sensor1[1] -= INTERVAL_SIZE;
        currentStep_sensor1 --;
      }
    }
  }
  else {
    bool increasing = currentInterval_sensor2[0] < ccwInterval_sensor2[0];

    cwInterval_sensor2[0] = currentInterval_sensor2[0];
    cwInterval_sensor2[1] = currentInterval_sensor2[1];

    currentInterval_sensor2[0] = ccwInterval_sensor2[0];
    currentInterval_sensor2[1] = ccwInterval_sensor2[1];

    if (ccwInterval_sensor2[0] == TRUE_NORTH && ccwInterval_sensor2[1] == TRUE_NORTH) {
      ccwInterval_sensor2[0] = cwInterval_sensor2[0];
      ccwInterval_sensor2[1] = cwInterval_sensor2[1];
      currentStep_sensor2 = 1;
    }
    else if (ccwInterval_sensor2[0] == TRUE_SOUTH && ccwInterval_sensor2[1] == TRUE_SOUTH) {
      ccwInterval_sensor2[0] = cwInterval_sensor2[0];
      ccwInterval_sensor2[1] = cwInterval_sensor2[1];
      currentStep_sensor2 = RESOLUTION_MULTIPLIER + 1;
    }
    else if (1 > abs(ccwInterval_sensor2[0] - TRUE_NORTH) && ccwInterval_sensor2[0] < cwInterval_sensor2[0]) {
      ccwInterval_sensor2[0] = TRUE_NORTH;
      ccwInterval_sensor2[1] = TRUE_NORTH;
      currentStep_sensor2 = 2;
    }
    else if (1 > abs(ccwInterval_sensor2[1] - TRUE_SOUTH) && ccwInterval_sensor2[0] > cwInterval_sensor2[0]) {
      ccwInterval_sensor2[0] = TRUE_SOUTH;
      ccwInterval_sensor2[1] = TRUE_SOUTH;
      currentStep_sensor2 = RESOLUTION_MULTIPLIER;
    }
    else {
      if (increasing || (ccwInterval_sensor2[0] == TRUE_NORTH && cwInterval_sensor2[0] == TRUE_NORTH)) {
        ccwInterval_sensor2[0] += INTERVAL_SIZE;
        ccwInterval_sensor2[1] += INTERVAL_SIZE;
        currentStep_sensor2 ++;
      }
      else if (!increasing || (ccwInterval_sensor2[1] == TRUE_SOUTH && cwInterval_sensor2[1] == TRUE_SOUTH)) {
        ccwInterval_sensor2[0] -= INTERVAL_SIZE;
        ccwInterval_sensor2[1] -= INTERVAL_SIZE;
        currentStep_sensor2 --;
      }
    }
  }
}



