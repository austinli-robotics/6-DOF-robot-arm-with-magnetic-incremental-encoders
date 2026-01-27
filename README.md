# 6-DOF-robotarm-with-magnetic-incremental-encodering

<img width="488" height="524" alt="Screenshot 2026-01-15 004144" src="https://github.com/user-attachments/assets/473ef59f-daf9-4620-9cc7-9af7a8b130f9" /> <img width="373" height="524" alt="Screenshot 2026-01-15 004026" src="https://github.com/user-attachments/assets/863d2e3c-1e07-4310-9828-0b1b67ae91f2" />



This build is a fully custom 6 DOF robot arm designed with custom BLDC motors actuators and a combination of absolute and incremental magnetic encoders.

Everything was coded in C++ for compatibility with an Arduino Mega 2560 board.

Incremental encoders were designed using KY-035 Hall Effect modules and a ring of alternating polarity magnets integrated into the rotating joint. This method leads to a functional resolution of approx. 4000 steps/rotation.

The design/layout of the joints and arm use a sphereical wrist, primarily used to simplify the inverse kinematics but also has the added benifit of creating channels for wires to pass through without being seen. The inverse kinematic equations have been incorperated into inverse_kinematics.ino.
