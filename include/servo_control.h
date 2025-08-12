#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include "robot_globals.h"

int    angleToMicroseconds(double angle);
double microsecondsToAngle(int us);
void   applyAngle(Target tgt, int us);

#endif
