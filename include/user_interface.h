#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <Arduino.h>
#include "inverse_kinematics.ino" // For Target enum

void promptCmd();
void promptAngleFor(Target tgt);
bool parseAngle(const String& s, int& out);

#endif // USER_INTERFACE_H