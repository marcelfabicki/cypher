#include "pid_control.h"
#include "imu.h"

float proportionalTorque;
float pConstant = 73;
float dConstant = 5;
float targetAngle = 0;

float outputTorque() {
    return pConstant * (targetAngle - tilt()) - dConstant * tiltSpeed();
}

float dTorqueTest() {
    return dConstant * tiltSpeed();
}