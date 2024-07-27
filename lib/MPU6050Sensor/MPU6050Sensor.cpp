#include "MPU6050Sensor.h"

MPU6050Sensor::MPU6050Sensor() : MPU6050(Wire)
{
    angleX = 0;
    angleY = 0;
    angleZ = 0;
    gyroXoffset = 0;
    gyroYoffset = 0;
    gyroZoffset = 0;
    previousTime = 0;
    smoothAngleX = 0;
    smoothAngleY = 0;
    smoothAngleZ = 0;
    lastAccX = 0;
    lastAccY = 0;
    lastAccZ = 0;
}

void MPU6050Sensor::begin()
{
    MPU6050::begin();
    calibrateGyro();
    previousTime = micros();
}

void MPU6050Sensor::update()
{
    MPU6050::update();
    unsigned long currentTime = micros();
    float dt = (currentTime - previousTime) / 1000000.0;
    previousTime = currentTime;
    calculateAngles(dt);
}

float MPU6050Sensor::getSmoothAngleX()
{
    return smoothAngleX;
}

float MPU6050Sensor::getSmoothAngleY()
{
    return smoothAngleY;
}

float MPU6050Sensor::getSmoothAngleZ()
{
    return smoothAngleZ;
}

void MPU6050Sensor::printAngles()
{
    Serial.print("AngleX: ");
    Serial.print(smoothAngleX);
    Serial.print("\tAngleY: ");
    Serial.print(smoothAngleY);
    Serial.print("\tAngleZ: ");
    Serial.println(smoothAngleZ);
}

void MPU6050Sensor::reset()
{
    angleX = 0;
    angleY = 0;
    angleZ = 0;
    smoothAngleX = 0;
    smoothAngleY = 0;
    smoothAngleZ = 0;
    lastAccX = 0;
    lastAccY = 0;
    lastAccZ = 0;
    previousTime = micros();
}

void MPU6050Sensor::calibrateGyro()
{
    float sumX = 0, sumY = 0, sumZ = 0;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
        MPU6050::update();
        sumX += getGyroX();
        sumY += getGyroY();
        sumZ += getGyroZ();
        delay(1);
    }

    gyroXoffset = sumX / CALIBRATION_SAMPLES;
    gyroYoffset = sumY / CALIBRATION_SAMPLES;
    gyroZoffset = sumZ / CALIBRATION_SAMPLES;
}

float MPU6050Sensor::getFilterAlpha(float accMagnitude)
{
    float alpha = ALPHA_START + (ALPHA_MIN - ALPHA_START) * (accMagnitude - 1) / MOTION_THRESHOLD;
    return constrain(alpha, ALPHA_START, ALPHA_MIN);
}

void MPU6050Sensor::calculateAngles(float dt)
{
    float accX = getAccX();
    float accY = getAccY();
    float accZ = getAccZ();
    float gyroX = getGyroX() - gyroXoffset;
    float gyroY = getGyroY() - gyroYoffset;
    float gyroZ = getGyroZ() - gyroZoffset;

    // Calculate angles from accelerometer data
    float accelAngleX = atan2(accY, sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    float accelAngleY = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;

    // Integrate gyro data
    angleX += gyroX * dt;
    angleY += gyroY * dt;
    angleZ += gyroZ * dt;

    // Calculate acceleration magnitude for adaptive filtering
    float accMagnitude = sqrt(accX * accX + accY * accY + accZ * accZ);
    float alpha = getFilterAlpha(accMagnitude);

    // Complementary filter
    smoothAngleX = alpha * (smoothAngleX + gyroX * dt) + (1 - alpha) * accelAngleX;
    smoothAngleY = alpha * (smoothAngleY + gyroY * dt) + (1 - alpha) * accelAngleY;
    smoothAngleZ = angleZ; // We don't have a reference for Z angle from accelerometer

    // Update last acceleration values
    lastAccX = accX;
    lastAccY = accY;
    lastAccZ = accZ;
}
