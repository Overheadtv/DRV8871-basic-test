
#include <Arduino.h>
#include "setup_routine.h"
#include "globals.h"
#include "esp_task_wdt.h"
//   variables declared in globals.h
//   variables defined in globals.cpp

void setup()
{
    setup_routine();    //setup routine
    //setupBLE();         //setup BLE

    // Setup PWM for MOTOR_IN1
    ledcSetup(Driver_Lock_PWM_channel, Door_lock_PWM_Frequency, Door_lock_PWM_Resolution);
    ledcAttachPin(Driver_Lock_PWM_pin, Driver_Lock_PWM_channel);

    ledcSetup(Passenger_Door_Lock_Control_pin, Door_lock_PWM_Frequency, Door_lock_PWM_Resolution);
    ledcAttachPin(Passenger_Lock_PWM_pin, Passenger_Lock_PWM_channel);
    digitalWrite(Passenger_Door_Lock_Control_pin, LOW); // Set IN2 low
    ledcWrite(Passenger_Lock_PWM_channel, 0); // Apply PWM to IN1
    
}

void loop()
{
  // Run motor in forward
  // Driver lock
  digitalWrite(Driver_Door_Lock_Control_pin, HIGH); // Set IN2 high
  ledcWrite(Driver_Lock_PWM_channel, 250); // Apply PWM to IN1
  // Passenger unlock
  digitalWrite(Passenger_Door_Lock_Control_pin, LOW); // Set IN2 high
  ledcWrite(Passenger_Lock_PWM_channel, 250); // Apply PWM to IN1
  //full power delay
  delay(50); // Run locks full power for 50 msec
  // Driver lock
  ledcWrite(Driver_Lock_PWM_channel, 100); // Apply PWM to IN1
  // Passenger Unlock
  ledcWrite(Passenger_Lock_PWM_channel, 100); // Apply PWM to IN1
  //low power delay
  delay(100); // Run lock low power for 100 msec

  // Stop motors
  digitalWrite(Driver_Door_Lock_Control_pin, LOW); // Set IN2 low
  ledcWrite(Driver_Lock_PWM_channel, 0); // Remove PWM from IN1
  ledcWrite(Passenger_Lock_PWM_channel, 0); // Remove PWM from IN1
  delay(1500); // Stop for 1.5 second
 
  // Run motor in reverse
  // Driver unlock
  digitalWrite(Driver_Door_Lock_Control_pin, LOW); // Set IN2 high
  ledcWrite(Driver_Lock_PWM_channel, 250); // Apply PWM to IN1
  // Passenger lock
  digitalWrite(Passenger_Door_Lock_Control_pin, HIGH); // Set IN2 high
  ledcWrite(Passenger_Lock_PWM_channel, 250); // Apply PWM to IN1
  //full power delay
  delay(50); // Run locks full power for 50 msec
  // Driver lock
  ledcWrite(Driver_Lock_PWM_channel, 100); // Apply PWM to IN1
  // Passenger Unlock
  ledcWrite(Passenger_Lock_PWM_channel, 100); // Apply PWM to IN1
  //low power delay
  delay(100); // Run lock low power for 100 msec
  

  // Stop motors
  digitalWrite(Passenger_Door_Lock_Control_pin, LOW); // Set IN2 low
  ledcWrite(Driver_Lock_PWM_channel, 0); // Remove PWM from IN1
  ledcWrite(Passenger_Lock_PWM_channel, 0); // Remove PWM from IN1
  delay(1500); // Stop for 1.5 second

    
}