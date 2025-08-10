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
int gripAngle = 67; //calibrated grip angle?

long oldPosition = 0;

int angleToMicroseconds(double angle) {
  double val = 460.0 + (((2400.0 - 460.0) / 180.0) * angle);
  return (int)val;
}

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
// Echo-friendly angle parser
bool parseAngle(const String& s, int& out) {
  String t = s; t.trim();
  if (t.length() == 0) return false;
  // ensure it looks numeric (allows leading + / -)
  for (size_t i = 0; i < t.length(); ++i) {
    char c = t[i];
    if (!(isDigit(c) || c=='+' || c=='-' )) return false;
    if ((c=='+' || c=='-') && i != 0) return false;
  }
  out = t.toInt();
  return true;
}

void promptCmd() {
  Serial.println("Enter target (b,1,2,g):");
}

void promptAngleFor(Target tgt) {
  switch (tgt) {
    case T_BASE: Serial.println("Enter angle for base servo (b):"); break;
    case T_ARM1: Serial.println("Enter angle for arm1 servo (1):"); break;
    case T_ARM2: Serial.println("Enter angle for arm2 servo (2):"); break;
    case T_GRIP: Serial.println("Enter angle for grip servo (g):"); break;
    default: break;
  }
}

void applyAngle(Target tgt, int angle) {
  // Optional: clamp to safe range, e.g. 0–180
  // angle = constrain(angle, 0, 180);

  switch (tgt) {
    case T_BASE:
      baseservo.writeMicroseconds(angleToMicroseconds(angle));
      Serial.print("b set to "); Serial.println(angle);
      break;
    case T_ARM1:
      arm1servo.writeMicroseconds(angleToMicroseconds(angle));
      Serial.print("1 set to "); Serial.println(angle);
      break;
    case T_ARM2:
      arm2servo.writeMicroseconds(angleToMicroseconds(angle));
      Serial.print("2 set to "); Serial.println(angle);
      break;
    case T_GRIP:
      gripservo.writeMicroseconds(angleToMicroseconds(angle));
      Serial.print("g set to "); Serial.println(angle);
      break;
    default: break;
  }
}


void setup() {
  // put your setup code here, to run once:

  baseservo.attach(BASESERVOPIN, 460, 2400); // min and max in ms
  arm1servo.attach(ARM1SERVOPIN, 460, 2400);
  arm2servo.attach(ARM2SERVOPIN, 460, 2400);
  gripservo.attach(GRIPSERVOPIN, 460, 2400);

  Serial.begin(9600);

  moveToPos(x, y, z, gripAngle);
  promptCmd();   // <-- start in "choose target" mode

}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    // Handle CRLF gracefully: ignore '\r'
    if (c == '\r') continue;

    // Simple backspace support (optional but handy)
    if (c == 0x08 || c == 0x7F) { // BS or DEL
      if (lineBuf.length() > 0) {
        lineBuf.remove(lineBuf.length() - 1);
        Serial.print("\b \b"); // erase last char on Serial Monitor
      }
      continue;
    }

    // Build line + echo as user types
    if (c != '\n') {
      Serial.print(c);
      lineBuf += c;
      continue;
    }

    // Reached end of line (user pressed Enter)
    Serial.println();  // move to next line for neat UI
    String line = lineBuf; line.trim();
    lineBuf = "";

    if (state == WAIT_CMD) {
      if (line.equalsIgnoreCase("b")) { target = T_BASE; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line == "1")           { target = T_ARM1; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line == "2")           { target = T_ARM2; state = WAIT_ANGLE; promptAngleFor(target); }
      else if (line.equalsIgnoreCase("g")) { target = T_GRIP; state = WAIT_ANGLE; promptAngleFor(target); }
      else {
        Serial.println("Invalid target. Use b, 1, 2, or g.");
        promptCmd();
      }
      return;
    }

    if (state == WAIT_ANGLE) {
      int angle;
      if (!parseAngle(line, angle)) {
        Serial.println("Invalid angle. Enter a number (e.g., 90).");
        promptAngleFor(target);
        return;
      }
      applyAngle(target, angle);
      // bounce back to waiting-for-command state
      state = WAIT_CMD;
      target = T_NONE;
      promptCmd();
      return;
    }
  }
}
