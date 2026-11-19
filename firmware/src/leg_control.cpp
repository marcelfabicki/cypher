#include "leg_control.h"
#include "cypher_config.h"
#include <ESP32Servo.h>
#include <cmath>
#include <Arduino.h>

namespace {
    Servo lowerRightServo;
    Servo upperRightServo;
    Servo upperLeftServo;
    Servo lowerLeftServo;

    float lowerAngle(int x, int y) {
        float r = sqrt(sq(x) + sq(y));
        return 180 - acos((sq(upperLegLength) + sq(lowerLegLength) - sq(r)) / (2 * upperLegLength * lowerLegLength)) * 180 / pi;
    }

    float upperAngle(int x, int y) {
        float r = sqrt(sq(x) + sq(y));
        return (-atan2(y, x) - acos((sq(r) + sq(upperLegLength) - sq(lowerLegLength)) / (2 * r * upperLegLength))) * 180 / pi;
    }

    void writeServo (char servo, int angle) {
        if (servo == 'u') {
            upperLeftServo.write(constrain((180 - upperServoError) - angle, 90 , 180));
            upperRightServo.write(constrain(angle, 0, 90));
        }

        if (servo == 'l') {
            lowerLeftServo.write(constrain((180 - lowerServoError) - angle, 0 , 170));
            lowerRightServo.write(constrain(angle, 10, 180));
        }
  
    }
    bool checkInput(int x, int y) {
        float m = sqrt(sq(x) + sq(y));
        return m < (upperLegLength + lowerLegLength) && m > (lowerLegLength - upperLegLength) && y <= 10;
    }   
}


void initialiseServos() {
    lowerLeftServo.attach(lowerLeftServoPin);
    lowerRightServo.attach(lowerRightServoPin);

    upperLeftServo.attach(upperLeftServoPin);
    upperRightServo.attach(upperRightServoPin);
}

void setLegPositions (int x, int y) {
    if (!checkInput(x, y)) {
        Serial.println("Invalid point");
        return;
    }

    writeServo('u', upperAngle(x, y));
    writeServo('l', lowerAngle(x, y)); 
}

void setTimedLegPosition (int x, int y, int time) {

}

void setLegAngle(int upper, int lower) {
    writeServo('u', upper);
    writeServo('l', lower);  
}

void homeServos() {
    writeServo('u', 0);
    writeServo('l', 165); 
}