#include <Arduino.h>
#include <ESP32Servo.h>
#include <cmath>

const float upperLegLength = 80.1;
const float lowerLegLength = 136.25;

const float pi = 3.14159;

const int upperServoError = 6;
const int lowerServoError = 3;

Servo lowerRightServo;
Servo upperRightServo;
Servo upperLeftServo;
Servo lowerLeftServo;

int x = -40;
int y;

float r = sqrt(sq(x) + sq(y));

// TODO: function to check (x, y) reachability

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

void setup() {
    Serial.begin(115200);
    delay(500);

    lowerLeftServo.attach(18);
    upperLeftServo.attach(19);

    lowerRightServo.attach(33);
    upperRightServo.attach(32);

}

void loop() {
    /*
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
    }*/
}