#include "BLEHandler.h"

BLEHandler::BLEHandler()
{
}

void BLEHandler::begin(const std::string &deviceName, const std::string &service_uuid, const std::string &characteristic_uuid)
{
  // Initialize BLE device
  BLEDevice::init(deviceName);

  // Create BLE server
  createBLEServer();

  // Create BLE service and characteristic
  createBLEServiceAndCharacteristic(service_uuid, characteristic_uuid);

  // Start advertising
  startAdvertising();
}

void BLEHandler::createBLEServer()
{
  // Create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);
}

void BLEHandler::createBLEServiceAndCharacteristic(const std::string &service_uuid, const std::string &characteristic_uuid)
{
  // Create BLE service
  BLEService *pService = pServer->createService(service_uuid);

  // Create BLE characteristic
  pCharacteristic = pService->createCharacteristic(
      characteristic_uuid,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  // Set characteristic callbacks
  pCharacteristic->setCallbacks(&characteristicCallbacks);

  // Add characteristic to service
  pService->start();
}

void BLEHandler::startAdvertising()
{
  // Start advertising
  pServer->getAdvertising()->start();
}

void BLEHandler::notify(const std::string &value)
{
  // Send notification
  pCharacteristic->setValue(value);
  pCharacteristic->notify();
}

// --- BLEHandlerCharacteristicCallbacks ---

void BLEHandlerCharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
}

// void BLEHandlerCharacteristicCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
// {
// }

// --- BLEHandlerServerCallbacks ---

// void BLEHandlerServerCallbacks::onConnect(BLEServer *pServer)
// {
//   Serial.println("Device is connected");
// }

// void BLEHandlerServerCallbacks::onDisconnect(BLEServer *pServer)
// {
//   Serial.println("Device is disconnected");
// }
