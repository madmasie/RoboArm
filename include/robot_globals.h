#ifndef ROBOT_GLOBALS_H
#define ROBOT_GLOBALS_H

#include <Arduino.h>
#include <Servo.h>

// -------- Pins --------
#define BASESERVOPIN A0
#define ARM1SERVOPIN A1
#define ARM2SERVOPIN A2
#define GRIPSERVOPIN A3

// Safe pulse range for BASE (tune later if needed)
const int BASE_MIN_US = 900;
const int BASE_MAX_US = 2100;



// -------- UI state / targets --------
enum State  { WAIT_CMD, WAIT_ANGLE };
enum Target { T_NONE, T_BASE, T_ARM1, T_ARM2, T_GRIP };

// -------- Shared objects (defined in the main .ino) --------
extern Servo baseservo;
extern Servo arm1servo;
extern Servo arm2servo;
extern Servo gripservo;

extern State  state;
extern Target target;
extern String lineBuf;

#endif
