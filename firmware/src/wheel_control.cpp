#include <Arduino.h>
#include "wheel_control.h"
#include <SimpleFOC.h>
#include <Wire.h>

namespace {
    const float supplyVoltage = 12;
    TwoWire& I2C_1 = Wire;
    TwoWire& I2C_2 = Wire1;

    MagneticSensorI2C rightWheelEncoder(AS5600_I2C);
    MagneticSensorI2C leftWheelEncoder(AS5600_I2C);

    BLDCMotor rightWheelMotor(7);
    BLDCMotor leftWheelMotor(7);

    BLDCDriver3PWM rightWheelDriver(25, 26, 27, 4);
    BLDCDriver3PWM leftWheelDriver(14, 12, 13, 4);

}

void initialiseMotors() {
    // Initialise Encoders
    I2C_1.begin(21, 22, 400000);
    I2C_2.begin(5, 15, 400000);

    rightWheelEncoder.init(&I2C_1);
    leftWheelEncoder.init(&I2C_2);

    rightWheelEncoder.update();
    leftWheelEncoder.update();

    rightWheelMotor.linkSensor(&rightWheelEncoder);
    leftWheelMotor.linkSensor(&leftWheelEncoder);

    // Initialise Drivers
    rightWheelDriver.voltage_power_supply = supplyVoltage;
    leftWheelDriver.voltage_power_supply = supplyVoltage;


    if (!rightWheelDriver.init()) {
        Serial.println("Right wheel driver init failed");
        while (1);
    }

    if (!leftWheelDriver.init()) {
        Serial.println("Left wheel driver init failed");
        while (1);
    }

    rightWheelMotor.linkDriver(&rightWheelDriver);
    leftWheelMotor.linkDriver(&leftWheelDriver);

    // Initialise Motors
    rightWheelMotor.torque_controller = TorqueControlType::voltage;
    leftWheelMotor.torque_controller = TorqueControlType::voltage;
    rightWheelMotor.controller = MotionControlType::torque;
    leftWheelMotor.controller = MotionControlType::torque;

    rightWheelMotor.voltage_limit = 11;
    leftWheelMotor.voltage_limit = 11;

    rightWheelMotor.voltage_sensor_align = 3;
    leftWheelMotor.voltage_sensor_align = 3;

    if (!rightWheelMotor.init() ||
        !leftWheelMotor.init() ||
        !rightWheelMotor.initFOC() ||
        !leftWheelMotor.initFOC()) {
        
        digitalWrite(4, LOW);
        Serial.println("Motor initialization failed");

        while (true) {
            delay(100);
        }
    }

}

void driveMotors(float leftVoltage, float rightVoltage) {
    rightWheelMotor.loopFOC();
    leftWheelMotor.loopFOC();

    rightWheelMotor.move(rightVoltage);
    leftWheelMotor.move(leftVoltage);
}