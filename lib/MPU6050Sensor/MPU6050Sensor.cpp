#include "MPU6050Sensor.h"

/**
 * @brief Constructor for the MPU6050Sensor class.
 * Initializes member variables to default values.
 */
MPU6050Sensor::MPU6050Sensor() : MPU6050(Wire)
{
    smoothAngleX = 0.0f;
    smoothAngleY = 0.0f;
    smoothAngleZ = 0.0f;
    previousAngleX = 0.0f;
    previousAngleY = 0.0f;
    previousAngleZ = 0.0f;
    gyroOffsetX = 0.0f;
    gyroOffsetY = 0.0f;
    gyroOffsetZ = 0.0f;
    lastUpdateTime = 0;
}

/**
 * @brief Initializes the MPU6050 sensor and calibrates the gyroscope.
 *
 * This function must be called before using the sensor to ensure it is 
 * ready for measurements.
 */
void MPU6050Sensor::begin() {
    Wire.begin();
    MPU6050::begin(); // Initialize MPU6050_tockn library
    MPU6050::calcGyroOffsets(true); // Calibrate gyroscope
    // MPU6050::calcAccelOffsets(); // Uncomment if accelerometer calibration is needed
    lastUpdateTime = millis();
}

/**
 * @brief Updates the sensor readings and computes the current angles.
 *
 * This function should be called regularly to keep the angle measurements updated.
 */
void MPU6050Sensor::update() {
    MPU6050::update(); // Explicitly call base class update method

    unsigned long currentTime = millis();
    float dt = (currentTime - lastUpdateTime) / 1000.0f; // Delta time in seconds
    lastUpdateTime = currentTime;

    // Get gyroscope data (degrees per second)
    float gyroRateX = this->getGyroX();
    float gyroRateY = this->getGyroY();
    float gyroRateZ = this->getGyroZ();

    // Apply calibration offsets
    gyroRateX -= gyroOffsetX;
    gyroRateY -= gyroOffsetY;
    gyroRateZ -= gyroOffsetZ;

    // Integrate gyroscope data to get angles
    float gyroAngleX = previousAngleX + gyroRateX * dt;
    float gyroAngleY = previousAngleY + gyroRateY * dt;
    float gyroAngleZ = previousAngleZ + gyroRateZ * dt;

    // Get accelerometer angles (X and Y axes only)
    float accelAngleX = this->getAccAngleX();
    float accelAngleY = this->getAccAngleY();

    // Apply complementary filter
    smoothAngleX = alpha * gyroAngleX + (1.0f - alpha) * accelAngleX;
    smoothAngleY = alpha * gyroAngleY + (1.0f - alpha) * accelAngleY;
    // Z axis uses gyroscope data only
    smoothAngleZ = gyroAngleZ;

    // Update previous angles
    previousAngleX = smoothAngleX;
    previousAngleY = smoothAngleY;
    previousAngleZ = smoothAngleZ;
}

/**
 * @brief Retrieves the smoothed angle around the X-axis.
 * @return The smoothed angle in degrees.
 */
float MPU6050Sensor::getSmoothAngleX() {
    return smoothAngleX;
}

/**
 * @brief Retrieves the smoothed angle around the Y-axis.
 * @return The smoothed angle in degrees.
 */
float MPU6050Sensor::getSmoothAngleY() {
    return smoothAngleY;
}

/**
 * @brief Retrieves the smoothed angle around the Z-axis.
 * @return The smoothed angle in degrees.
 */
float MPU6050Sensor::getSmoothAngleZ() {
    return smoothAngleZ;
}

/**
 * @brief Prints the current smoothed angles to the Serial monitor.
 *
 * This function is useful for debugging purposes.
 */
void MPU6050Sensor::printAngles() {
    Serial.print("Angle X: ");
    Serial.print(smoothAngleX);
    Serial.print(" | Angle Y: ");
    Serial.print(smoothAngleY);
    Serial.print(" | Angle Z: ");
    Serial.println(smoothAngleZ);
}

/**
 * @brief Resets the angle measurements and calibration offsets.
 *
 * This function sets all angles and offsets back to zero.
 */
void MPU6050Sensor::reset() {
    smoothAngleX = 0.0f;
    smoothAngleY = 0.0f;
    smoothAngleZ = 0.0f;

    previousAngleX = 0.0f;
    previousAngleY = 0.0f;
    previousAngleZ = 0.0f;

    gyroOffsetX = 0.0f;
    gyroOffsetY = 0.0f;
    gyroOffsetZ = 0.0f;

    MPU6050::calcGyroOffsets(true);
}
