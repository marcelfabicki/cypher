#include <Arduino.h>
#include <ESP32Servo.h>
#include <cmath>

const float upperLegLength = 80.1;
const float lowerLegLength = 136.25;

const float pi = 3.14159;

const int upperServoError = 6;
const int lowerServoError = 3;

char selectionInput;

bool selectionText = false;

bool xInputGiven = false;

Servo lowerRightServo;
Servo upperRightServo;
Servo upperLeftServo;
Servo lowerLeftServo;

int x;
int y;


float kneeAngle(int xInput, int yInput) {
  float r = sqrt(sq(xInput) + sq(yInput));
  return 180 - acos((sq(upperLegLength) + sq(lowerLegLength) - sq(r)) / (2 * upperLegLength * lowerLegLength)) * 180 / pi;
}

float hipAngle(int xInput, int yInput) {
  float r = sqrt(sq(xInput) + sq(yInput));
  return (-atan2(yInput, xInput) - acos((sq(r) + sq(upperLegLength) - sq(lowerLegLength)) / (2 * r * upperLegLength))) * 180 / pi;
}

void writeServo (char servo, int angle) {
  if (servo == 'u') {
      upperLeftServo.write(constrain((180 - upperServoError) - angle, 90 , 180));
      upperRightServo.write(constrain(angle, 0, 90));
  }

  if (servo == 'l') {
      lowerLeftServo.write(constrain((180 - lowerServoError) - angle, 0 , 150));
      lowerRightServo.write(constrain(angle, 30, 180));
  }
  
}

void checkInput() {
  float m = sqrt(sq(x) + sq(y));
  if (m > (upperLegLength + lowerLegLength) || m < (lowerLegLength - upperLegLength) || y > 0) {
      Serial.println("Invalid point, please try again");
      return;
  }
  writeServo('u', hipAngle(x, y));
  writeServo('l', kneeAngle(x, y)); 
}


void yMovement() {
  x = -40;
  for (int i = 60; i <= 200; i = i + 2) {
      writeServo('u', hipAngle(x, -i));
      writeServo('l', kneeAngle(x, -i));
      delay(10);
  }
  for (int i = 200; i >= 60; i = i - 2) {
      writeServo('u', hipAngle(x, -i));
      writeServo('l', kneeAngle(x, -i));
      delay(10);
  }
}

void xMovement() {
  y = -120;
  for (int i = -130; i <= 40; i = i +2) {
    writeServo('u', hipAngle(i, y));
    writeServo('l', kneeAngle(i, y));
    delay(8);
  }
  for (int i = 40; i >= -130; i = i - 2) {
    writeServo('u', hipAngle(i, y));
    writeServo('l', kneeAngle(i, y));
    delay(8);
  }
}



void selection() {
  selectionText = true;

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() != 1 && selectionInput != 'p') {
      Serial.println("please enter a single character");
      return;
    }

    if (input == "r") {
      selectionText = false;
      xInputGiven = false;
      selectionInput = 0;
      return;
    }

    if (input == "y" || input == "x") {
      selectionInput = input[0];
      return;
    }

    if (input == "p") {
      Serial.println("Enter a value for x: ");
      selectionInput = 'p';
      return;
    }

    if (selectionInput == 'p') {
      for (int i = 0; i < input.length(); i++) {
        if (i == 0 && input[i] == '-' && input.length() > 1) {
          continue;
        }
        if (isDigit(input[i]) == false) {
          Serial.println("Please enter a valid integer");
          return;
        }
      }
      if (!xInputGiven) {
        x = input.toInt();
        Serial.println("Enter a value for y: ");
        xInputGiven = true;
        return;
      }
      if (xInputGiven) {
        y = input.toInt();
        Serial.println("Enter a value for x: ");
        xInputGiven = false;
        checkInput();
        return;
      }
    }

    else {
      Serial.println("Please enter a valid command");
    }
  }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    lowerLeftServo.attach(18);
    upperLeftServo.attach(19);

    lowerRightServo.attach(33);
    upperRightServo.attach(32);

    delay(500);
}

void loop() {
  if (selectionText == false) {
    Serial.println("p - x-y point input");
    Serial.println("y - y movement loop");
    Serial.println("x - x movement loop");
    Serial.println("r - restart");
  }
  
  selection();

  if (selectionInput == 'y') {
    yMovement();
    return;
  }

  else if (selectionInput == 'x') {
    xMovement();
  return;
  }

}