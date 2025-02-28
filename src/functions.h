// FUNCTIONS_H
#ifndef FUNCTIONS_H // include guard
#define FUNCTIONS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "globals.h"



void LedBlink(byte side, byte blink, int rates);
bool Valid_Button(byte side_selected, byte switch_selected);
void set_mode_flags(byte set_window_mode, byte set_motor_mode, byte set_auto_mode, byte window_pos, byte set_side);
byte moving_average(byte side, int new_value, bool index_return = false);
void writeIntIntoEEPROM(int address, int number);
int readIntFromEEPROM(int address);
int average_current();
int calc_motor_avg_current();
void sound_alarm();
void print_status();
void calc_window_avg_time(byte side, byte direction);
void save_window_position(byte side, byte position);
void reset_override_timer();
void print_Poverride_status();
void sound_assign(byte sound_number);
void calculateMotorData();
void calculateLinearRegression(const int* adc_values, const float* current_values, int n, float& m, float& b);
void printLinearRegressionParams(const char* label, float m, float b);
float adcToCurrent(int adc_value, float m, float b);
float calculateCurrent(int adcValue, float A, float B);
RegressionResult exponentialRegression(int x[], float y[], int n);
void notify_current();
void reset_action();
void ext_control_action();
void ext_control_active();
void printBinary(uint8_t value);
void door_lock_action(byte side, byte which_lock);


#endif /* FUNCTIONS_H */

