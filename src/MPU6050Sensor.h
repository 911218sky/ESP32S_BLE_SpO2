#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

class MPU6050Sensor : public MPU6050 {
public:
    MPU6050Sensor();
    void begin();
    void update();
    float getSmoothAngleX();
    float getSmoothAngleY();
    float getSmoothAngleZ();
    void printAngles();
    void reset();

private:
    void calibrateGyro();
    float getFilterAlpha(float accMagnitude);
    void calculateAngles(float dt);

    float angleX, angleY, angleZ;
    float gyroXoffset, gyroYoffset, gyroZoffset;
    unsigned long previousTime;
    float smoothAngleX, smoothAngleY, smoothAngleZ;
    float lastAccX, lastAccY, lastAccZ;

    static const int CALIBRATION_SAMPLES = 1000;
    static constexpr float ALPHA_START = 0.8f;
    static constexpr float ALPHA_MIN = 0.1f;
    static constexpr float MOTION_THRESHOLD = 0.1f;
};

#endif // MPU6050SENSOR_H
