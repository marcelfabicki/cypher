#include <Arduino.h>
#include "leg_control.h"
#include "imu.h"
#include "wheel_control.h"
#include "pid_control.h"


void setup() {
    Serial.begin(115200);
    delay(500);
    initialiseServos();
    initialiseIMU();
    initialiseMotors();
    delay(500);

    setLegPositions(16, -200);
}

void loop() {
   driveMotors(outputTorque(), outputTorque());
   Serial.println(outputTorque());
}