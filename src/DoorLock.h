#ifndef DOORLOCK_H
#define DOORLOCK_H

#include <Arduino.h>

enum LockTarget { DRIVER, PASSENGER, BOTH };

class DoorLock {
public:
    DoorLock(uint8_t driverControlPin, uint8_t driverPwmPin, uint8_t driverPwmChannel,
             uint8_t passengerControlPin, uint8_t passengerPwmPin, uint8_t passengerPwmChannel);

    void lock(LockTarget target);
    void unlock(LockTarget target);
    void update(); // Call this in loop()
    void toggleMotorReverse(LockTarget target);
    void lockMotorTime(LockTarget target, uint16_t newTime);
    bool isOperating(); // Check if any motor is running
    bool driverLockMotorReverse;   // true = lock, false = unlock
    bool passengerLockMotorReverse;
        

private:
    struct LockState {
        bool running;
        unsigned long startTime;
        uint8_t step;
        bool lockDirection;
    };
    

    void startMotor(LockTarget target, bool lockDirection);

    uint8_t driverControlPin, driverPwmPin, driverPwmChannel;
    uint8_t passengerControlPin, passengerPwmPin, passengerPwmChannel;

    //initialize values for motor run time
    uint16_t driverFullPowerTime;
    uint16_t driverMaintainPowerTime;
    uint16_t passengerFullPowerTime;
    uint16_t passengerMaintainPowerTime;

    LockState driverState, passengerState;
};

#endif // DOORLOCK_H
