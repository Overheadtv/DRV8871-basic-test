#include "DoorLock.h"

// Constructor: Initialize pins and PWM channels
DoorLock::DoorLock(uint8_t driverControlPin, uint8_t driverPwmPin, uint8_t driverPwmChannel,
                   uint8_t passengerControlPin, uint8_t passengerPwmPin, uint8_t passengerPwmChannel)
    : driverControlPin(driverControlPin), driverPwmPin(driverPwmPin), driverPwmChannel(driverPwmChannel),
      passengerControlPin(passengerControlPin), passengerPwmPin(passengerPwmPin), passengerPwmChannel(passengerPwmChannel) {

    pinMode(driverControlPin, OUTPUT);
    ledcSetup(driverPwmChannel, 5000, 8);
    ledcAttachPin(driverPwmPin, driverPwmChannel);
    digitalWrite(driverControlPin, LOW);
    ledcWrite(driverPwmChannel, 0);

    pinMode(passengerControlPin, OUTPUT);
    ledcSetup(passengerPwmChannel, 5000, 8);
    ledcAttachPin(passengerPwmPin, passengerPwmChannel);
    digitalWrite(passengerControlPin, LOW);
    ledcWrite(passengerPwmChannel, 0);

    // Initialize states
	//driverState = {running, startTime, step, lockDirection}
	//running			false		Motor is not running initially
	//startTime			0			No operation has started yet
	//step				0			Step counter starts at 0 (not in sequence yet)
	//lockDirection		false		Default to unlocking (though this is irrelevant at initialization)
	
    driverState = {false, 0, 0, false};
    passengerState = {false, 0, 0, false};

    // Set/compute default motor run times
    lockMotorTime(DRIVER, 150);
    lockMotorTime(PASSENGER, 150);

}

// Check if any lock motor is currently operating
bool DoorLock::isOperating() {
    return driverState.running || passengerState.running;
}

//change lock motor runtime
void DoorLock::lockMotorTime(LockTarget target, uint16_t newTime) {
    Serial.print("Lock motor time changed to ");
    Serial.println(newTime);
    if (target == DRIVER){
        // compute motor timing
        driverFullPowerTime = newTime * 0.33;
        driverMaintainPowerTime = newTime * 0.67;
    }
    else if (target == PASSENGER) {
        // compute motor timing
        passengerFullPowerTime = newTime * 0.33;
        passengerMaintainPowerTime = newTime * 0.67;
    }
}

//change direction of lock motor
void DoorLock::toggleMotorReverse(LockTarget target) {
    if (target == DRIVER || target == BOTH) {
        driverLockMotorReverse = !driverLockMotorReverse;
    }
    if (target == PASSENGER || target == BOTH) {
        passengerLockMotorReverse = !passengerLockMotorReverse;
    }
}

// Lock function
void DoorLock::lock(LockTarget target) {
    startMotor(target, true);
}

// Unlock function
void DoorLock::unlock(LockTarget target) {
    startMotor(target, false);
}

// Start the motor operation
void DoorLock::startMotor(LockTarget target, bool lockDirection) {
    if (target == DRIVER || target == BOTH) {
        driverState = {true, millis(), 0, lockDirection};
    }
    if (target == PASSENGER || target == BOTH) {
        passengerState = {true, millis(), 0, lockDirection};
    }
}

// This function must be called in loop()
void DoorLock::update() {
    unsigned long currentMillis = millis();

    // Process Driver Lock
    if (driverState.running) {
        switch (driverState.step) {
            case 0: // Start motor
                digitalWrite(driverControlPin, (driverState.lockDirection != driverLockMotorReverse) ? HIGH : LOW);
                ledcWrite(driverPwmChannel, 250);
                driverState.startTime = currentMillis;
                driverState.step = 1;
                break;
            case 1: // Reduce power after 33% time
                if (currentMillis - driverState.startTime >= driverFullPowerTime) {
                    ledcWrite(driverPwmChannel, 100);
                    driverState.startTime = currentMillis;
                    driverState.step = 2;
                }
                break;
            case 2: // After 100ms, stop motor
                if (currentMillis - driverState.startTime >= driverMaintainPowerTime) {
                    ledcWrite(driverPwmChannel, 0);
                    digitalWrite(driverControlPin, LOW);
                    driverState.running = false;
                }
                break;
        }
    }

    // Process Passenger Lock
    if (passengerState.running) {
        switch (passengerState.step) {
            case 0: // Start motor
                digitalWrite(passengerControlPin, (passengerState.lockDirection != passengerLockMotorReverse) ? HIGH : LOW);
                ledcWrite(passengerPwmChannel, 250);
                passengerState.startTime = currentMillis;
                passengerState.step = 1;
                break;
            case 1: // After 50ms, reduce power
                if (currentMillis - passengerState.startTime >= passengerFullPowerTime) {
                    ledcWrite(passengerPwmChannel, 100);
                    passengerState.startTime = currentMillis;
                    passengerState.step = 2;
                }
                break;
            case 2: // After 100ms, stop motor
                if (currentMillis - passengerState.startTime >= passengerMaintainPowerTime) {
                    ledcWrite(passengerPwmChannel, 0);
                    digitalWrite(passengerControlPin, LOW);
                    passengerState.running = false;
                }
                break;
        }
    }
}
