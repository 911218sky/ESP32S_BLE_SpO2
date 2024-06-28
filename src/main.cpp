#include <stdint.h>
#include "BLEHandler.h"
#include "PulseOximeter.h"

#define DEBUG_LEVEL 1

PulseOximeter pulseOximeter;
BLEHandler bleHandler;

bool deviceConnected = false;
float heartRate = -1;
float spO2 = -1;

int __round(float value)
{
  return static_cast<int>(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

void BLEHandlerCharacteristicCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
  if (DEBUG_LEVEL >= 1)
    Serial.println("onRead callback triggered");
  // Create a buffer to store the data
  uint8_t data[8];

  // Round the heart rate and spO2 values
  int roundedHeartRate = __round(heartRate);
  int roundedSpO2 = __round(spO2);

  // Copy the heartRate and spO2 data into the data array using memcpy
  memcpy(data, &roundedHeartRate, sizeof(roundedHeartRate));             // Copy heartRate
  memcpy(data + sizeof(roundedSpO2), &roundedSpO2, sizeof(roundedSpO2)); // Copy spO2

  // Set the data to the characteristic and send it to the client
  pCharacteristic->setValue(data, sizeof(data));
}

void BLEHandlerServerCallbacks::onConnect(BLEServer *pServer)
{
  Serial.println("Device is connected");
  pulseOximeter.particleSensor.wakeUp();
  deviceConnected = true;
}

void BLEHandlerServerCallbacks::onDisconnect(BLEServer *pServer)
{
  Serial.println("Device is disconnected");
  bleHandler.startAdvertising();
  deviceConnected = false;
}

void setup()
{
  Serial.begin(9600);
  if (DEBUG_LEVEL >= 1)
    Serial.println("Initializing...");
  pulseOximeter.begin();
  bleHandler.begin("ESP-32S-001");
}

void loop()
{
  if (!deviceConnected)
  {
    if (DEBUG_LEVEL >= 1)
      Serial.println("No device connected. Waiting for connection...");
    pulseOximeter.particleSensor.shutDown();
    delay(1000);
    return;
  }

  if (!pulseOximeter.isFingerDetected())
  {
    if (DEBUG_LEVEL >= 1)
      Serial.println("No finger detected. Please place your finger on the sensor.");
    pulseOximeter.reset();
    spO2 = -1;
    heartRate = -1;
    delay(1000);
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
      Serial.println(spO2);
    }
    if (DEBUG_LEVEL >= 1 && heartRate != -1)
    {
      Serial.print("Heart Rate: ");
      Serial.println(heartRate);
    }
  }

  delay(50);
}
