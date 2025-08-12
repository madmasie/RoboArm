#include "servo_control.h"
#include "robot_globals.h"

//datasheet pulse width range
//MG900S reads 500Hz control signal, 1 pulse every 20ms
const int US_MIN = 500;   // 0°
const int US_MAX = 2500;  // 180°

//takes an angle in degrees and converts it to microseconds
//460 = max pulse width (0 degrees)
//2400 = min pulse width (180 degrees)
int angleToMicroseconds(double angle) {
  return (int)(US_MIN + (US_MAX - US_MIN) * (angle / 180.0));
}

double microsecondsToAngle(int us) {
  return 180.0 * (us - US_MIN) / (US_MAX - US_MIN);
}

void applyAngle(Target tgt, int us) {
  us = constrain(us, 460, 2400);      // safety clamp, might be causeing issues?
  double deg = microsecondsToAngle(us);
  double rad = deg * (3.14159265358979323846 / 180.0);

  switch (tgt) {
    case T_BASE:
      baseservo.writeMicroseconds(us);
      Serial.print("b -> "); Serial.print(us); Serial.print(" us (");
      Serial.print(deg, 2); Serial.print(" deg, "); Serial.print(rad, 4); Serial.println(" rad)");
      break;
    case T_ARM1:
      arm1servo.writeMicroseconds(us);
      Serial.print("1 -> "); Serial.print(us); Serial.print(" us (");
      Serial.print(deg, 2); Serial.print(" deg, "); Serial.print(rad, 4); Serial.println(" rad)");
      break;
    case T_ARM2:
      arm2servo.writeMicroseconds(us);
      Serial.print("2 -> "); Serial.print(us); Serial.print(" us (");
      Serial.print(deg, 2); Serial.print(" deg, "); Serial.print(rad, 4); Serial.println(" rad)");
      break;
    case T_GRIP:
      gripservo.writeMicroseconds(us);
      Serial.print("g -> "); Serial.print(us); Serial.print(" us (");
      Serial.print(deg, 2); Serial.print(" deg, "); Serial.print(rad, 4); Serial.println(" rad)");
      break;
    default: break;
  }
}
