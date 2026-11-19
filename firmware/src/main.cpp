#include <Arduino.h>
#include "leg_control.h"
#include "imu.h"


void setup() {
    Serial.begin(115200);
    delay(500);
    initialiseServos();
    initialiseIMU();
    delay(500);

    //setLegPositions(5, -200);
}

void loop() {
    Serial.println(tilt());
   
}