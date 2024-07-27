#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

/**
 * @class BLEHandler
 * @brief Handles Bluetooth Low Energy (BLE) server functionalities.
 *
 * This class is designed to create and manage a BLE server, including
 * services and characteristics for data transmission. It includes callbacks
 * for handling characteristic read events and connection state changes.
 */
class BLEHandler : public BLECharacteristicCallbacks, public BLEServerCallbacks
{
public:
    /**
     * @brief Constructor for the BLEHandler class.
     *
     * Initializes member variables associated with the BLE server 
     * and characteristic.
     */
    BLEHandler();

    /**
     * @brief Initializes the BLE device with the specified name, service UUID, and characteristic UUID.
     *
     * @param deviceName The name of the BLE device.
     * @param service_uuid The UUID of the service to be created.
     * @param characteristic_uuid The UUID of the characteristic to be created.
     */
    void begin(const std::string &deviceName, const std::string &service_uuid, const std::string &characteristic_uuid);

    /**
     * @brief Starts advertising the BLE service with the specified UUID.
     *
     * This function makes the device discoverable to other BLE clients.
     */
    void startAdvertising();

    // BLECharacteristicCallbacks
    /**
     * @brief Handles read requests from a client on the characteristic.
     *
     * @param pCharacteristic Pointer to the characteristic which is being read.
     * @param param Parameters associated with the read request.
     */
    void onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) override;

    // BLEServerCallbacks
    /**
     * @brief Called when a client connects to the BLE server.
     *
     * @param pServer Pointer to the BLE server that has accepted the connection.
     */
    void onConnect(BLEServer *pServer) override;

    /**
     * @brief Called when a client disconnects from the BLE server.
     *
     * @param pServer Pointer to the BLE server that the client disconnected from.
     */
    void onDisconnect(BLEServer *pServer) override;

private:
    /**
     * @brief Creates a BLE server instance.
     *
     * Initializes the underlying BLE server and sets the necessary callbacks.
     */
    void createBLEServer();

    /**
     * @brief Creates a BLE service and characteristic.
     *
     * @param service_uuid The UUID of the service to create.
     * @param characteristic_uuid The UUID of the characteristic to create.
     */
    void createBLEServiceAndCharacteristic(const std::string &service_uuid, const std::string &characteristic_uuid);

    // Member variables
    BLEServer *pServer;             ///< Pointer to the BLE server instance.
    BLECharacteristic *pCharacteristic; ///< Pointer to the created BLE characteristic.
    std::string receivedValue;      ///< Variable to store the value received from clients.
    std::string service_uuid;       ///< UUID of the BLE service.
    std::string characteristic_uuid; ///< UUID of the BLE characteristic.
};

#endif // BLE_HANDLER_H
