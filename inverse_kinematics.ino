#include <Servo.h>
#include "encoder.h"
#include "motor_control.h"
#include "D-H_Parameters.h"

bool isReachable = true;
void setupTimer1_10Hz();

//defining motor and sensor pins
const int INC_ENCODER_1_HALLPIN_1 = A1;
const int INC_ENCODER_1_HALLPIN_2 = A2;
const int LIMIT_SWITCH_1 = 7;
const int MOTOR_PIN_1 = 1;

const int ABS_ENCODER_2_PIN = A3;
const int MOTOR_PIN_2 = 2;

const int ABS_ENCODER_3_PIN = A6;
const int MOTOR_PIN_3 = 4;

const int INC_ENCODER_4_HALLPIN_1 = A4;
const int INC_ENCODER_4_HALLPIN_2 = A5;
const int LIMIT_SWITCH_4 = 8;
const int MOTOR_PIN_4 = 3;

const int ABS_ENCODER_5_PIN = A9;
const int MOTOR_PIN_5 = 6;

const int INC_ENCODER_6_HALLPIN_1 = A7;
const int INC_ENCODER_6_HALLPIN_2 = A8;
const int LIMIT_SWITCH_6 = 9;
const int MOTOR_PIN_6 = 5;

//end effector position
float alpha = 0; //yaw
float beta = 0; //pitch
float gamma = 0; //roll
const float X = 0;
const float Y = 0;
const float Z = 0;

//joint rotation array
float* joint_rotations[6] = {&t1, &t2[0], &t3, &t4[0], &t5[0], &t6};
//motor array
Motor_Control* motors[6];

