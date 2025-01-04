#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

/**
 * @class MPU6050Sensor
 * @brief A class to interface with the MPU6050 sensor and compute orientation angles.
 *
 * This class extends the functionality of the MPU6050 library to include smoothing
 * of the angle measurements using a complementary filter. It also handles gyro calibration.
 */
class MPU6050Sensor : public MPU6050 {
public:
    /**
     * @brief Constructor for the MPU6050Sensor class.
     * Initializes member variables to default values.
     */
    MPU6050Sensor();

    /**
     * @brief Initializes the MPU6050 sensor and calibrates the gyroscope.
     *
     * This function must be called before using the sensor to ensure it is 
     * ready for measurements.
     */
    void begin();

    /**
     * @brief Updates the sensor readings and computes the current angles.
     *
     * This function should be called regularly to keep the angle measurements updated.
     */
    void update();

    /**
     * @brief Retrieves the smoothed angle around the X-axis.
     * @return The smoothed angle in degrees.
     */
    float getSmoothAngleX();

    /**
     * @brief Retrieves the smoothed angle around the Y-axis.
     * @return The smoothed angle in degrees.
     */
    float getSmoothAngleY();

    /**
     * @brief Retrieves the smoothed angle around the Z-axis.
     * @return The smoothed angle in degrees.
     */
    float getSmoothAngleZ();

    /**
     * @brief Prints the current smoothed angles to the Serial monitor.
     *
     * This function is useful for debugging purposes.
     */
    void printAngles();

    /**
     * @brief Resets the angle measurements and calibration offsets.
     *
     * This function sets all angles and offsets back to zero.
     */
    void reset();

private:
    // Smoothed angles
    float smoothAngleX;
    float smoothAngleY;
    float smoothAngleZ;

    // Previous angles for complementary filter
    float previousAngleX;
    float previousAngleY;
    float previousAngleZ;

    // Calibration offsets
    float gyroOffsetX;
    float gyroOffsetY;
    float gyroOffsetZ;

    // Complementary filter coefficient (0 < alpha < 1)
    const float alpha = 0.98;

    // Time tracking
    unsigned long lastUpdateTime;
};

#endif // MPU6050SENSOR_H
