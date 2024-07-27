#include <stdint.h>
#include "BLEHandler.h"
#include "PulseOximeter.h"
#include "MPU6050Sensor.h"

#define DEBUG_LEVEL 0
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define DEVICE_NAME "ESP-32S-001"

PulseOximeter pulseOximeter;
BLEHandler bleHandler;
MPU6050Sensor mpu;

float heartRate = -1;
float spO2 = -1;

void BLEHandler::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
  if (DEBUG_LEVEL >= 1)
    Serial.println("onRead callback triggered");
  uint8_t data[8];

  int roundedHeartRate = std::round(heartRate);
  int roundedSpO2 = std::round(spO2);

  memcpy(data, &roundedHeartRate, sizeof(roundedHeartRate));             // Copy heartRate
  memcpy(data + sizeof(roundedSpO2), &roundedSpO2, sizeof(roundedSpO2)); // Copy spO2

  pCharacteristic->setValue(data, sizeof(data));
}

void BLEHandler::onConnect(BLEServer *pServer)
{
  Serial.println("Device is connected");
  pulseOximeter.wakeUp();
}

void BLEHandler::onDisconnect(BLEServer *pServer)
{
  Serial.println("Device is disconnected");
  bleHandler.startAdvertising();
  pulseOximeter.shutDown();
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  if (DEBUG_LEVEL >= 1)
    Serial.println("Initializing...");
  pulseOximeter.begin();
  bleHandler.begin(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);
  bleHandler.startAdvertising();
  mpu.begin();
  pulseOximeter.shutDown();
}

void loop()
{

  // if (DEBUG_LEVEL >= 1)
  // {
  //   mpu.update();
  //   mpu.printAngles();
  //   Serial.print("Temperature: ");
  //   Serial.println(mpu.getTemp());
  // }

  if (!pulseOximeter.isFingerDetected())
  {
    // if (DEBUG_LEVEL >= 1)
    //   Serial.println("No finger detected. Please place your finger on the sensor.");
    pulseOximeter.reset();
    spO2 = -1;
    heartRate = -1;
    // delay(1000);
    return;
  }

  pulseOximeter.update();
  if (pulseOximeter.hasNewValue())
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

  delay(50);
}