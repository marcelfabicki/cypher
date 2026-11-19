#include "pid_control.h"
#include "imu.h"

float proportionalTorque;
float pConstant = 7;
float targetAngle = 0;

float outputTorque() {
    return pConstant * (targetAngle - tilt());
}