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
    /**
     * @brief Calibrates the gyroscope by sampling for a fixed number of samples.
     *
     * This function computes the average offset for the gyro measurements.
     */
    void calibrateGyro();

    /**
     * @brief Calculates the filter alpha value based on acceleration magnitude.
     *
     * @param accMagnitude The magnitude of acceleration measured by the accelerometer.
     * @return The calculated alpha value, constrained between ALPHA_START and ALPHA_MIN.
     */
    float getFilterAlpha(float accMagnitude);
    
    /**
     * @brief Computes the angles based on gyro and accelerometer readings.
     *
     * @param dt The time difference in seconds since the last update.
     */
    void calculateAngles(float dt);

    // Member variables to store angle values and offsets
    float angleX, angleY, angleZ;
    float gyroXoffset, gyroYoffset, gyroZoffset;
    unsigned long previousTime;
    float smoothAngleX, smoothAngleY, smoothAngleZ;
    float lastAccX, lastAccY, lastAccZ;

    // Constants for calibration and filtering
    static const int CALIBRATION_SAMPLES = 1000;  ///< Number of samples for gyro calibration.
    static constexpr float ALPHA_START = 0.8f;     ///< Starting alpha value for filtering.
    static constexpr float ALPHA_MIN = 0.1f;       ///< Minimum alpha value for filtering.
    static constexpr float MOTION_THRESHOLD = 0.1f; ///< Threshold for detecting motion.
};

#endif // MPU6050SENSOR_H
