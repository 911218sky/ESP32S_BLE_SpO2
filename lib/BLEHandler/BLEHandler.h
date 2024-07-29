#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>

/**
 * @class BLEHandler
 * @brief Handles Bluetooth Low Energy (BLE) server functionalities.
 */
class BLEHandler : public NimBLECharacteristicCallbacks, public NimBLEServerCallbacks
{
public:
    BLEHandler();
    void begin(const char *deviceName, const char *service_uuid, const char *characteristic_uuid);
    void startAdvertising();

    // Overriding callbacks
    void onRead(NimBLECharacteristic *pCharacteristic) override;
    void onConnect(NimBLEServer *pServer) override;
    void onDisconnect(NimBLEServer *pServer) override;

private:
    NimBLEServer *pServer;
    NimBLECharacteristic *pCharacteristic;
};

#endif // BLE_HANDLER_H
