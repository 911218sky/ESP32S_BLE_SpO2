#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

class BLEHandler : public BLECharacteristicCallbacks, public BLEServerCallbacks
{
public:
    BLEHandler();
    void begin(const std::string &deviceName, const std::string &service_uuid, const std::string &characteristic_uuid);
    void startAdvertising();

    // BLECharacteristicCallbacks
    void onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;
    // void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;

    // BLEServerCallbacks
    void onConnect(BLEServer *pServer) override;
    void onDisconnect(BLEServer *pServer) override;

private:
    void createBLEServer();
    void createBLEServiceAndCharacteristic(const std::string &service_uuid, const std::string &characteristic_uuid);

    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;
    std::string receivedValue, service_uuid, characteristic_uuid;
};

#endif // BLE_HANDLER_H
