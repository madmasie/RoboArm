#include "inverse_kinematics.h"
#include "servo_control.h"

void moveToAngle(double b, double a1, double a2, double g) {
    arm1servo.writeMicroseconds(angleToMicroseconds(188 - a1));
    arm2servo.writeMicroseconds(angleToMicroseconds(a2 + 101));
    baseservo.writeMicroseconds(angleToMicroseconds(b + 90));
    gripservo.writeMicroseconds(angleToMicroseconds(g));
}

void moveToPos(double x, double y, double z, double g) {
    double b = atan2(y, x) * (180 / 3.1415); // base angle
    double l = sqrt(x * x + y * y); // x and y extension
    double h = sqrt(l * l + z * z);
    double phi = atan(z / l) * (180 / 3.1415);
    double theta = acos((h / 2) / 75) * (180 / 3.1415);
    double a1 = phi + theta; // angle for first part of the arm
    double a2 = phi - theta; // angle for second part of the arm

    moveToAngle(b, a1, a2, g);
}