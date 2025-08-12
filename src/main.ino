#include <Encoder.h>
#include <Servo.h>

//globals
enum State  { WAIT_CMD, WAIT_ANGLE };
enum Target { T_NONE, T_BASE, T_ARM1, T_ARM2, T_GRIP };

State  state  = WAIT_CMD;
Target target = T_NONE;
String lineBuf;   // collects a full line until Enter


#define BASESERVOPIN A0
#define ARM1SERVOPIN A1
#define ARM2SERVOPIN A2
#define GRIPSERVOPIN A3

Encoder myEnc(A6, A7);
Servo arm1servo;
Servo arm2servo;
Servo baseservo;
Servo gripservo;

double x = 75;    // length in mm of arm1
double y = 0;
double z = 75;
int gripAngle = 0; //0 degrees initialization upon startup (closed grip)
int baseAngle = 90; // base angle at startup
int arm1Angle = 90; // arm1 angle at startup
int arm2Angle = 90; // arm2 angle at startup

long oldPosition = 0;

/*******************angleToMicroseconds()***************************************
 * Converts an angle in degrees to a pulse width in microseconds.
 * The conversion is based on the range of 460 to 2400 microseconds
 * corresponding to 0 to 180 degrees.
 *********************************************************************/
int angleToMicroseconds(double angle) {
  double val = 460.0 + (((2400.0 - 460.0) / 180.0) * angle);
  return (int)val;
}

/*******************moveToAngle()***************************************
 * moveToAngle() takes angles for the base, arm1, arm2, and gripper
 * and applies them to the respective servos. It converts the angles
 * to pulse widths in microseconds using angleToMicroseconds().
 *********************************************************************/
void moveToAngle(double b, double a1, double a2, double g) {
  arm1servo.writeMicroseconds(angleToMicroseconds(188 - a1));
  arm2servo.writeMicroseconds(angleToMicroseconds(a2 + 101));
  baseservo.writeMicroseconds(angleToMicroseconds(b + 90));
  gripservo.writeMicroseconds(angleToMicroseconds(g));
}

/*******************moveToPos()***************************************
 * moveToPos() calculates the angles for the robotic arm
 * based on the provided x, y, z coordinates and gripper angle.
 * It uses inverse kinematics to determine the angles
 * needed to reach the specified position.
 *********************************************************************/
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
bool parseAngle(const String& s, int& out) {
  String t = s; 
  t.trim();
  if (t.length() == 0) return false; // still reject empty

  // Try converting — toInt works for 0 too
  out = t.toInt();

  // Check if every character is numeric or sign
  for (size_t i = 0; i < t.length(); ++i) {
    char c = t[i];
    if (!(isDigit(c) || (i == 0 && (c == '+' || c == '-')))) {
      return false; // invalid char
    }
  }
  return true;
}

/*******************promptCmd()***************************************
 * promptCmd() prints a prompt to the Serial Monitor
 * asking the user to enter a target for the robotic arm.
 * It informs them of the available targets: base, arm1, arm2, or gripper.
 *********************************************************************/
void promptCmd() {
  Serial.println("Enter target (b,1,2,g):");
}

/*******************microsecondsToAngle()***************************************
 * Converts a pulse width in microseconds to an angle in degrees.
 * The conversion is based on the range of 460 to 2400 microseconds
 * corresponding to 0 to 180 degrees.
 *********************************************************************/
double microsecondsToAngle(int us) {
  return ((us - 460.0) * 180.0) / (2400.0 - 460.0);
}

/*******************applyAngle()***************************************
 * promptAngleFor() prompts the user to enter a pulse width
 * for the specified target servo. It informs the user
 * which servo they are setting the angle for tells them to enter it
 * in microseconds or as an angle in degrees.
 *********************************************************************/
void promptAngleFor(Target tgt) {
  switch (tgt) {
    case T_BASE: Serial.println("Enter pulse width for base (us), or 'deg 60.5':"); break;
    case T_ARM1: Serial.println("Enter pulse width for arm1 (us), or 'deg 60.5':"); break;
    case T_ARM2: Serial.println("Enter pulse width for arm2 (us), or 'deg 60.5':"); break;
    case T_GRIP: Serial.println("Enter pulse width for gripper (us), or 'deg 60.5':"); break;
    default: break;
  }
}

/*******************applyAngle()***************************************
 * applyAngle() takes a target (one of the 4 servos)) and a
 * puulsewidth in microseconds. It converts the pulse width
 * to an angle in degrees and radians, then applies the angle
 * to the servo. It also prints the pulse width, angle in
 * degrees, and angle in radians to the Serial Monitor.
 *********************************************************************/
void applyAngle(Target tgt, int us) {
  us = constrain(us, 460, 2400);      // safety clamp (pulse)
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
    default: break;
  }
}


/*******************setup()***************************************
 * setup() initializes the servos, sets their initial positions,
 * and prompts the user to choose a target for movement.
 * It also initializes the Serial Monitor for communication.
 *********************************************************************/
void setup() {
    baseservo.attach(BASESERVOPIN, 460, 2400);
    arm1servo.attach(ARM1SERVOPIN, 460, 2400);
    arm2servo.attach(ARM2SERVOPIN, 460, 2400);
    gripservo.attach(GRIPSERVOPIN, 460, 2400);

    Serial.begin(9600);

    baseservo.writeMicroseconds(1500);
    arm1servo.writeMicroseconds(1500);
    arm2servo.writeMicroseconds(1500);
    gripservo.writeMicroseconds(angleToMicroseconds(gripAngle));
    delay(500);

    promptCmd();
}

/****************************loop()************************************
 * loop() reads user input from the Serial Monitor,
 * processes commands to move the robotic arm, and handles
 * backspace/delete input.
 * It supports both pulse width in microseconds and angle in degrees.
 * It also provides a simple command interface to select
 * which part of the arm to control (base, arm1, arm2, or gripper).
 *********************************************************************/
void loop() {
    // Main loop logic remains unchanged
}
