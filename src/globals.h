// ISR.h
#ifndef GLOBALS_H // include guard
#define GLOBALS_H

#include <Arduino.h>
//general definitions
#define Driver_Side 0
#define Passenger_Side 1
#define Lock 0
#define Unlock 1
#define CW 0
#define CCW 1
#define YES 1
#define NO 0
#define DOWN 0
#define UP 1
#define passenger_override 2
#define Side 0
//led definitions
#define LedPin 6
//beeper definitions
#define Beeper_Pin 7
#define Beeper_freq 1200
#define freq 2000
//window motor defintions
#define Driver_IBT2_UP_pin 8
#define Driver_IBT2_DOWN_pin 3
#define Passenger_IBT2_UP_pin 18
#define Passenger_IBT2_DOWN_pin 17
#define Driver_motor_current_pin 1
#define Passenger_motor_current_pin 2
#define Remote_Window_Up_pin 40
#define Remote_Window_Down_pin 39
#define auto_stop_sensitivity 1.5
//door lock definitions
#define Driver_Lock_Switch_pin 9
#define Driver_Unlock_Switch_pin 10
#define Passenger_Lock_Switch_pin 12
#define Passenger_Unlock_Switch_pin 11
#define Remote_Lock_Switch_pin 41
#define Remote_Unlock_Switch_pin 42
#define Driver_Door_Lock_Control_pin 13
#define Passenger_Door_Lock_Control_pin 14
#define Driver_Lock_PWM_pin 35
#define Passenger_Lock_PWM_pin 36
#define Door_switch_debounce_time 200   //door lock switch debounce time, 200msec
#define Door_lock_PWM_Frequency 4000
#define Door_lock_PWM_Resolution 8
#define Door_lock_mode 0
#define Door_lock_motor_run_time 100UL
//PWM channel definitions
#define Driver_Down_PWM_channel 0
#define Driver_Up_PWM_channel 1
#define Passenger_Down_PWM_channel 2
#define Passenger_Up_PWM_channel 3
#define Driver_Lock_PWM_channel 4
#define Passenger_Lock_PWM_channel 5
#define Beeper_PWM_channel 6
#define PWM_resolution 8
//various definitions
#define EEPROM_SIZE 18              // This is 18-Bytes
#define moving_avg_array_size 250   //number of current readings saved for average motor current calculation
#define motor_start_speed 145
#define door_lock_timeout 300      //set lock timeout to 300msec
#define lock_speed 120              //set lock motor speed
#define debounce_time_period 20     //set debounce period to 20msec
#define window_overrun_timeout 750  //set window overrun timeout to 750msec
#define WATCHDOG_TIMEOUT_SECONDS 5  //set watchdog timeout to 3 seconds
#define passenger_override_time 220 //set passenger override time to 220msec
#define save_switch 1               //set save switch to 1 for array index
#define save_switch_direction 2     //set save switch to 2 for array index
#define Reset_pin 37                 //set reset button pin


//declare these variables
//make external, available to all files

extern const byte button[2][6];
extern volatile bool debounce_active[2][6]; // flag 0=no debounce now,1=debounce in progress,
extern volatile bool button_state[2][6];
extern volatile bool button_change[2][6];
extern bool button_active[2][6];
extern bool button_last_valid[2][6];
extern volatile byte lock_button_active[2][2];  // flag side,0=no lock switch active,side,1=lock switch active
extern unsigned long lock_button_time[2][2];    // start time lock button pressed/released
extern byte lock_button_state[2][2];  // flag 0=no lock switch active,1=lock switch active
extern bool lock_button_last[2][2];  //last valid lock button state
extern byte window_mode[2];         //set window mode, 0=idle, 1=manual, 2=auto
extern byte motor_mode[2];          //set motor mode, 0=off, 1=starting, 2=running
extern byte direction_mode[2];           //set direction mode, 0=off, 1=down, 2=up
extern byte motor_speed[2];         //set motor speed, 0-255
//extern const int freq;  //set PWM frequency n
extern int Beep_Duty_Cycle;    //set initial duty cycle value
extern byte switch_number; // select the switch number 1=manual down, 2=manual up, 3=auto
extern bool side_select;   // select 0=driver or 1=passenger side
extern unsigned long current_loop_time;
extern unsigned long lock_motor_start_time[2];            // lock emergency time out timer
extern volatile unsigned int interruptCount[2]; 
extern unsigned int Driver_currentPin;
extern int motor_current_alarm[2][2];   
extern volatile bool read_motor_current[2];
extern int Readings[2][moving_avg_array_size];
extern byte position_mode[2];
extern byte motor_current_alarm_count[2];
//extern int motor_current;
extern int beep_alarm[10][2];
extern bool beep_alarm_enable;
extern byte timer_id;
extern int prescaler;
extern int threshold;
extern bool door_lock_active[2];
extern byte door_lock_direction[2];
extern byte door_lock_side;
extern int Lock_Motor_Speed;
extern bool lock_emergency_timer_active[2];
extern bool lock_motor_active[2];
extern unsigned long window_run_start_time[2];
extern bool window_run_timer[2];
extern unsigned int window_run_time_alarm[2][2]; //[0=driver, 1=passenger], [0=down, 1=up]
extern bool window_time_save[2]; 
extern bool passenger_override_timecheck;
extern unsigned long passenger_override_start_time;
extern bool passenger_override_active;
extern byte passenger_override_status[2];
extern unsigned long passenger_override_timeout_start_time; //passenger override timeout timer start time
extern bool passenger_override_timeout_active; //flag to indicate passenger override timeout is active
//extern byte beep_count;
extern volatile byte reset_pin_active;
extern volatile unsigned long reset_start_time;
extern volatile unsigned long reset_button_time; //reset button debounce start time
extern volatile byte doorLockStates;

extern unsigned long lock_emergency_time_start[2];
extern volatile bool notifyFlag;
extern int notifyValue;
extern bool door_lock_status;
// Global variables to store received data and flag
extern volatile bool dataReceived;
extern volatile uint8_t receivedByte;
extern volatile uint32_t receivedInt;
extern bool BLEbyteReceived;
extern byte BLEside;
extern byte BLEswitch; 
extern volatile byte RMT_control; 
extern unsigned long start_delay_time[2];
extern bool BLESwitchActive;
extern byte switch_starting[2];
extern volatile bool ext_control; 
extern volatile byte ext_function;
extern unsigned long extDebounceTime[4]; // Debounce timestamps for each switch
extern const int extPins[4];
extern float drv_up_m, drv_up_b, drv_down_m, drv_down_b, psg_up_m, psg_up_b, psg_down_m, psg_down_b;
struct RegressionResult {
    float A;
    float B;
};
extern RegressionResult coefficientsDriverUp;
extern RegressionResult coefficientsDriverDown;
extern RegressionResult coefficientsPassengerUp;
extern RegressionResult coefficientsPassengerDown;
extern byte notify_data[4];
extern byte door_lock_mode;
extern volatile int test_1; 
extern volatile int test_2;
extern volatile int test_3;
extern volatile int test_4;
extern volatile byte test_ISR[4]; 
extern volatile byte remote_control_state[2][2];
extern volatile byte rmt_control_busy;
extern volatile byte remote_number;

#endif