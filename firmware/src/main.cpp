#include <Arduino.h>
#include <Wire.h>
#include <SimpleFOC.h>

// ======================================================
// HARDWARE
// ======================================================

// Two independent ESP32 I2C buses
TwoWire I2C_1 = TwoWire(0);
TwoWire I2C_2 = TwoWire(1);

// AS5600 encoders
MagneticSensorI2C sensor1(AS5600_I2C);
MagneticSensorI2C sensor2(AS5600_I2C);

// 7 pole-pair motors
BLDCMotor motor1(7);
BLDCMotor motor2(7);

// SimpleFOC Mini drivers
//                    A   B   C   EN
BLDCDriver3PWM driver1(25, 26, 27, 4);
BLDCDriver3PWM driver2(14, 12, 13, 4);


// ======================================================
// TEST SETTINGS
// ======================================================

constexpr float SUPPLY_VOLTAGE = 12.0f;

// Start moderately.
// Once everything works you can increase this.
constexpr float HOLD_VOLTAGE = 4.0f;

// FOC alignment voltage
constexpr float ALIGN_VOLTAGE = 2.0f;

// PWM frequency
constexpr long PWM_FREQUENCY = 20000;


// ======================================================
// STATE
// ======================================================

bool motor1FOC = false;
bool motor2FOC = false;

bool holdMotor1 = false;
bool holdMotor2 = false;

float holdAngle1 = 0.0f;
float holdAngle2 = 0.0f;

unsigned long lastPrint = 0;


// ======================================================
// MOTOR CONFIGURATION
// ======================================================

void configureMotor(BLDCMotor &motor) {

  // Closed-loop position control
  motor.controller = MotionControlType::angle;

  // Voltage-based torque control
  motor.torque_controller = TorqueControlType::voltage;

  // Maximum available motor voltage
  motor.voltage_limit = HOLD_VOLTAGE;

  // FOC alignment strength
  motor.voltage_sensor_align = ALIGN_VOLTAGE;


  // ----------------------------------------------------
  // POSITION LOOP
  // ----------------------------------------------------

  motor.P_angle.P = 15.0f;
  motor.P_angle.I = 0.0f;
  motor.P_angle.D = 0.0f;

  motor.velocity_limit = 30.0f;


  // ----------------------------------------------------
  // VELOCITY LOOP
  // ----------------------------------------------------

  motor.PID_velocity.P = 0.20f;
  motor.PID_velocity.I = 1.00f;
  motor.PID_velocity.D = 0.0f;

  motor.PID_velocity.limit = HOLD_VOLTAGE;

  motor.LPF_velocity.Tf = 0.01f;
}


// ======================================================
// STOP EVERYTHING
// ======================================================

void stopEverything() {

  holdMotor1 = false;
  holdMotor2 = false;

  // Remove PWM before disabling shared EN
  driver1.setPwm(0, 0, 0);
  driver2.setPwm(0, 0, 0);

  driver1.disable();
  driver2.disable();

  Serial.println();
  Serial.println(">>> BOTH MOTORS DISABLED");
}


// ======================================================
// PRINT MENU
// ======================================================

