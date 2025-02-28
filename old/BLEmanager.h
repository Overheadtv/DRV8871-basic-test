// BLEManager.h
#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <NimBLEDevice.h> 
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "46ae84f4-26ee-4883-bd54-719d45ab54f0"
#define CHARACTERISTIC_UUID_WRITE "cfabbb58-3da5-4071-a605-23cd731492de"
#define CHARACTERISTIC_UUID_NOTIFY "eb39c749-958a-456f-af4b-baec2e4e99af"

// Function declarations
void setupBLE();
void notifyClients();

extern bool deviceConnected;
extern bool notificationsEnabled;

// struct Button {
//   bool current_value;
//   bool last_value;
//   bool timing_active;
//   unsigned long debounce_time;
//   byte pin;
// };

// extern Button button1;
// extern Button button2;

#endif // BLE_MANAGER_H
