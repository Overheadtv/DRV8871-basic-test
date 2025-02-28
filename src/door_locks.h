// door_locks.h
#ifndef DOOR_LOCKS // include guard
#define DOOR_LOCKS

#include <Arduino.h>
#include "globals.h"


void door_lock_action();
void LockWithDoorSwitches();
void DoorLockwithPanelSwitches(byte side, byte lock);
void DoorLockwithPanelSwitchesandTimers(byte side, byte lock);
void DoorLockWithOne5PinSwitch(byte side, byte lock_direction);
void DoorLockWithTwo5PinSwitches(byte side, byte lock);
void start_lock_motors(bool side, bool direction);
void stop_lock_motors(bool side, bool direction);
void door_locks_enable(byte side, bool direction, bool pass = 0);
void door_locks_stop(byte side = side_select);
void set_active_lock_switch_interrupts(bool side, byte switch_number);
void set_inactive_lock_switch_interrupts(bool side, byte switch_number);
void disable_lock_switch_interrupts(byte switch_number);

#endif /* door_locks */