void printMenu() {

  Serial.println();
  Serial.println("========================================");
  Serial.println("DUAL MOTOR TEST");
  Serial.println("========================================");

  Serial.println("1 = Encoders only");
  Serial.println("2 = Enable drivers, zero PWM");
  Serial.println("3 = Equal PWM EMI test");
  Serial.println("4 = FOC Motor 1 + hold Motor 1");
  Serial.println("5 = FOC Motor 2 + hold Motor 2");
  Serial.println("6 = Hold BOTH motors closed-loop");
  Serial.println();
  Serial.println("c = capture current positions");
  Serial.println("0 = STOP / disable everything");
  Serial.println("m = show menu");

  Serial.println("========================================");
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  SimpleFOCDebug::enable(&Serial);


  // ====================================================
  // 1. ENCODERS
  // ====================================================

  I2C_1.begin(
    21,         // SDA
    22,         // SCL
    400000
  );

  I2C_2.begin(
    5,          // SDA
    15,         // SCL
    400000
  );

  sensor1.init(&I2C_1);
  sensor2.init(&I2C_2);

  sensor1.update();
  sensor2.update();

  Serial.println();
  Serial.println("ENCODERS:");

  Serial.print("Encoder 1: ");
  Serial.println(sensor1.getAngle(), 4);

  Serial.print("Encoder 2: ");
  Serial.println(sensor2.getAngle(), 4);


  // Link sensors
  motor1.linkSensor(&sensor1);
  motor2.linkSensor(&sensor2);


  // ====================================================
  // 2. DRIVERS
  // ====================================================

  driver1.voltage_power_supply = SUPPLY_VOLTAGE;
  driver2.voltage_power_supply = SUPPLY_VOLTAGE;

  driver1.voltage_limit = SUPPLY_VOLTAGE;
  driver2.voltage_limit = SUPPLY_VOLTAGE;

  driver1.pwm_frequency = PWM_FREQUENCY;
  driver2.pwm_frequency = PWM_FREQUENCY;


  Serial.println();
  Serial.println("Initialising driver 1...");

  if (!driver1.init()) {
    Serial.println("DRIVER 1 INIT FAILED");
    while (1);
  }


  Serial.println("Initialising driver 2...");

  if (!driver2.init()) {
    Serial.println("DRIVER 2 INIT FAILED");
    while (1);
  }


  motor1.linkDriver(&driver1);
  motor2.linkDriver(&driver2);


  // Keep power stage disabled initially
  driver1.disable();
  driver2.disable();


  // ====================================================
  // 3. MOTOR SETTINGS
  // ====================================================

  configureMotor(motor1);
  configureMotor(motor2);


  Serial.println();
  Serial.println("SETUP COMPLETE.");

  printMenu();
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  // ====================================================
  // MOTOR 1 CLOSED LOOP
  // ====================================================

  if (motor1FOC && holdMotor1) {

    motor1.loopFOC();

    motor1.move(holdAngle1);
  }


  // ====================================================
  // MOTOR 2 CLOSED LOOP
  // ====================================================

  if (motor2FOC && holdMotor2) {

    motor2.loopFOC();

    motor2.move(holdAngle2);
  }


  // ====================================================
  // KEEP UNUSED ENCODERS UPDATED
  // ====================================================

  if (!holdMotor1) {
    sensor1.update();
  }

  if (!holdMotor2) {
    sensor2.update();
  }


  // ====================================================
  // SERIAL COMMANDS
  // ====================================================

  if (Serial.available()) {

    char cmd = Serial.read();

    // Ignore line endings
    if (cmd == '\n' || cmd == '\r') {
      return;
    }


    // ==================================================
    // 1 — ENCODER TEST
    // ==================================================

    if (cmd == '1') {

      stopEverything();

      Serial.println();
      Serial.println(">>> STAGE 1: ENCODERS ONLY");
      Serial.println("Rotate both wheels by hand.");
      Serial.println("Angles should change smoothly.");
    }


    // ==================================================
    // 2 — DRIVER ENABLE / ZERO PWM
    // ==================================================

    else if (cmd == '2') {

      holdMotor1 = false;
      holdMotor2 = false;

      driver1.enable();
      driver2.enable();

      driver1.setPwm(0, 0, 0);
      driver2.setPwm(0, 0, 0);

      Serial.println();
      Serial.println(">>> STAGE 2");
      Serial.println("Both drivers ENABLED.");
      Serial.println("PWM = 0 V.");
      Serial.println("Encoders should remain perfectly stable.");
    }


    // ==================================================
    // 3 — PWM / EMI TEST
    // ==================================================

    else if (cmd == '3') {

      holdMotor1 = false;
      holdMotor2 = false;

      driver1.enable();
      driver2.enable();

      // Equal phase voltages.
      // Causes switching but no intended rotating field.
      driver1.setPwm(0.5f, 0.5f, 0.5f);
      driver2.setPwm(0.5f, 0.5f, 0.5f);

      Serial.println();
      Serial.println(">>> STAGE 3: PWM / EMI TEST");
      Serial.println("Both drivers switching.");
      Serial.println("Watch both encoder readings.");
    }


    // ==================================================
    // 4 — MOTOR 1 FOC
    // ==================================================

    else if (cmd == '4') {

      Serial.println();
      Serial.println(">>> STAGE 4: MOTOR 1 FOC");

      // Make sure motor 2 outputs nothing
      driver2.setPwm(0, 0, 0);

      // Stop raw PWM on motor 1
      driver1.setPwm(0, 0, 0);

      if (!motor1FOC) {

        Serial.println("motor1.init()");

        motor1.init();

        // Ensure correct limit after init
        motor1.voltage_limit = HOLD_VOLTAGE;
        motor1.PID_velocity.limit = HOLD_VOLTAGE;

        Serial.println("motor1.initFOC()");

        if (!motor1.initFOC()) {

          Serial.println("MOTOR 1 FOC FAILED");

          driver1.setPwm(0, 0, 0);

          return;
        }

        motor1FOC = true;

        Serial.println("MOTOR 1 FOC SUCCESS");
      }


      // Capture current position
      sensor1.update();

      holdAngle1 = sensor1.getAngle();

      motor1.target = holdAngle1;

      holdMotor1 = true;
      holdMotor2 = false;

      // Motor 2 remains electrically quiet
      driver2.setPwm(0, 0, 0);

      Serial.print("Motor 1 holding: ");
      Serial.println(holdAngle1, 4);
    }


    // ==================================================
    // 5 — MOTOR 2 FOC
    // ==================================================

    else if (cmd == '5') {

      Serial.println();
      Serial.println(">>> STAGE 5: MOTOR 2 FOC");

      // Temporarily stop motor 1 output while motor 2
      // performs its blocking FOC alignment.
      holdMotor1 = false;

      driver1.setPwm(0, 0, 0);
      driver2.setPwm(0, 0, 0);


      if (!motor2FOC) {

        Serial.println("motor2.init()");

        motor2.init();

        motor2.voltage_limit = HOLD_VOLTAGE;
        motor2.PID_velocity.limit = HOLD_VOLTAGE;

        Serial.println("motor2.initFOC()");

        if (!motor2.initFOC()) {

          Serial.println("MOTOR 2 FOC FAILED");

          driver2.setPwm(0, 0, 0);

          return;
        }

        motor2FOC = true;

        Serial.println("MOTOR 2 FOC SUCCESS");
      }


      sensor2.update();

      holdAngle2 = sensor2.getAngle();

      motor2.target = holdAngle2;

      holdMotor1 = false;
      holdMotor2 = true;

      driver1.setPwm(0, 0, 0);

      Serial.print("Motor 2 holding: ");
      Serial.println(holdAngle2, 4);
    }


    // ==================================================
    // 6 — BOTH MOTORS CLOSED LOOP
    // ==================================================

    else if (cmd == '6') {

      if (!motor1FOC || !motor2FOC) {

        Serial.println();
        Serial.println("Run stages 4 and 5 first.");

        return;
      }


      Serial.println();
      Serial.println(">>> STAGE 6: BOTH MOTORS CLOSED LOOP");


      // Capture both CURRENT positions
      sensor1.update();
      sensor2.update();

      holdAngle1 = sensor1.getAngle();
      holdAngle2 = sensor2.getAngle();

      motor1.target = holdAngle1;
      motor2.target = holdAngle2;

      // Reset accumulated PID states
      motor1.PID_velocity.reset();
      motor2.PID_velocity.reset();

      // Shared EN pin becomes active
      motor1.enable();
      motor2.enable();

      holdMotor1 = true;
      holdMotor2 = true;


      Serial.print("Motor 1 target: ");
      Serial.println(holdAngle1, 4);

      Serial.print("Motor 2 target: ");
      Serial.println(holdAngle2, 4);

      Serial.println();
      Serial.println("BOTH MOTORS HOLDING.");
    }


    // ==================================================
    // C — CAPTURE CURRENT POSITIONS
    // ==================================================

    else if (cmd == 'c') {

      if (motor1FOC) {

        sensor1.update();

        holdAngle1 = sensor1.getAngle();

        motor1.target = holdAngle1;

        motor1.PID_velocity.reset();
      }

      if (motor2FOC) {

        sensor2.update();

        holdAngle2 = sensor2.getAngle();

        motor2.target = holdAngle2;

        motor2.PID_velocity.reset();
      }

      Serial.println();
      Serial.println("Current positions captured.");
    }


    // ==================================================
    // 0 — STOP
    // ==================================================

    else if (cmd == '0') {

      stopEverything();
    }


    // ==================================================
    // MENU
    // ==================================================

    else if (cmd == 'm') {

      printMenu();
    }
  }


  // ====================================================
  // SERIAL MONITORING
  // ====================================================

  if (millis() - lastPrint >= 100) {

    float angle1 = sensor1.getAngle();
    float angle2 = sensor2.getAngle();

    Serial.print("M1: ");
    Serial.print(angle1, 3);

    Serial.print(" | ");

    if (holdMotor1) {

      Serial.print("target ");
      Serial.print(holdAngle1, 3);

    } else {

      Serial.print("not holding");
    }


    Serial.print(" || M2: ");

    Serial.print(angle2, 3);

    Serial.print(" | ");

    if (holdMotor2) {

      Serial.print("target ");
      Serial.print(holdAngle2, 3);

    } else {

      Serial.print("not holding");
    }

    Serial.println();

    lastPrint = millis();
  }
}