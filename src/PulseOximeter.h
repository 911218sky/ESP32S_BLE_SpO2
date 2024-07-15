#ifndef PULSE_OXIMETER_H
#define PULSE_OXIMETER_H

#include <stdint.h>
#include "MAX30105.h"

// Define sensor configuration parameters
#define MAX30105_POWER_LEVEL 30   // LED current setting (0-255)
#define MAX30105_SAMPLE_AVERAGE 8 // Number of samples to average (1, 2, 4, 8, 16, 32)
#define MAX30105_LED_MODE 2       // LED mode (1 = RED only, 2 = RED + IR, 3 = RED + IR + GREEN)
#define MAX30105_SAMPLE_RATE 800  // Sampling rate in Hz (50, 100, 200, 400, 800, 1000, 1600, 3200)
#define MAX30105_PULSE_WIDTH 411  // Pulse width in us (69, 118, 215, 411)
#define MAX30105_ADC_RANGE 4096   // ADC range (2048, 4096, 8192, 16384)

// Define heart rate calculation parameters
#define RATE_SIZE 10 // Size of the heart rate circular buffer

/**
 * @brief Class for interfacing with the MAX30105 pulse oximeter sensor.
 *
 * This class provides methods for initializing the sensor, reading data,
 * and calculating heart rate and SpO2 values.
 */
class PulseOximeter : public MAX30105
{
public:
  /**
   * @brief Constructor for the PulseOximeter class.
   */
  PulseOximeter();

  /**
   * @brief Initializes the MAX30105 sensor with the defined configuration parameters.
   */
  void begin();

  /**
   * @brief Checks if a finger is detected on the sensor.
   *
   * @return true if a finger is detected, false otherwise.
   */
  bool isFingerDetected();

  /**
   * @brief Calculates and returns the SpO2 value.
   *
   * @return The calculated SpO2 value, or -1.0f if an error occurs.
   */
  float getSpO2();

  /**
   * @brief Calculates and returns the heart rate value.
   *
   * @return The calculated heart rate value, or -1 if an error occurs.
   */
  int getHeartRate();

  /**
   * @brief Updates the sensor readings and internal buffers.
   *
   * This method should be called regularly to ensure that the sensor
   * readings are up-to-date.
   */
  void update();

  /**
   * @brief Resets the sensor and internal buffers.
   *
   * This method resets the sensor to its initial state and clears all
   * internal buffers.
   */
  void reset();

  /**
   * @brief Checks if there are new sensor readings available.
   *
   * @return true if there are new sensor readings available, false otherwise.
   */
  bool hasNewValue();

private:
  const int numSamples = 50; // Number of samples to collect for averaging
  int32_t redValues[50];     // Buffer for storing red LED readings
  int32_t irValues[50];      // Buffer for storing IR LED readings
  int32_t rates[RATE_SIZE];  // Circular buffer for storing heart rate values
  int32_t lastHeartRate;     // Last calculated heart rate
  int sampleCount;           // Current sample count
  int ratesCount;            // Current heart rate count
  bool isFull;               // Flag to indicate if the circular buffer is full

  /**
   * @brief Calculates the SpO2 value based on red and IR LED readings.
   *
   * @param redValue The red LED reading.
   * @param irValue The IR LED reading.
   * @return The calculated SpO2 value, or -1.0f if an error occurs.
   */
  float calculateSpO2(int32_t redValue, int32_t irValue);

  /**
   * @brief Calculates the heart rate from IR sensor data.
   *
   * This function processes the infrared (IR) values from a pulse oximeter sensor to detect peaks
   * corresponding to heartbeats and calculates the heart rate in beats per minute (BPM).
   *
   * @param irValues Array of IR sensor values.
   * @param numSamples Number of samples in the irValues array.
   * @param heartRate Pointer to store the calculated heart rate in BPM.
   * @param validHeartRate Pointer to store the validity of the calculated heart rate (1 if valid, 0 if not).
   */
  void calculateHeartRate(int32_t *irValues, int numSamples, int32_t *heartRate, int8_t *validHeartRate);
};

#endif // PULSE_OXIMETER_H
