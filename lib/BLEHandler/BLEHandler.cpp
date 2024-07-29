#include "BLEHandler.h"

BLEHandler::BLEHandler() : pServer(nullptr), pCharacteristic(nullptr) {}

void BLEHandler::begin(const char *deviceName, const char *service_uuid, const char *characteristic_uuid) {
    // Initialize the BLE device
    NimBLEDevice::init(deviceName);
    
    // Create a server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(this);

    // Create a service
    NimBLEService *pService = pServer->createService(service_uuid);
    
    // Create a characteristic
    pCharacteristic = pService->createCharacteristic(characteristic_uuid, 
                                    NIMBLE_PROPERTY::READ | 
                                    NIMBLE_PROPERTY::WRITE);
    pCharacteristic->setCallbacks(this);

    // Start the service
    pService->start();

    // Start advertising
    startAdvertising();
}

void BLEHandler::startAdvertising() {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pCharacteristic->getService()->getUUID());
    pAdvertising->start();
    Serial.println("BLE Advertising Started.");
}
