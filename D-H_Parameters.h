//end effector position
float alpha = 0; //yaw
float beta = 0; //pitch
float gamma = 0; //roll
const float X = 0;
const float Y = 0;
const float Z = 0;

//D-H parameters of joint 1
const float d1 = 139.1;
const float a1 = 0;
const float e1 = PI/2; //alpha: angle about x-axis
float t1 = 0; //theta: angle about z-axis ; unknown parameter

//D-H parameters of joint 2
const float d2 = 0;
const float a2 = 329;
const float e2 = 0;
float t2[3] = {0,0,0}; //3rd element is the final/selected position

//D-H parameters of joint 3
const float d3 = 0;
const float a3 = 0;
const float e3 = -PI/2;
float t3 = 0;

//D-H parameters of joint 4
const float d4 = 316;
const float a4 = 0;
const float e4 = PI/2;
float t4[3] = {0,0,0}; //3rd element is the final/selected position

//D-H parameters of joint 5
const float d5 = 0;
const float a5 = 0;
const float e5 = PI/2;
float t5[3] = {0,0,0}; //+PI/4 ; 3rd element is the final/selected position

//D-H parameters of joint 6
const float d6 = 205;
const float a6 = 0;
const float e6 = 0;
float t6 = 0;