#include <stdint.h>
#include "BLEHandler.h"
#include "PulseOximeter.h"

#define DEBUG_LEVEL 1

PulseOximeter pulseOximeter;
BLEHandler bleHandler;

float heartRate = -1;
float spO2 = -1;

void BLEHandlerCharacteristicCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
  Serial.println("onRead callback triggered");
  // Create a buffer to store the data
  uint8_t data[8];

  // Copy the heartRate and spO2 data into the data array using memcpy
  memcpy(data, &heartRate, sizeof(heartRate));           // Copy heartRate
  memcpy(data + sizeof(heartRate), &spO2, sizeof(spO2)); // Copy spO2

  // Set the data to the characteristic and send it to the client
  pCharacteristic->setValue(data, sizeof(data));
}

void BLEHandlerServerCallbacks::onDisconnect(BLEServer *pServer)
{
  if (DEBUG_LEVEL >= 1)
    Serial.println("Device is disconnected");
  bleHandler.startAdvertising();
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
  if (!pulseOximeter.isFingerDetected())
  {
    Serial.println("No finger detected. Please place your finger on the sensor.");
    pulseOximeter.lastHeartRate = 0;
    delay(1000);
    return;
  }
  pulseOximeter.update();
  if (!pulseOximeter.sampleCount)
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