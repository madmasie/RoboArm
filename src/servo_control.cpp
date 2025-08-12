#include "servo_control.h"
#include "inverse_kinematics.ino" // For Target enum

int angleToMicroseconds(double angle) {
    double val = 460.0 + (((2400.0 - 460.0) / 180.0) * angle);
    return (int)val;
}

double microsecondsToAngle(int us) {
    return ((us - 460.0) * 180.0) / (2400.0 - 460.0);
}

void applyAngle(Target tgt, int us) {
    us = constrain(us, 460, 2400); // safety clamp (pulse)
    double angle = microsecondsToAngle(us);
    double radians = angle * (3.14159265358979323846 / 180.0);

    switch (tgt) {
        case T_BASE:
            baseservo.writeMicroseconds(us);
            Serial.print("b -> "); Serial.print(us); Serial.print(" us  (");
            Serial.print(angle, 2); Serial.print(" deg, ");
            Serial.print(radians, 4); Serial.println(" rad)");
            break;
        case T_ARM1:
            arm1servo.writeMicroseconds(us);
            Serial.print("1 -> "); Serial.print(us); Serial.print(" us  (");
            Serial.print(angle, 2); Serial.print(" deg, ");
            Serial.print(radians, 4); Serial.println(" rad)");
            break;
        case T_ARM2:
            arm2servo.writeMicroseconds(us);
            Serial.print("2 -> "); Serial.print(us); Serial.print(" us  (");
            Serial.print(angle, 2); Serial.print(" deg, ");
            Serial.print(radians, 4); Serial.println(" rad)");
            break;
        case T_GRIP:
            gripservo.writeMicroseconds(us);
            Serial.print("g -> "); Serial.print(us); Serial.print(" us  (");
            Serial.print(angle, 2); Serial.print(" deg, ");
            Serial.print(radians, 4); Serial.println(" rad)");
            break;
        default:
            break;
    }
}