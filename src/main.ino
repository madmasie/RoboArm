#include <Encoder.h>
#include <Servo.h>

#include "robot_globals.h"
#include "servo_control.h"
#include "user_interface.h"
#include "inverse_kinematics.h"

// --------- Define the shared objects here (match externs) ---------
Servo baseservo;
Servo arm1servo;
Servo arm2servo;
Servo gripservo;

State  state  = WAIT_CMD;
Target target = T_NONE;
String lineBuf;

Encoder myEnc(A6, A7);

//default angles
int baseAngle = 140;
int gripAngle = 0;

void setup() {
  baseservo.attach(BASESERVOPIN, 460, 2400);
  arm1servo.attach(ARM1SERVOPIN, 460, 2400);
  arm2servo.attach(ARM2SERVOPIN, 460, 2400);
  gripservo.attach(GRIPSERVOPIN, 460, 2400);
0+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Serial.begin(9600);

  // baseservo.writeMicroseconds(angleToMicroseconds(baseAngle));
  // delay(500);
  // arm1servo.writeMicroseconds(1500);
  // delay(500);
  // arm2servo.writeMicroseconds(1500);
  // delay(500);
  gripservo.writeMicroseconds(angleToMicroseconds(gripAngle));
  delay(500);
  gripservo.writeMicroseconds(angleToMicroseconds(120));
  delay(500);
  gripservo.writeMicroseconds(angleToMicroseconds(30));

  promptCmd();
}

void printAngles() {
  int us;
  double deg;

  us = baseservo.readMicroseconds();  deg = microsecondsToAngle(us);
  Serial.print("b: "); Serial.print(us); Serial.print(" us ("); Serial.print(deg, 1); Serial.println(" deg)");

  us = arm1servo.readMicroseconds();  deg = microsecondsToAngle(us);
  Serial.print("1: "); Serial.print(us); Serial.print(" us ("); Serial.print(deg, 1); Serial.println(" deg)");

  us = arm2servo.readMicroseconds();  deg = microsecondsToAngle(us);
  Serial.print("2: "); Serial.print(us); Serial.print(" us ("); Serial.print(deg, 1); Serial.println(" deg)");

  us = gripservo.readMicroseconds();  deg = microsecondsToAngle(us);
  Serial.print("g: "); Serial.print(us); Serial.print(" us ("); Serial.print(deg, 1); Serial.println(" deg)");
}



void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r') continue;

    if (c == 0x08 || c == 0x7F) {
      if (lineBuf.length() > 0) { lineBuf.remove(lineBuf.length() - 1); Serial.print("\b \b"); }
      continue;
    }
    if (c != '\n') { Serial.print(c); lineBuf += c; continue; }

    Serial.println();
    String line = lineBuf; line.trim();
    lineBuf = "";

    if (state == WAIT_CMD) {
      if (line == "?") { printAngles(); promptCmd(); return; }  
      if (line.equalsIgnoreCase("b")) { target = T_BASE; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line == "1")           { target = T_ARM1; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line == "2")           { target = T_ARM2; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line.equalsIgnoreCase("g")) { target = T_GRIP; state = WAIT_ANGLE; promptAngleFor(target); }
      else { Serial.println("Invalid target. Use b, 1, 2, g, or ?"); promptCmd(); }
      return;
}


    if (state == WAIT_ANGLE) {
      if (line.startsWith("deg") || line.startsWith("DEG")) {
        String rest = line.substring(3); rest.trim();
        double deg = rest.toFloat();
        int us = angleToMicroseconds(deg);
        applyAngle(target, us);
      } else {
        int val;
        if (!parseAngle(line, val)) {
          Serial.println("Invalid input. Enter microseconds (e.g., 1500) or 'deg 60.5'.");
          promptAngleFor(target);
          return;
        }
        //if the number looks too small to be µs, treat it as degrees
        int us = (val < 400) ? angleToMicroseconds((double)val) : val;
        applyAngle(target, us);
      }
      state = WAIT_CMD; target = T_NONE; promptCmd(); return;
    }

  }
}
