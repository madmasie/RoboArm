#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

int angleToMicroseconds(double angle);
double microsecondsToAngle(int us);
#include "inverse_kinematics.ino" // For Target enum
void applyAngle(Target tgt, int us);

#endif // SERVO_CONTROL_H