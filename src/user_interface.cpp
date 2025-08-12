#include "user_interface.h"

void promptCmd() {
  Serial.println("Enter target (b,1,2,g):");
}

void promptAngleFor(Target tgt) {
  switch (tgt) {
    case T_BASE: Serial.println("Enter pulse width for base (us), or 'deg 60.5':"); break;
    case T_ARM1: Serial.println("Enter pulse width for arm1 (us), or 'deg 60.5':"); break;
    case T_ARM2: Serial.println("Enter pulse width for arm2 (us), or 'deg 60.5':"); break;
    case T_GRIP: Serial.println("Enter pulse width for gripper (us), or 'deg 60.5':"); break;
    default: break;
  }
}

bool parseAngle(const String& s, int& out) {
  String t = s; 
  t.trim();
  if (t.length() == 0) return false;
  out = t.toInt();
  for (size_t i = 0; i < t.length(); ++i) {
    char c = t[i];
    if (!(isDigit(c) || (i == 0 && (c == '+' || c == '-')))) return false;
  }
  return true;
}
