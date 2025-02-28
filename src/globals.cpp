#include <Arduino.h>
#include "globals.h"

/**
 * button: 2D array holding pin assignments for buttons
 * debounce_active: 2D array tracking if button debounce is in progress
 * button_state: 2D array tracking current button state
 * button_change: 2D array tracking if button state changed
 * window_mode: 2D array tracking window mode (idle, manual, auto)
 * motor_mode: Array tracking motor mode (off, starting, running)
 * direction: Array tracking auto mode (auto down, auto up)
 * motor_speed: Array tracking motor speed value
 * button_active: 2D array tracking if button is active
 * freq: PWM frequency value
 * Speed: Duty cycle value for PWM
 * switch_number: Selected switch number
 * side_select: Selected side (driver/passenger)
 * current_loop_time: Current time value
 * interruptCount: Interrupt counter
 * Driver_currentPin: Driver side current pin
 * motor_current_alarm: 2D array of motor current alarm thresholds
 * read_motor_current: Array tracking if motor current is read
 * READINGS: 2D array holding motor current readings
 * position_mode: Array tracking window position
 */
// const byte button[2][5] = {{0, 33, 32, 35, 17}, {0, 34, 22, 23, 16}}; // button pin assignments
const byte button[2][6] = {{0, 4, 5, 21, 9, 10}, {0, 47, 48, 45, 12, 11}}; // button pin assignments
//const byte button[2][6] = {{0, 4, 5, 21, 0, 0}, {0, 47, 48, 45, 0, 0}}; // button pin assignments
volatile bool debounce_active[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};    // flag 0=no debounce now,1=debounce in progress,
volatile bool button_state[2][6] = {{0, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 0, 0}};       // read button state
volatile bool button_change[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};      // flag that state changed
bool button_active[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};      //flag that button is debounced & active
bool button_last_valid[2][6] = {{0, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 0, 0}};
volatile byte lock_button_active[2][2] = {{0, 0}, {0, 0}};  // flag 0=no lock switch active,1=lock switch active
unsigned long lock_button_time[2][2] = {{0, 0}, {0, 0}}; // start time lock button pressed/released
byte lock_button_state[2][2] = {{1, 1}, {1, 1}};  // flag 0=no lock switch active,1=lock switch active
bool lock_button_last[2][2]= {{1, 1}, {1, 1}};  //last valid lock button state
byte window_mode[2] = {0, 0};                                 // set window mode, 0=idle, 1=manual, 2=auto
byte motor_mode[2] = {0, 0};                                  // set motor mode, 0=off, 1=starting, 2=running
byte direction_mode[2] = {1, 1};                              // set direction mode, 0=off, 1=down, 2=up
byte position_mode[2] = {0, 0};                               // set position mode, 0=partial, 1=full down, 2=full up
byte motor_speed[2] = {0, 0};                                 // set motor speed, 0-255
bool door_lock_active[2] = {false, false};
byte door_lock_direction[2];
byte door_lock_side;
//const int freq = 2000;  // set PWM frequency
int Beep_Duty_Cycle = 0;                  // set initial duty cycle value for PWM
byte switch_number = 1;             //switch flag 1=manual down, 2=manual up, 3=auto
bool side_select = 0;                   // select 0=driver or 1=passenger side
unsigned long current_loop_time;
unsigned long lock_motor_start_time[2];            // lock emergency time out timer
volatile unsigned int interruptCount[2] = {0,0}; //counter to track PWM interrupts
//unsigned int Driver_currentPin = 0;
int motor_current_alarm[2][2]; // alarm thresholds for motor current-0,0 dr down, 0,1 dr up, 1,0 ps dn, 1,1 ps up
volatile bool read_motor_current[2] = {};
int Readings[2][moving_avg_array_size] = {};
bool lock_emergency_timer_active[2] = {0, 0};      //init lock timer flag to false
bool lock_motor_active[2] = {0, 0};      //init lock motor status to false
byte motor_current_alarm_count[2] = {0, 0};
//int motor_current;                                                                         
int beep_alarm[10][2];  //count, {freq, duration}
bool beep_alarm_enable = false;
byte timer_id = 0;
int prescaler = 80; // Between 0 and 65 535
int threshold = 1000000; // 64 bits value (limited to int size of 32bits)
bool window_run_timer[2] = {0, 0};
bool window_time_save[2] = {0, 0};      //flag to save window run time
unsigned long window_run_start_time[2] = {0, 0};
unsigned int window_run_time_alarm[2][2] = {0}; //[0=driver, 1=passenger], [0=down, 1=up]
bool passenger_override_timecheck = false;     //flag to override time check is active
unsigned long passenger_override_start_time = 0;    //time when passenger override started
bool passenger_override_active = false;        //flag to indicate passenger override is active
byte passenger_override_status[2] = {0};        //save switch status at time of override start
unsigned long passenger_override_timeout_start_time = 0; //passenger override timeout timer start time
bool passenger_override_timeout_active = false; //flag to indicate passenger override timeout is active
//byte beep_count = 0; //pointer for beep array index
volatile byte reset_pin_active = 0; //flag to track if reset pin is active
volatile unsigned long reset_start_time = 0; //reset button timer start time
volatile unsigned long reset_button_time = 0; //reset button debounce start time
volatile byte doorLockStates=0;
int Lock_Motor_Speed = 128;
unsigned long lock_emergency_time_start[2];
volatile bool notifyFlag = false;
int notifyValue = 0;
bool door_lock_status = false;  //door lock status, 0=unlocked, 1=locked
// Global variables to store received data and flag
volatile bool dataReceived = false;
volatile uint8_t receivedByte = 0;
volatile uint32_t receivedInt = 0;
bool BLEbyteReceived = false;
byte BLEside = 0;
byte BLEswitch = 0;
volatile byte RMT_control = 0; 
unsigned long start_delay_time[2];
bool BLESwitchActive = false;
byte switch_starting[2] = {0, 0};
volatile bool ext_control = false; 
volatile byte ext_function = 0;
unsigned long extDebounceTime[4] = {0, 0, 0, 0}; // Debounce timestamps for each switch
const int extPins[4] = {Remote_Unlock_Switch_pin, Remote_Lock_Switch_pin, Remote_Window_Up_pin, Remote_Window_Down_pin};
float drv_up_m, drv_up_b, drv_down_m, drv_down_b, psg_up_m, psg_up_b, psg_down_m, psg_down_b;   // Slopes and intercepts for each set of current data points
RegressionResult coefficientsDriverUp;
RegressionResult coefficientsDriverDown;
RegressionResult coefficientsPassengerUp;
RegressionResult coefficientsPassengerDown;
byte notify_data[4];
byte door_lock_mode = 0;
volatile int test_1=0;
volatile int test_2=0;
volatile int test_3=0;
volatile int test_4=0;
volatile byte test_ISR[4]={0,0,0,0};
volatile byte remote_control_state[2][2] = {{0,0}, {0,0}};
volatile byte rmt_control_busy = 0;
volatile byte remote_number = 0;