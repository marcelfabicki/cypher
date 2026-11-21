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

    float lowerAngle(float x, float y) {
        float r = sqrt(sq(x) + sq(y));
        return 180 - acos((sq(upperLegLength) + sq(lowerLegLength) - sq(r)) / (2 * upperLegLength * lowerLegLength)) * 180 / pi;
    }

    float upperAngle(float x, float y) {
        float r = sqrt(sq(x) + sq(y));
        return (-atan2(y, x) - acos((sq(r) + sq(upperLegLength) - sq(lowerLegLength)) / (2 * r * upperLegLength))) * 180 / pi;
    }

    void writeServo (char servo, float angle) {
        if (servo == 'u') {
            angle = constrain(angle, 0.0f, 90.0f);

            upperLeftServo.write(constrain((180 - upperServoError) - angle, 90 , 180));
            upperRightServo.write(angle);
        }

        if (servo == 'l') {
            lowerLeftServo.write(constrain((180 - lowerServoError) - angle, 0 , 180));
            lowerRightServo.write(constrain(angle, 0, 180));
        }
  
    }
    bool checkAngleInput(float upper, float lower) {
        return upper >= 0 && upper <= 90 && lower >= 0 && lower <= 180;
    }

    bool checkPositionInput(float x, float y) {
        float m = sqrt(sq(x) + sq(y));
        return m <= (upperLegLength + lowerLegLength) && m >= (lowerLegLength - upperLegLength) && y <= 10 && checkAngleInput(upperAngle(x, y), lowerAngle(x, y));
    }

}


void initialiseServos() {
    lowerLeftServo.attach(lowerLeftServoPin);
    lowerRightServo.attach(lowerRightServoPin);

    upperLeftServo.attach(upperLeftServoPin);
    upperRightServo.attach(upperRightServoPin);
}

void setLegPositions (float x, float y) {
    if (!checkPositionInput(x, y)) {
        Serial.println("Invalid point");
        return;
    }

    writeServo('u', upperAngle(x, y));
    writeServo('l', lowerAngle(x, y)); 
}

void setLegAngle(float upper, float lower) {
    writeServo('u', upper);
    writeServo('l', lower);  
}

void homeServos() {
    writeServo('u', 0);
    writeServo('l', 180); 
    // position (-56.15, 0)
}

void setTimedLegPosition (float x, float y, int time) {
    if (!checkPositionInput(x, y)) {
        Serial.println("Invalid point");
        return;
    }

    homeServos();
    delay(1000);

    int steps = 50;
    float targetUpper = upperAngle(x, y);
    float targetLower = lowerAngle(x, y);

    for (float i = 1; i <= steps; i++ ) {
        setLegAngle(targetUpper * i / steps, ((targetLower - 180) * (i / steps) + 180));
        delay(time / steps);
    }
}