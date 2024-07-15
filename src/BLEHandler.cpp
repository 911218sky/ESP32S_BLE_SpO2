#include "BLEHandler.h"
#include <Arduino.h> // 為了使用 Serial

BLEHandler::BLEHandler() : pServer(nullptr), pCharacteristic(nullptr) {}

void BLEHandler::begin(const std::string &deviceName, const std::string &service_uuid, const std::string &characteristic_uuid)
{
  this->service_uuid = service_uuid;
  this->characteristic_uuid = characteristic_uuid;

  BLEDevice::init(deviceName);
  createBLEServer();
  createBLEServiceAndCharacteristic(service_uuid, characteristic_uuid);
  Serial.println("BLE service initialized successfully");
}

void BLEHandler::startAdvertising()
{
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(service_uuid);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE advertising started");
}

void BLEHandler::createBLEServer() {
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);
}

void BLEHandler::createBLEServiceAndCharacteristic(const std::string &service_uuid, const std::string &characteristic_uuid)
{
  BLEService *pService = pServer->createService(service_uuid);
  pCharacteristic = pService->createCharacteristic(
      characteristic_uuid,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->setCallbacks(this);
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
}

// 實現回調方法（即使是空的實現）
// void BLEHandler::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) {
//     Serial.println("Characteristic Read");
// }

// void BLEHandler::onConnect(BLEServer *pServer) {
//     Serial.println("Client Connected");
// }

// void BLEHandler::onDisconnect(BLEServer *pServer) {
//     Serial.println("Client Disconnected");
// }
