// BLEManager.cpp
#include "BLEManager.h"

// Declare global variables
BLECharacteristic *pCharacteristicNotify;
BLECharacteristic *pCharacteristicWrite;
bool deviceConnected = false;
bool notificationsEnabled = false;

// Button button1; 
// Button button2;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChar) override {
        std::string value = pChar->getValue();
        const char* valueCStr = value.c_str();
        Serial.print("Received command: ");
        Serial.println(value.c_str());

        if (strcmp(valueCStr, "ENABLE") == 0) {
            notificationsEnabled = true;
            Serial.println("Notifications enabled");
        } else if (strcmp(valueCStr, "DISABLE") == 0) {
            notificationsEnabled = false;
            Serial.println("Notifications disabled");
        } else {
            if (value.length() == 4) {
                int receivedValue = (static_cast<uint8_t>(value[3]) << 24) |
                                    (static_cast<uint8_t>(value[2]) << 16) |
                                    (static_cast<uint8_t>(value[1]) << 8) |
                                    static_cast<uint8_t>(value[0]);
                Serial.print("Received integer value: ");
                Serial.println(receivedValue);
            } else {
                Serial.println("Received data length mismatch.");
            }
        }
    }

    void onRead(BLECharacteristic *pChar) override {
        Serial.println("Read request received");
    }
};

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Connected, Let's Go...");
    };

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        BLEDevice::startAdvertising();
        Serial.println("Disconnected, restarting advertising...");
    }
};

void setupBLE() {
    BLEDevice::init("MyESP32");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristicNotify = pService->createCharacteristic(
                            CHARACTERISTIC_UUID_NOTIFY,
                            BLECharacteristic::PROPERTY_NOTIFY
                        );
    pCharacteristicNotify->addDescriptor(new BLE2902());

    pCharacteristicWrite = pService->createCharacteristic(
                            CHARACTERISTIC_UUID_WRITE,
                            BLECharacteristic::PROPERTY_WRITE
                        );
    pCharacteristicWrite->setCallbacks(new MyCallbacks());

    pService->start();
    BLEDevice::startAdvertising();
    Serial.println("Waiting for a client connection to notify...");
}

void notifyClients() {
    if (deviceConnected && notificationsEnabled) {
        int valueToSend = 42;  // Example value
        uint8_t intData[4];
        intData[0] = valueToSend & 0xFF;
        intData[1] = (valueToSend >> 8) & 0xFF;
        intData[2] = (valueToSend >> 16) & 0xFF;
        intData[3] = (valueToSend >> 24) & 0xFF;

        pCharacteristicNotify->setValue(intData, sizeof(intData));
        pCharacteristicNotify->notify();
        Serial.print("Notified integer value: ");
        Serial.println(valueToSend);
    }
}
