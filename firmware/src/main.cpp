#include <Arduino.h>
#include <ESP32Servo.h>
#include <cmath>

const float upperLegLength = 80.1;
const float lowerLegLength = 136.25;

const float pi = 3.14159;

const int upperServoError = 6;
const int lowerServoError = 3;

bool xInputText = false;
bool yInputText = false;

int writeType = 1;

Servo lowerRightServo;
Servo upperRightServo;
Servo upperLeftServo;
Servo lowerLeftServo;

int x;
int y;


float kneeAngle(int xInput, int yInput) {
    float r = sqrt(sq(xInput) + sq(yInput));
    return acos((sq(upperLegLength) + sq(lowerLegLength) - sq(r)) / (2 * upperLegLength * lowerLegLength)) * 180 / pi;
}

float hipAngle(int xInput, int yInput) {
    float r = sqrt(sq(xInput) + sq(yInput));
    return (-atan2(y, x) - acos((sq(r) + sq(upperLegLength) - sq(lowerLegLength)) / (2 * r * upperLegLength))) * 180 / pi;
}

void writeServo (int servo, int angle) {
    if (servo == 1) {
        upperLeftServo.write(constrain((180 - upperServoError) - angle, 90 , 180));
        upperRightServo.write(constrain(angle, 0, 90));
    }

    if (servo == 2) {
        lowerLeftServo.write(constrain((180 - lowerServoError) - angle, 0 , 150));
        lowerRightServo.write(constrain(angle, 30, 180));
    }
  
}

void checkInput() {
    int m = sqrt(sq(x) + sq(y));
    if (m > (upperLegLength + lowerLegLength) || m < (lowerLegLength - upperLegLength) || y > 0) {
        Serial.println("Invalid coordinate, please try again");
        writeType = 1;
        return;
    }
    Serial.println("good input");
    writeServo(1, hipAngle(x, y));
    writeServo(2, 180 - kneeAngle(x, y));
    writeType = 1;

    
}

void xInput() {
    xInputText = true;
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int number = input.toInt();

        if (x != number) {
            writeType = 2;
            x = number;
            xInputText = false;
        }
    }
}

void yInput() {
    yInputText = true;
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int number = input.toInt();

        if (y != number) {
            y = number;
            yInputText = false;
            checkInput();
        }
    }
}



void yMovement() {
    for (int i = 60; i <= 200; i = i + 2) {
        y = -i;
        writeServo(1, hipAngle(x, y));
        writeServo(2, 180 - kneeAngle(x, y));
        delay(10);
    }
    for (int i = 200; i >= 60; i = i - 2) {
        y = -i;
        writeServo(1, hipAngle(x, y));
        writeServo(2, 180 - kneeAngle(x, y));
        delay(10);
    }
}

void xMovement() {

}

void circleMovement() {

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
    switch (writeType) {
        case 1:
            if (xInputText == false) {
                Serial.println("Enter a value for x: ");
            }
            xInput();
            break;
        case 2:
            if (yInputText == false) {
                Serial.println("Enter a value for y: ");
            }    
            yInput();
            break;
    }
}