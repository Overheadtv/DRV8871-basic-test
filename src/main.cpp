#include <Arduino.h>
#include "DoorLock.h"

// Define pins for driver and passenger locks
#define DRIVER_CONTROL_PIN 35
#define DRIVER_PWM_PIN 13
#define DRIVER_PWM_CHANNEL 0

#define PASSENGER_CONTROL_PIN 36
#define PASSENGER_PWM_PIN 14
#define PASSENGER_PWM_CHANNEL 1

#define DRIVER_LOCK_SWITCH  47
#define DRIVER_UNLOCK_SWITCH 48
#define PASSENGER_LOCK_SWITCH 45

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Create the DoorLock object
DoorLock doorLock(DRIVER_CONTROL_PIN, DRIVER_PWM_PIN, DRIVER_PWM_CHANNEL,
                  PASSENGER_CONTROL_PIN, PASSENGER_PWM_PIN, PASSENGER_PWM_CHANNEL);

void setup() {
    delay(100);  // Give the serial monitor time to initialize
    Serial.begin(115200);  // Ensure this is present and set to an appropriate baud rate
    delay(100);  // Give the serial monitor time to initialize
    Serial.println("Starting setup...");
    pinMode(DRIVER_LOCK_SWITCH, INPUT_PULLUP);
    pinMode(DRIVER_UNLOCK_SWITCH, INPUT_PULLUP);
    pinMode(PASSENGER_LOCK_SWITCH, INPUT_PULLUP);
    doorLock.driverLockMotorReverse = false;
    doorLock.passengerLockMotorReverse = true;
    doorLock.lockMotorTime(DRIVER, 1000);
    doorLock.lockMotorTime(PASSENGER, 300);
}

void loop() {
    unsigned long currentMillis = millis();

    // Check switches every debounceDelay milliseconds
    if (currentMillis - lastDebounceTime >= debounceDelay) {
        lastDebounceTime = currentMillis;

        // Read switch states
        bool driverLockPressed = digitalRead(DRIVER_LOCK_SWITCH) == LOW;
        bool driverUnlockPressed = digitalRead(DRIVER_UNLOCK_SWITCH) == LOW;
        bool passengerLockPressed = digitalRead(PASSENGER_LOCK_SWITCH) == LOW;

        // Ensure motor is not already running before triggering
        if (driverLockPressed && !doorLock.isOperating()) {
            doorLock.lock(DRIVER);
        }
        
        if (driverUnlockPressed && !doorLock.isOperating()) {
            doorLock.unlock(DRIVER);
        }
        
        if (passengerLockPressed && !doorLock.isOperating()) {
            doorLock.lock(PASSENGER);
        }
    }

    // Call update function to process door lock motor steps
    doorLock.update();
}