void setup() {
  Serial.begin(9600);

  //setting pinmode of limit switches
  pinMode (LIMIT_SWITCH_1, INPUT_PULLUP);
  pinMode (LIMIT_SWITCH_4, INPUT_PULLUP);
  pinMode (LIMIT_SWITCH_6, INPUT_PULLUP);
  

  alpha = alpha * PI / 180;
  beta = beta * PI / 180;
  gamma = gamma * PI / 180;

  const float end_eff_r_matrix[3][3] =
  {
    {cos(alpha)*cos(beta), cos(alpha)*sin(beta)*sin(gamma) - sin(alpha)*cos(gamma), cos(alpha)*sin(beta)*cos(gamma) + sin(alpha)*sin(gamma)},
    {sin(alpha)*cos(beta), sin(alpha)*sin(beta)*sin(gamma) + cos(alpha)*cos(gamma), sin(alpha)*sin(beta)*cos(gamma) - cos(alpha)*sin(gamma)},
    {-sin(beta),   cos(beta)*sin(gamma),   cos(beta)*cos(gamma)}
  };

  //wrist position
  float wrist_X = X - d6*end_eff_r_matrix[1][3];
  float wrist_Y = Y - d6*end_eff_r_matrix[2][3];
  float wrist_Z = Z - d6*end_eff_r_matrix[3][3];

  //joint 1
  t1 = atan2(wrist_Y, wrist_X);
  float r = sqrt(pow(wrist_Y, 2) + pow(wrist_X, 2));
  float s = wrist_Z-d1;
  float c3 = (pow(r, 2) + pow(s, 2) - pow(a2, 2) - pow(d4, 2)) / (2*a2*d4);

  //joint 3
  t3 = atan2(sqrt(1 - pow(c3, 2)), c3);

  //joint 2
  t2[0] = asin(((a2 - d4*cos(t3))*s - d4*sin(t3)*r) / (pow(r,2) + pow(s, 2)));
  t2[1] = acos(((a2 + d4*cos(t3))*r + d4*sin(t3)*s) / (pow(r,2) + pow(s, 2))); 

  //joint 5
  t5[0] = atan2(sqrt(1 - pow(sin(t1)*end_eff_r_matrix[1][3] - cos(t1)*end_eff_r_matrix[2][3], 2)), sin(t1)*end_eff_r_matrix[1][3] - cos(t1)*end_eff_r_matrix[2][3]);
  t5[1] = atan2(-1 * sqrt(1 - pow(sin(t1)*end_eff_r_matrix[1][3] - cos(t1)*end_eff_r_matrix[2][3], 2)), sin(t1)*end_eff_r_matrix[1][3] - cos(t1)*end_eff_r_matrix[2][3]);

  //note, t4[0] is linked to t2[0]; as is t4[1] and t2[1]
  t4[0] = atan2(-cos(t1)*sin(t2[0]+t3)*end_eff_r_matrix[1][3] - sin(t1)*sin(t2[0]+t3)*end_eff_r_matrix[2][3] + cos(t2[0]+t3)*end_eff_r_matrix[2][3], 
             cos(t1)*cos(t2[0]+t3)*end_eff_r_matrix[1][3] + sin(t1)*cos(t2[0]+t3)*end_eff_r_matrix[2][3] + sin(t2[0]+t3)*end_eff_r_matrix[3][3]);
  t4[1] = atan2(-cos(t1)*sin(t2[1]+t3)*end_eff_r_matrix[1][3] - sin(t1)*sin(t2[1]+t3)*end_eff_r_matrix[2][3] + cos(t2[1]+t3)*end_eff_r_matrix[2][3], 
             cos(t1)*cos(t2[1]+t3)*end_eff_r_matrix[1][3] + sin(t1)*cos(t2[1]+t3)*end_eff_r_matrix[2][3] + sin(t2[1]+t3)*end_eff_r_matrix[3][3]);
  
  //joint 6
  t6 = atan2(sin(t1)*end_eff_r_matrix[1][2] - cos(t1)*end_eff_r_matrix[2][2], -sin(t1)*end_eff_r_matrix[1][1] + cos(t1)*end_eff_r_matrix[2][2]);

  //shifting angles to be between 0, 360 degrees
  t1 += PI/2;
  t2[0] += PI/2;
  t2[1] += PI/2;
  t3 += PI/2;
  t4[0] += PI/2;
  t4[1] += PI/2;
  t5[0] += PI/2 + PI/4;
  t5[1] += PI/2 + PI/4;
  t6 += PI/2;
  
  Encoder inc_encoder1(INC_ENCODER_1_HALLPIN_1, INC_ENCODER_1_HALLPIN_2, LIMIT_SWITCH_1);
  Motor_Control motor1(MOTOR_PIN_1, &inc_encoder1, 0.0, 359.99);
  Encoder abs_encoder2(ABS_ENCODER_2_PIN);
  Motor_Control motor2(MOTOR_PIN_2, &abs_encoder2, 120.0, 240.0);
  Encoder abs_encoder3(ABS_ENCODER_2_PIN);
  Motor_Control motor3(MOTOR_PIN_3, &abs_encoder3, 80.0, 280.0);
  Encoder inc_encoder4(INC_ENCODER_4_HALLPIN_1, INC_ENCODER_4_HALLPIN_2, LIMIT_SWITCH_4);
  Motor_Control motor4(MOTOR_PIN_1, &inc_encoder4, 0.0, 359.99);
  Encoder abs_encoder5(ABS_ENCODER_5_PIN);
  Motor_Control motor5(MOTOR_PIN_5, &abs_encoder5, 80.0, 280.0);
  Encoder inc_encoder6(INC_ENCODER_6_HALLPIN_1, INC_ENCODER_6_HALLPIN_2, LIMIT_SWITCH_6);
  Motor_Control motor6(MOTOR_PIN_6, &inc_encoder6, 0.0, 359.99);

  //populate motors array
  motors[0] = &motor1;
  motors[1] = &motor2;
  motors[2] = &motor3;
  motors[3] = &motor4;
  motors[4] = &motor5;
  motors[5] = &motor6;

  for (int i=0; i<6; i++)
  {
    if (i == 1 || i == 4) //joints 2,4,5
    {
      if (!motors[i]->inRange(joint_rotations[i][0])){
        if (!motors[i]->inRange(joint_rotations[i][1])){
          isReachable = false;
        }
        else{
          joint_rotations[i][2] = joint_rotations[i][1];

          if (i == 2) //if joint 2 is valid: change corresponding joint 4
          {
            joint_rotations[3][2] = joint_rotations[3][1];
          }
        }
      }
      else{
        joint_rotations[i][2] = joint_rotations[i][0];

        if (i == 2) //if joint 2 is valid: change corresponding joint 4
          {
            joint_rotations[3][2] = joint_rotations[3][0];
          }
      }
    }
    else if (i == 0 || i == 2 || i == 5) //joints 1,3,6
    {
      if (!motors[i]->inRange(*joint_rotations[i])){
        isReachable = false;
      }
    }
  }

  //lever joints move to zero
  motor2.moveTo(180);
  motor3.moveTo(180);
  motor5.moveTo(180);

  //set zero position of revolute joints
  motor1.setZero();
  motor4.setZero();
  motor6.setZero();

  //set up position printing interrupts
  setupTimer1_10Hz();

  //check if position is reachable
  if (isReachable) {
    Encoder::configureEncoders();
    motor1.moveTo(t1);
    motor2.moveTo(t2[2]);
    motor3.moveTo(t3);
    motor4.moveTo(t4[2]);
    motor5.moveTo(t5[2]);
    motor6.moveTo(t6);
  }
  else {
    Serial.println("position not reachable"); 
  }

}

void setupTimer1_10Hz() {
  cli();

  TCCR0B = 0;
  // Set CTC mode
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10); // Set prescaler to 64

  TCNT1 = 0;
  OCR1B = 24999; // Set compare value for 10Hz
  TIMSK1 |= (1 << OCIE1B); // Enable compare interrupt

  sei();
}

//print motor positions
ISR(TIMER1_COMPB_vect)
{
  for (int i=0; i<6; i++)
  {
    Serial.print("Motor ");
    Serial.print(i+1);
    Serial.print(" position: ");
    Serial.println(motors[i]->getPosition());
  }
}

void loop() {

  
}
