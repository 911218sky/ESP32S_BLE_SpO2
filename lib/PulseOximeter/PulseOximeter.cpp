#include "PulseOximeter.h"
#include <Wire.h>
#include <math.h>

PulseOximeter::PulseOximeter() : sampleCount(0), lastHeartRate(0), ratesCount(0), isFull(false) {}

void PulseOximeter::begin()
{
  if (!MAX30105::begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  MAX30105::setup(MAX30105_POWER_LEVEL, MAX30105_SAMPLE_AVERAGE, MAX30105_LED_MODE,
                  MAX30105_SAMPLE_RATE, MAX30105_PULSE_WIDTH, MAX30105_ADC_RANGE);
  reset();
}

void PulseOximeter::reset()
{
  if (sampleCount == 0)
    return;
  memset(redValues, 0, sizeof(redValues));
  memset(irValues, 0, sizeof(irValues));
  sampleCount = 0;
  ratesCount = 0;
  lastHeartRate = 0;
  isFull = false;
}

bool PulseOximeter::isFingerDetected()
{
  return getIR() >= 50000;
}

void PulseOximeter::update()
{
  redValues[sampleCount] = getRed();
  irValues[sampleCount] = getIR();
  sampleCount++;
  if (sampleCount >= numSamples)
  {
    sampleCount = 0;
    isFull = true;
  }
}

bool PulseOximeter::hasNewValue()
{
  return sampleCount % 5 == 0;
}

float PulseOximeter::getSpO2()
{
  int32_t totalRed = 0, totalIR = 0;
  int n = isFull ? numSamples : sampleCount;

  for (int i = 0; i < n; i++)
  {
    totalRed += redValues[i];
    totalIR += irValues[i];
  }

  return calculateSpO2(totalRed / n, totalIR / n);
}

int32_t PulseOximeter::getHeartRate()
{
  int32_t heartRate;
  int8_t validHeartRate;
  calculateHeartRate(irValues, numSamples, &heartRate, &validHeartRate);

  if (validHeartRate && (abs(lastHeartRate - heartRate) < 30 || !lastHeartRate))
  {
    lastHeartRate = heartRate;
    return heartRate;
  }
  return -1;
}

float PulseOximeter::calculateSpO2(int32_t redValue, int32_t irValue)
{
  if (redValue <= 0 || irValue <= 0)
    return -1.0f;

  float ratio = static_cast<float>(redValue) / static_cast<float>(irValue);
  return constrain(100.0f - 5.0f * (ratio - 0.75f), 0.0f, 100.0f);
}

void PulseOximeter::calculateHeartRate(int32_t *irValues, int numSamples, int32_t *heartRate, int8_t *validHeartRate)
{
  int peakCount = 0, lastPeakIndex = -1, sumIntervals = 0;
  int n = isFull ? numSamples : sampleCount;

  for (int i = 1; i < n - 1; i++)
  {
    if (irValues[i] > irValues[i - 1] && irValues[i] > irValues[i + 1])
    {
      peakCount++;
      if (lastPeakIndex != -1)
      {
        sumIntervals += (i - lastPeakIndex);
      }
      lastPeakIndex = i;
    }
  }

  if (peakCount < 2)
  {
    *heartRate = 0;
    *validHeartRate = 0;
    return;
  }

  *heartRate = ((60.0f) / ((float)sumIntervals / (peakCount - 1) * 0.4)) * 2.3;
  int rateCount = 0;

  for (int i = 0; i < RATE_SIZE; i++)
  {
    if (!rates[i])
      continue;
    *heartRate += rates[i];
    rateCount++;
  }
  *heartRate /= (rateCount + 1);

  if (*heartRate < 60 || *heartRate > 300)
  {
    *heartRate = 0;
    *validHeartRate = 0;
    return;
  }

  rates[ratesCount++] = *heartRate;
  ratesCount %= RATE_SIZE; // Size of the rate buffer
  *validHeartRate = 1;
}
