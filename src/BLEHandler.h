#ifndef BLEHANDLER_H
#define BLEHANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Define UUIDs for the service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/**
 * @class BLEHandlerCharacteristicCallbacks
 * @brief The BLEHandlerCharacteristicCallbacks class is responsible for handling BLE characteristic callback events.
 */
class BLEHandlerCharacteristicCallbacks : public BLECharacteristicCallbacks
{
public:
  /**
   * @brief Callback function for when a characteristic is written to.
   * @param pCharacteristic Pointer to the characteristic that was written to.
   */
  void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;

  /**
   * @brief Callback function for when a characteristic is read.
   * @param pCharacteristic Pointer to the characteristic that was read.
   */
  void onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;
};

/**
 * @class BLEHandlerServerCallbacks
 * @brief The BLEHandlerServerCallbacks class is responsible for handling BLE server callback events.
 */
class BLEHandlerServerCallbacks : public BLEServerCallbacks
{
public:
  /**
   * @brief Callback function for when a device connects to the server.
   * @param pServer Pointer to the BLE server.
   */
  void onConnect(BLEServer *pServer) override;

  /**
   * @brief Callback function for when a device disconnects from the server.
   * @param pServer Pointer to the BLE server.
   */
  void onDisconnect(BLEServer *pServer) override;
};

/**
 * @class BLEHandler
 * @brief The BLEHandler class is responsible for initializing and managing BLE services and characteristics, and handling BLE events.
 */
class BLEHandler
{
public:
  /**
   * @brief Constructor initializes member variables.
   */
  BLEHandler();

  /**
   * @brief Initializes the BLE device with the given device name.
   * @param deviceName The name of the BLE device.
   * @param service_uuid The UUID of the BLE service (default is SERVICE_UUID).
   * @param characteristic_uuid The UUID of the BLE characteristic (default is CHARACTERISTIC_UUID).
   */
  void begin(const std::string &deviceName, const std::string &service_uuid = SERVICE_UUID, const std::string &characteristic_uuid = CHARACTERISTIC_UUID);

  /**
   * @brief Starts BLE advertising.
   */
  void startAdvertising();

  /**
   * @brief Sends a notification with the given value.
   * @param value The value to be sent as a notification.
   */
  void notify(const std::string &value);

  /**
   * @brief Checks if a device is connected.
   * @return True if a device is connected, false otherwise.
   */
  bool isDeviceConnected();

private:
  BLEServer *pServer;                 ///< Pointer to the BLE server.
  BLECharacteristic *pCharacteristic; ///< Pointer to the BLE characteristic.
  bool deviceConnected;               ///< Flag indicating if a device is connected.
  bool oldDeviceConnected;            ///< Flag indicating if a device was previously connected.
  std::string receivedValue;          ///< The value received from the BLE characteristic.

  // Callback objects
  BLEHandlerCharacteristicCallbacks characteristicCallbacks;
  BLEHandlerServerCallbacks serverCallbacks;

  /**
   * @brief Creates the BLE server and sets its callbacks.
   */
  void createBLEServer();

  /**
   * @brief Creates the BLE service and characteristic, and sets their callbacks.
   * @param service_uuid The UUID of the BLE service.
   * @param characteristic_uuid The UUID of the BLE characteristic.
   */
  void createBLEServiceAndCharacteristic(const std::string &service_uuid, const std::string &characteristic_uuid);
};

#endif // BLEHANDLER_H
