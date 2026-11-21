#include <Arduino.h>
#include "leg_control.h"
#include "imu.h"
#include "wheel_control.h"
#include "pid_control.h"


void setup() {
    Serial.begin(115200);
    delay(500);
    initialiseServos();
    //initialiseIMU();
    //initialiseMotors();
    delay(500);

    //setLegPositions(10, -120);
    //setLegAngle(0, 180);

    setTimedLegPosition(10, -200, 2000);
}

void loop() {
   //driveMotors(outputTorque(), outputTorque());
   //Serial.println(outputTorque());
}