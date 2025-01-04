#include <Arduino.h>
#include <Wire.h>
#include "BLEHandler.h"
#include "PulseOximeter.h"
#include "MPU6050Sensor.h"
#include "Config.h"

#define DEBUG_LEVEL 0
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define DEVICE_NAME "ESP-32S-001"

// 0: MAX30105_MODE
// 1: MPU_MODE
bool MODES[2] = {true, false};

PulseOximeter pulseOximeter;
BLEHandler bleHandler;
MPU6050Sensor mpu;

float heartRate = -1;
float spO2 = -1;
int16_t x = 0;
int16_t y = 0;
int16_t z = 0;
int16_t temperature = 0;

size_t heartRateSize = sizeof(heartRate);
size_t spO2Size = sizeof(spO2);
size_t xSize = sizeof(x);
size_t ySize = sizeof(y);
size_t zSize = sizeof(z);
size_t temperatureSize = sizeof(temperature);

void BLEHandler::onWrite(NimBLECharacteristic *pCharacteristic)
{
  std::string receivedData = pCharacteristic->getValue();
  memcpy(MODES, receivedData.data(), sizeof(MODES));
  if (DEBUG_LEVEL >= 1)
  {
    printf("MODES: %d %d\n", MODES[MAX30105_MODE], MODES[MPU_MODE]);
    if (MODES[MAX30105_MODE])
      printf("MAX30105_MODE is enabled\n");
    else
      printf("MAX30105_MODE is disabled\n");
    if (MODES[MPU_MODE])
      printf("MPU_MODE is enabled\n");
    else
      printf("MPU_MODE is disabled\n");
  }

  // Wake up the pulse oximeter if it is enabled
  if (MODES[MAX30105_MODE])
    pulseOximeter.wakeUp();
  else
  {
    pulseOximeter.reset();
    pulseOximeter.shutDown();
  }
  // MPU cannot be turned off, so it is not handled
}

void BLEHandler::onRead(NimBLECharacteristic *pCharacteristic)
{
  if (DEBUG_LEVEL >= 1)
    Serial.println("onRead callback triggered");

  // Create a buffer to store the data
  uint8_t data[16];

  // Round the heart rate and spO2 values
  int roundedHeartRate = MODES[MAX30105_MODE] ? std::round(heartRate) : -1;
  int roundedSpO2 = MODES[MAX30105_MODE] ? std::round(spO2) : -1;

  const float PRECISION_FACTOR = 100.0;
  x = MODES[MPU_MODE] ? constrain(std::round(mpu.getAngleX() * PRECISION_FACTOR) / PRECISION_FACTOR, -180, 180) : -1;
  y = MODES[MPU_MODE] ? constrain(std::round(mpu.getAngleY() * PRECISION_FACTOR) / PRECISION_FACTOR, -180, 180) : -1;
  z = MODES[MPU_MODE] ? constrain(std::round(mpu.getAngleZ() * PRECISION_FACTOR) / PRECISION_FACTOR, -180, 180) : -1;
  temperature = MODES[MPU_MODE] ? constrain(mpu.getTemp(), -255, 255) : -1;

  // Copy the data into the data array
  memcpy(data, &roundedHeartRate, heartRateSize);
  memcpy(data + heartRateSize, &roundedSpO2, spO2Size);
  memcpy(data + heartRateSize + spO2Size, &x, xSize);
  memcpy(data + heartRateSize + spO2Size + xSize, &y, ySize);
  memcpy(data + heartRateSize + spO2Size + xSize + ySize, &z, zSize);
  memcpy(data + heartRateSize + spO2Size + xSize + ySize + zSize, &temperature, temperatureSize);

  // Set the data to the characteristic and send it to the client
  pCharacteristic->setValue(data, sizeof(data));
}

void BLEHandler::onConnect(NimBLEServer *pServer)
{
  pulseOximeter.begin();
  Serial.println("Device is connected");
}

void BLEHandler::onDisconnect(NimBLEServer *pServer)
{
  pulseOximeter.shutDown();
  bleHandler.startAdvertising();
  Serial.println("Device is disconnected");
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  if (DEBUG_LEVEL >= 1)
    Serial.println("Initializing...");
  pulseOximeter.begin();
  pulseOximeter.shutDown();
  bleHandler.begin(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);
  bleHandler.startAdvertising();
  mpu.begin();
}

void loop()
{

  if (DEBUG_LEVEL >= 1)
  {
    // mpu.printAngles();
    // Serial.print("Temperature: ");
    // Serial.println(mpu.getTemp());
  }

  if (MODES[MPU_MODE])
    mpu.update();

  if (MODES[MAX30105_MODE])
    pulseOximeter.update();

  if (MODES[MAX30105_MODE] && !pulseOximeter.isFingerDetected())
  {
    // if (DEBUG_LEVEL >= 1)
    //   Serial.println("No finger detected. Please place your finger on the sensor.");
    pulseOximeter.reset();
    spO2 = -1;
    heartRate = -1;
    // delay(1000);
    return;
  }

  if (MODES[MAX30105_MODE] && pulseOximeter.hasNewValue())
  {
    spO2 = pulseOximeter.getSpO2();
    heartRate = pulseOximeter.getHeartRate();
    if (DEBUG_LEVEL >= 1)
    {
      Serial.print("Average SpO2:");
      Serial.println(std::round(spO2));
    }
    if (DEBUG_LEVEL >= 1 && heartRate != -1)
    {
      Serial.print("Heart Rate: ");
      Serial.println(std::round(heartRate));
    }
  }

  // Delay for the pulse oximeter
  if (MODES[MAX30105_MODE])
    delay(50);
}