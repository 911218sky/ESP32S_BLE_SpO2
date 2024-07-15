#include "PulseOximeter.h"
#include <Wire.h>
#include <math.h>

PulseOximeter::PulseOximeter() : sampleCount(0), lastHeartRate(0), ratesCount(0) {}

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
  memset(rates, 0, sizeof(rates));
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
  int32_t totalRed = 0;
  int32_t totalIR = 0;
  int n = isFull ? numSamples : sampleCount;

  for (int i = 0; i < n; i++)
  {
    totalRed += redValues[i];
    totalIR += irValues[i];
  }

  int32_t avgRed = totalRed / n;
  int32_t avgIR = totalIR / n;

  return calculateSpO2(avgRed, avgIR);
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
  // 1. Check for valid input values
  if (redValue <= 0 || irValue <= 0)
  {
    return -1.0f;
  }

  // 2. Calculate the ratio of red to infrared light
  float ratio = (float)redValue / (float)irValue;

  // 3. Estimate SpO2 using an empirical formula
  float spo2 = 100.0f - 5.0f * (ratio - 0.75f);

  // 4. Constrain SpO2 to the range of 0% to 100%
  if (spo2 > 100.0f)
  {
    spo2 = 100.0f;
  }
  else if (spo2 < 0.0f)
  {
    spo2 = 0.0f;
  }

  return spo2;
}

// Function to calculate heart rate from the given data
void PulseOximeter::calculateHeartRate(int32_t *irValues, int numSamples, int32_t *heartRate, int8_t *validHeartRate)
{
  int peakCount = 0;
  int lastPeakIndex = -1;
  int sumIntervals = 0;
  int n = isFull ? numSamples : sampleCount;

  for (int i = 1; i < n - 1; i++)
  {
    if (irValues[i] > irValues[i - 1] && irValues[i] > irValues[i + 1])
    {
      peakCount++;
      if (lastPeakIndex != -1)
        sumIntervals += (i - lastPeakIndex);
      lastPeakIndex = i;
    }
  }

  if (peakCount < 2)
  {
    *heartRate = 0;
    *validHeartRate = 0;
    return;
  }

  // 計算平均峰值間隔
  float avgInterval = (float)sumIntervals / (peakCount - 1);

  // 計算心率 (BPM)
  // 公式: 心率 = (60 * 采樣率) / 平均峰值間隔
  *heartRate = ((60.0f) / (avgInterval * 0.4)) * 2.3;

  int rateCount = 0;

  // 濾波心率
  for (int i = 0; i < RATE_SIZE; i++)
  {
    if (!rates[i])
      continue;
    *heartRate += rates[i];
    rateCount++;
  }

  *heartRate /= (rateCount + 1);

  // If the calculated heart rate is outside a reasonable range, set it as invalid and return
  if (*heartRate < 60 || *heartRate > 300)
  {
    *heartRate = 0;
    *validHeartRate = 0;
    return;
  }

  rates[ratesCount++] = *heartRate;
  ratesCount %= RATE_SIZE;

  // If the heart rate is within a reasonable range, mark it as valid
  *validHeartRate = 1;
}
