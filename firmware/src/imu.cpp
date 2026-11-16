#include "imu.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


namespace {
    Adafruit_MPU6050 mpu;
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


    Serial.println(g.gyro.x);


    delay(100);
}