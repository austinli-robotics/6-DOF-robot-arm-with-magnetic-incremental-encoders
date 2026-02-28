#ifndef INC_ENCODER
#define INC_ENCODER
#include <Arduino.h>

class Incremental_Encoder
{
  public:
    Incremental_Encoder(const int, const int, const int);

    static void updateAllEncoders();
    int getCurrentSensorValue(int) const;
    float getPosition() const;
    void setPosition (float);
    bool limitSwitchPressed();

  private:
    int sensorPin1, sensorPin2, limitSwitchPin;
    int currValue_sensor1, currValue_sensor2;
    float cwInterval_sensor1[2], ccwInterval_sensor1[2], currentInterval_sensor1[2];  //interval of next steps
    float cwInterval_sensor2[2], ccwInterval_sensor2[2], currentInterval_sensor2[2];  //inclusive first value, exclusive second value
    static const int RESOLUTION_MULTIPLIER = 8; //total resolution = 28 * multiplier
    static const int MAX_NORTH = 170, MAX_SOUTH = 860;
    static inline constexpr float INTERVAL_SIZE = 1.0*(MAX_SOUTH - MAX_NORTH) / RESOLUTION_MULTIPLIER;
    static inline constexpr float TRUE_NORTH = MAX_NORTH + (INTERVAL_SIZE / 2);
    static inline constexpr float TRUE_SOUTH = MAX_SOUTH - (INTERVAL_SIZE / 2);
    int currentStep_sensor1; //keeps track of general location of sensor
                             //1 = true north -> (resolution multiplier + 1) = true south
    int currentStep_sensor2;
    int count;

    void change_cwInterval(int sensor); //clockwise (negative)
    void change_ccwInterval(int sensor); //counter-clockwise (positive)

    float position;
    void updateEncoderCount();
    void updateEncoderPosition();
    void updateSensorValues();

    static inline int filterIndex = 0;
    static const int FILTER_SIZE = 16;
    int filterArray_sensor1[FILTER_SIZE], filterArray_sensor2[FILTER_SIZE];
    int filterCalc(int arr[FILTER_SIZE]);

    static const int MAX_ENCODERS = 3;
    static inline Incremental_Encoder* encodersArray[MAX_ENCODERS] = {nullptr};
    static inline int numEncoders = 0;
    void registerEncoder();

};




#endif