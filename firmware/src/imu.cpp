#include "imu.h"
#include "cypher_config.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

namespace {
    Adafruit_MPU6050 mpu;

    double tiltAngle;
}
void initialiseIMU() {
    mpu.begin();
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void readSensor() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    Serial.println(a.gyro.x);

    delay(100);
}

float tilt() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    tiltAngle = 0.98 * (tiltAngle + (g.gyro.x + imuGyroError) * imuElapsedTime) + 0.02 * atan2(a.gyro.y + imuYAxisError, a.gyro.z + imuZAxisError);
    return tiltAngle;
}