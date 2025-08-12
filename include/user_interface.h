#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "robot_globals.h"

void promptCmd();
void promptAngleFor(Target tgt);
bool parseAngle(const String& s, int& out);

#endif
