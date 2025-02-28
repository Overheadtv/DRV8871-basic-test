
#include "door_locks.h"

#include "functions.h"
#include "globals.h"



void DoorLockwithPanelSwitches(byte side, byte lock_direction) //side, 0=driver/1=passenger, lock, 0=lock/1=unlock
    //void door_lock_action(byte side, byte lock_direction) //side, 0=driver/1=passenger, lock, 0=lock/1=unlock
{
    //test what state lock button is in
    switch(lock_button_active[side][lock_direction])
    {
        case 1:
            //save start debounce time
            lock_button_time[side][lock_direction] = current_loop_time;
            //set button state to next state
            lock_button_active[side][lock_direction] = 2;
            break;
        
        case 2:
            //test if debounce time has passed
            if(current_loop_time - lock_button_time[side][lock_direction] > 50UL)
            {
                //read selected pin value
                bool pin_value = digitalRead(button[side][lock_direction + 4]);
                //test if button state has changed
                if(pin_value != lock_button_last[side][lock_direction])
                {
                    //valid button detected
                    //check which press or release
                    if(pin_value == 0)
                    {
                        //button pressed
                        //side, 0=driver, 1=passenger, 2=both
                        //direction, 0=lock, 1=unlock
                        //start lock motor
                        door_locks_enable(2, lock_direction);
                        // //save lock motor start time
                        // lock_motor_start_time[side] = current_loop_time;         
                        // //set lock motor active
                        // lock_motor_active[side] = true;
                        // //set lock timer active
                        // lock_emergency_timer_active[side] = true;
                        //set new button state value
                        lock_button_active[side][lock_direction] = 3;
                    }
                    else
                    {
                        //button released
                        //lock motor not started
                        //reset button state value
                        lock_button_active[side][lock_direction] = 0;
                    }
                    //save current valid button state
                    lock_button_last[side][lock_direction] = pin_value;
                    
                }
                else
                {
                    //non valid button detected
                    //reset lock button state value for selected switch
                    lock_button_active[side][lock_direction] = 0;
                }
                
            }
            else
            {
                //debounce time not passed yet, no action
            }
            break;

        case 3:
            // Serial.print("testing lock motor timer ");
            // unsigned long display_time = current_loop_time - lock_motor_start_time[side] ;
            // side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            // Serial.println(display_time);
            //test if lock motor run time has passed
            if(current_loop_time - lock_motor_start_time[side] > Door_lock_motor_run_time)
            {
                //stop lock motors, clear active flags
                door_locks_stop();
                //door lock action complete, clear button state value
                lock_button_active[side][lock_direction] = 0;
            }
            else
            {
                //motor run time not passed yet, no action
            }
            break;
    }
}


//lock switch states set by ISR, 0=no action, 1=driver lock, 2=driver unlock, 3=passenger lock, 4=passenger unlock
void DoorLockwithPanelSwitchesandTimers(byte side, byte lock_direction)
{
    static unsigned long driver_lock_override_timer_start = 0;
    static bool driver_pass_count = 0;
    //test what state lock button is in
    switch(lock_button_active[side][lock_direction])
    {
        case 1:
            //save start debounce time
            lock_button_time[side][lock_direction] = current_loop_time;
            //set button state to next state
            lock_button_active[side][lock_direction] = 2;
            break;
        
        case 2:
            //test if debounce time has passed
            //Serial.println("Door lock button debounce test");
            if(current_loop_time - lock_button_time[side][lock_direction] > 50UL)
            {
                //read selected pin value
                bool pin_value = digitalRead(button[side][lock_direction + 4]);
                // Serial.print("Door lock button state: ");
                // Serial.println(pin_value);
                //test if button state has changed
                if(pin_value != lock_button_last[side][lock_direction])
                {
                    //valid button detected
                    //save current valid button state
                    lock_button_last[side][lock_direction] = pin_value;
                    //check which press or release
                    if(pin_value == 0)
                    {
                        //button pressed
                        //side, 0=driver, 1=passenger, 2=both
                        //direction, 0=lock, 1=unlock
                        //start lock motor
                        if(side == Driver_Side)
                        {
                            //test if this is first time driver is pressed
                            if(driver_pass_count == 0)
                            {
                                //Serial.println("Driver lock first pass");
                                //initial pass, operate driver lock
                                //set first pass flag
                                driver_pass_count = 1;
                                //save driver lock override start time
                                driver_lock_override_timer_start = current_loop_time;
                                //start lock motor
                                door_locks_enable(Driver_Side, lock_direction, driver_pass_count);
                                // //save lock motor start time
                                // lock_motor_start_time[side] = current_loop_time;         
                                // //set lock motor active
                                // lock_motor_active[side] = true;
                                // //set lock timer active
                                // lock_emergency_timer_active[side] = true;
                                //set state value to test if motor timer expired
                                lock_button_active[side][lock_direction] = 3;
                                
                            }
                            else
                            {
                                //Serial.println("Driver lock second pass");
                                //second pass, operate passenger lock
                                //clear first pass flag
                                driver_pass_count = 0;
                                //start lock motor
                                door_locks_enable(Passenger_Side, lock_direction, driver_pass_count);
                                // //save lock motor start time
                                // lock_motor_start_time[Passenger_Side] = current_loop_time;         
                                // //set lock motor active
                                // lock_motor_active[Passenger_Side] = true;
                                // //set lock timer active
                                // lock_emergency_timer_active[Passenger_Side] = true;
                                //set state value to test if motor timer expired
                                lock_button_active[Passenger_Side][lock_direction] = 3;
                                //terminate driver state value
                                lock_button_active[Driver_Side][lock_direction] = 0;
                                lock_button_active[Driver_Side][!lock_direction] = 0;
                                
                                return;

                            }
                            
                        }
                        else
                        {
                            //test is driver first pass active
                            if(driver_pass_count == 1)
                            {
                                //cancel driver button active
                                lock_button_active[Driver_Side][Lock] = 0;
                                lock_button_active[Driver_Side][Unlock] = 0;  
                            }
                            //Serial.println("Passenger lock motor started");
                            //passenger side operate lock motor
                            //start lock motor
                            door_locks_enable(Passenger_Side, lock_direction);
                            // //save lock motor start time
                            // lock_motor_start_time[side] = current_loop_time;         
                            // //set lock motor active
                            // lock_motor_active[side] = true;
                            // //set lock timer active
                            // lock_emergency_timer_active[side] = true;
                            //set state value to test if motor timer expired
                            lock_button_active[side][lock_direction] = 3;
                        }
                    }
                    else
                    {
                        //button released
                        //lock motors not started
                        //reset button state value if motor is not active
                        if(lock_motor_active[side] == false)
                        {
                            if(side == Driver_Side)
                            {
                                if(driver_pass_count == 1)
                                {
                                    //driver button first pass
                                    //do not reset state value 
                                    //restore state value
                                    lock_button_active[side][lock_direction] = 4;
                                } 
                                else
                                {
                                    //driver button second pass
                                    //reset state value
                                    lock_button_active[side][lock_direction] = 0;
                                } 
                            }
                            else
                            {
                                //reset lock button state value for selected switch
                                lock_button_active[side][lock_direction] = 0;
                                //reset the driver pass counter
                                driver_pass_count = 0;
                            }
                            
                        }
                    }
                }
                else
                {
                    //non valid button detected
                    //reset lock button state value for selected switch
                    lock_button_active[side][lock_direction] = 0;
                }
                
            }
            else
            {
                //debounce time not passed yet, no action
            }
            break;

        case 3:
            {
            // Serial.print("testing lock motor timer ");
            // long unsigned display_time = current_loop_time - lock_motor_start_time[side];
            // side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            // Serial.println(display_time);
            //test if lock motor run time has passed
            if(current_loop_time - lock_motor_start_time[side] > Door_lock_motor_run_time)
            {
                //stop lock motors, clear active flags
                door_locks_stop();
                //check which side is active
                if(side == Driver_Side)
                {
                    //Serial.println("Driver motor stopped, mode=4");
                    //driver door lock action complete
                    if(driver_pass_count == 1)  //driver button first pass
                    {
                        //set state value to test if driver override timer expired
                        lock_button_active[side][lock_direction] = 4;
                    }
                    else
                    {
                        //clear button state value
                        lock_button_active[side][lock_direction] = 0;
                        //reset the driver pass counter
                        driver_pass_count = 0;
                    }
                }
                else
                {
                    //passenger door lock action complete
                    //clear button state value
                    lock_button_active[side][lock_direction] = 0;
                    //reset the driver pass counter
                    driver_pass_count = 0;
                    //set lock timer active
                    lock_emergency_timer_active[side] = false;
                    // Serial.println("Passenger motor stopped, mode=0");
                    // Serial.print("Side - ");
                    // Serial.print(side);
                    // Serial.print(", Lock - ");
                    // Serial.print(lock_direction);
                    // Serial.print(", Lock button state = ");
                    // Serial.println(lock_button_active[side][lock_direction]);
                }
            }
            else
            {
                //motor run time not passed yet, no action
            }
            }
            break;

        case 4:
            //test if driver override time has passed
            if(current_loop_time - driver_lock_override_timer_start > 2000UL)
            {
                //Serial.println("Driver lock override time expired");
                //clear driver lock timer
                driver_lock_override_timer_start = 0;
                //clear button state value
                lock_button_active[side][lock_direction] = 0;
                //reset the driver pass counter
                driver_pass_count = 0;
                //sound alarm
                sound_assign(12);
            }   
            break;    
    }
}  

void DoorLockWithTwo5PinSwitches(byte side, byte lock_direction)
{
    //test what state lock button is in
    switch(lock_button_active[side][lock_direction])
    {
        case 1:     //start debounce sequence or stop lock motor
            //save start debounce time
                lock_button_time[side][lock_direction] = current_loop_time;
                //set button state to next state
                lock_button_active[side][lock_direction] = 2;

            //this code is for using lock switch to turn motor off when interrupted    
            /* //test if lock motor is active
            if(lock_motor_active[side] == 1)
            {
                //lock motor is active, stop lock motor
                door_locks_stop();
                //reset lock button state value
                lock_button_active[side][lock_direction] = 5;
            }
            else
            {
                //save start debounce time
                lock_button_time[side][lock_direction] = current_loop_time;
                //set button state to next state
                lock_button_active[side][lock_direction] = 2;
            } */
            break;

        case 2:     //debounce timer and motor start
        {
            /* //use this to display lock button time in terminal 
            long unsigned display_time = current_loop_time - lock_button_time[side][lock_direction];
            Serial.print("testing debounce timer ");
            side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            Serial.println(display_time); */
            //test if debounce time, 20msec, has passed
            if(current_loop_time - lock_button_time[side][lock_direction] > 20UL)
            {
                //read selected switch value
                bool pin_value = digitalRead(button[side][lock_direction + 4]);
                //button debounced, save current value
                button_last_valid[side][lock_direction + 4] = pin_value;
                //check which press or release
                if(pin_value == 1)  //press
                {
                    //a valid button press detected, set state to motor start
                    lock_button_active[side][lock_direction] = 3;
                        
                }
                else    //release
                {
                    //a non valid button press detected
                    //set state value for selected switch to complete and reset
                    lock_button_active[side][lock_direction] = 5;
                }
            }
            else
            {
                //debounce time not passed yet, no action
            }
        
            break;
        }
        case 3:     //prepare and start lock motor
        {
            //first, check if already in requested position
            //then, disable opposite side button press interrupt
            //read button value of opposite button request
            bool opposite_side = !side;
            if(button_last_valid[opposite_side][lock_direction + 4] == 0)
            {
                //lock is not is correct position
                //disable interrupt for opposite side switch
                detachInterrupt(button[opposite_side][lock_direction + 4]);
                //start lock motor
                door_locks_enable(opposite_side, lock_direction);
                //set state value to wait til motor run timer has expired
                lock_button_active[side][lock_direction] = 4;
            }
            else
            {
                //lock already in proper position
                //set state value to finish and set flags and interrupts for next event
                lock_button_active[side][lock_direction] = 5;
                Serial.println("Lock action cancelled as already in place");
            }
            break;
        }

        case 4: //test if lock motor timer has expired
        {
            /* long unsigned display_time = current_loop_time - lock_motor_start_time[!side];
            !side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            Serial.println(display_time); */
            //test if lock motor run time has passed
            if(current_loop_time - lock_motor_start_time[!side_select] > Door_lock_motor_run_time)
            {
                //stop lock motors, clear active flags
                door_locks_stop(!side);
                //set state value for selected switch to complete and reset
                lock_button_active[side][lock_direction] = 5;
            }
            else
            {
                //motor run time not passed yet, no action
            }
            break;
        } 
            
        case 5:     //lock motor complete, reset flags & prepare for next event
            //read & set last state values
            button_last_valid[0][4] =  digitalRead(button[0][4]);   //driver lock switch
            button_last_valid[0][5] =  digitalRead(button[0][5]);   //driver unlock switch
            button_last_valid[1][4] =  digitalRead(button[1][4]);   //passenger lock switch
            button_last_valid[1][5] =  digitalRead(button[1][5]);   //passenger unlock switch

            //set interrupts based on door lock switch position
            //if lock switch active(1) then disable lock interrupt & vice versa
            if(button_last_valid[0][4] == 1)    //driver side lock
            {
                detachInterrupt(button[0][4]);
                // attachInterrupt(button[0][5], button8_ISR, RISING);
                test_ISR[0]=0;
                test_ISR[1]=0;
            }

            if(button_last_valid[0][5] == 1)    //driver side unlock
            {
                detachInterrupt(button[0][5]);
                // attachInterrupt(button[0][4], button7_ISR, RISING);
                test_ISR[0]=0;
                test_ISR[1]=0;
            } 

            if(button_last_valid[1][4] == 1)    //passenger side lock
            {
                detachInterrupt(button[1][4]);
                // attachInterrupt(button[1][5], button10_ISR, RISING);
                test_ISR[2]=0;
                test_ISR[3]=0;
            }

            if(button_last_valid[1][5] == 1)    //passenger side unlock
            {
                detachInterrupt(button[1][5]);
                // attachInterrupt(button[1][4], button9_ISR, RISING);
                test_ISR[2]=0;
                test_ISR[3]=0;
            } 
            //reset lock button state value for selected switch
            //this will end door lock activity
            lock_button_active[0][0] = 0;
            lock_button_active[0][1] = 0;
            lock_button_active[1][0] = 0;
            lock_button_active[1][1] = 0;
            
            //Serial.println("Lock operation complete+++++++++++++++++++++++++++++++++++++++++++");

            break;
    }
    
}

void DoorLockWithOne5PinSwitch(byte side, byte lock_direction)
{
    //jump to current lock button active state
    switch(lock_button_active[side][lock_direction])
    {
        case 1:     //start debounce sequence\
            //save start debounce time
                lock_button_time[side][lock_direction] = current_loop_time;
                //set button state to next state
                lock_button_active[side][lock_direction] = 2;
            break;

        case 2:     //debounce timer
        {
            /* //use this to display lock button time in terminal 
            long unsigned display_time = current_loop_time - lock_button_time[side][lock_direction];
            Serial.print("testing debounce timer ");
            side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            Serial.println(display_time); */
            //test if debounce time, 20msec, has passed
            if(current_loop_time - lock_button_time[side][lock_direction] > 20UL)
            {
                //read selected switch value
                bool pin_value = digitalRead(button[side][lock_direction + 4]);
                //button debounced, save current value
                button_last_valid[side][lock_direction + 4] = pin_value;
                //check which press or release
                if(pin_value == 1)  //press
                {
                    //a valid button press detected, set state to motor start
                    lock_button_active[side][lock_direction] = 3;
                        
                }
                else    //release
                {
                    //a non valid button press detected
                    //set state value for selected switch to complete and reset
                    lock_button_active[side][lock_direction] = 5;
                }
            }
            else
            {
                //debounce time not passed yet, no action
            }
        
            break;
        }
        case 3:     //prepare and start lock motor
        {
            
            //start lock motor
            door_locks_enable(Passenger_Side, lock_direction);
            //set state value to wait til motor run timer has expired
            lock_button_active[side][lock_direction] = 4;
            break;
        }

        case 4: //test if lock motor timer has expired
        {
            /* long unsigned display_time = current_loop_time - lock_motor_start_time[!side];
            !side == Driver_Side ? Serial.print("Driver side, "):Serial.print("Passenger side, "); 
            Serial.println(display_time); */
            //test if lock motor run time has passed
            if(current_loop_time - lock_motor_start_time[!side_select] > Door_lock_motor_run_time)
            {
                //stop lock motors, clear active flags
                door_locks_stop(Passenger_Side);
                //set state value for selected switch to complete and reset
                lock_button_active[side][lock_direction] = 5;
            }
            else
            {
                //motor run time not passed yet, no action
            }
            break;
        } 
            
        case 5:     //lock motor complete, reset flags & prepare for next event
            //read & set last state values
            button_last_valid[0][4] =  digitalRead(button[0][4]);   //driver lock switch
            button_last_valid[0][5] =  digitalRead(button[0][5]);   //driver unlock switch
            
            //set interrupts based on door lock switch position
            //if lock switch active(1) then disable lock interrupt & vice versa
            if(button_last_valid[0][4] == 1)    //driver side lock
            {
                detachInterrupt(button[0][4]);
                // attachInterrupt(button[0][5], button8_ISR, RISING);
                test_ISR[0]=0;
                test_ISR[1]=0;
            }

            if(button_last_valid[0][5] == 1)    //driver side unlock
            {
                detachInterrupt(button[0][5]);
                // attachInterrupt(button[0][4], button7_ISR, RISING);
                test_ISR[0]=0;
                test_ISR[1]=0;
            } 
 
            //reset lock button state value for selected switch
            //this will end door lock activity
            lock_button_active[0][0] = 0;
            lock_button_active[0][1] = 0;
            
            //Serial.println("Lock operation complete+++++++++++++++++++++++++++++++++++++++++++");

            break;
    }
    
}

/**
 * Enables the door lock motors and sets the lock state.
 * Enables the emergency timer, sets the active flags,
 * and starts the lock motor PWM.
 *
 * @param side Side to activate lock for - 0 for driver, 1 for passenger, 2 for both
 * @param direction Direction to activate lock - 0 for unlock, 1 for lock
 */
void door_locks_enable(byte side, bool direction, bool pass)    //side, 0=driver/1=passenger/2=both, lock, 0=lock/1=unlock
{
    // side = 0 for driver, 1 for passenger, 2 for both
    // direction = 0 for unlock, 1 for lock
    // save lock motor start time
    //test which lock mode
    switch(door_lock_mode)
    {
        case 1:
            lock_motor_start_time[side] = current_loop_time; 
            //set lock motor mode active
            lock_motor_active[side] = true;
            //save the emergency timer start time
            lock_emergency_time_start[side] = current_loop_time;              //lock motor run start time
            //set lock timer active
            lock_emergency_timer_active[side] = true;
            break;

        case 2:
            lock_motor_start_time[Passenger_Side] = current_loop_time; 
            //set lock motor mode active
            lock_motor_active[Passenger_Side] = true;
            //save the emergency timer start time
            lock_emergency_time_start[Passenger_Side] = current_loop_time;              //lock motor run start time
            //set lock timer active
            lock_emergency_timer_active[Passenger_Side] = true;
            // //clear driver lock motor mode active
            // lock_motor_active[Driver_Side] = false;
            // //clear driver lock timer active
            // lock_emergency_timer_active[Driver_Side] = false;
            break;

        case 3:
            lock_motor_start_time[side_select] = current_loop_time; 
            //set lock motor mode active
            lock_motor_active[side_select] = true;
            //save the emergency timer start time
            lock_emergency_time_start[side_select] = current_loop_time;              //lock motor run start time
            //set lock timer active
            lock_emergency_timer_active[side_select] = true;
            break;

        case 4:
            if(pass == 1)  //first pass
            {
                lock_motor_start_time[side_select] = current_loop_time; 
                //set lock motor mode active
                lock_motor_active[side_select] = true;
                //save the emergency timer start time
                lock_emergency_time_start[side_select] = current_loop_time;              //lock motor run start time
                //set lock timer active
                lock_emergency_timer_active[side_select] = true;
            }
            else
            {
                lock_motor_start_time[Passenger_Side] = current_loop_time; 
                //set lock motor mode active
                lock_motor_active[Passenger_Side] = true;
                //save the emergency timer start time
                lock_emergency_time_start[Passenger_Side] = current_loop_time;              //lock motor run start time
                //set lock timer active
                lock_emergency_timer_active[Passenger_Side] = true;
                //clear driver lock motor mode active
                lock_motor_active[Driver_Side] = false;
                //clear driver lock timer active
                lock_emergency_timer_active[Driver_Side] = false;
            }
            break;
    }
    
        
    //set motor control pins for direction
    switch(direction)
    {
        case 0:
            // lock mode
            //digitalWrite(Door_Lock_pin, 1);
            //digitalWrite(Door_Unlock_pin, 0);
            break;
        case 1:
            // unlock mode
            //digitalWrite(Door_Lock_pin, 0);
            //digitalWrite(Door_Unlock_pin, 1);
            break;
    }

    // start lock motor pwm, speed
    switch (side)
    {
        case 0:
            // driver side mode
            //Serial.println("Driver lock motor started");
            ledcWrite(Driver_Lock_PWM_channel, lock_speed);
            ledcWrite(Passenger_Lock_PWM_channel, 0);
            // if(notificationsEnabled)
            // {
            //     //notify BLE data for selection, controller, control, and side
            //     side_select == Driver_Side ? notify_data[3]=0x80:notify_data[3]=0x81;
            //     //notify BLE data for lock or unlock status
            //     direction == 0 ? notify_data[2] = 0x02:notify_data[2] = 0x12;     //
            //     notifyClients2(notify_data[3], notify_data[2]); //send data  
            // }
            break;

        case 1:
            // passenger side mode
            //Serial.println("Passenger lock motor started");
            ledcWrite(Driver_Lock_PWM_channel, 0);
            ledcWrite(Passenger_Lock_PWM_channel, lock_speed);
            // if(notificationsEnabled)
            // {
            //     //notify BLE data for selection, controller, control, and side
            //     side_select == Driver_Side ? notify_data[3]=0x80:notify_data[3]=0x81;
            //     //notify BLE data for lock or unlock status
            //     direction == 0 ? notify_data[2] = 0x02:notify_data[2] = 0x12;     //
            //     notifyClients2(notify_data[3], notify_data[2]); //send data  
            // }
            break;

        case 2:
            // both sides mode
            Serial.println("Driver and Passenger lock motors started");
            ledcWrite(Driver_Lock_PWM_channel, lock_speed);
            ledcWrite(Passenger_Lock_PWM_channel, lock_speed);
            // if(notificationsEnabled)
            // {
            //     //notify BLE data for selection, controller, control, and side
            //     side_select == Driver_Side ? notify_data[3]=0x80:notify_data[3]=0x81;
            //     //notify BLE data for lock or unlock status
            //     direction == 0 ? notify_data[2] = 0x01:notify_data[2] = 0x11;     //
            //     notifyClients2(notify_data[3], notify_data[2]); //send data  
            // }
            break;
    }
}


/**
 * Stops the door lock motors and resets the lock state.
 * Disables the emergency timer, resets the active flags,
 * waits for switches to settle, and re-enables the lock
 * switch interrupts.
 */
void   door_locks_stop(byte side)
{
    // stop the lock motors
    ledcWrite(Driver_Lock_PWM_channel, 0);
    ledcWrite(Passenger_Lock_PWM_channel, 0);
    //digitalWrite(Door_Lock_pin, HIGH);
   // digitalWrite(Door_Unlock_pin, HIGH);
    // stop the emergency timer
    lock_emergency_timer_active[side] = false;
    //set lock motor mode inactive
    lock_motor_active[side] = false;
}



/**
 * Starts the door lock motor and sets status flags for the requested side and direction.
 *
 * Checks if lock is already in requested state. If so, exits without action.
 * If not, disables interrupts, sets motor direction and speed, sets status flags,
 * starts timer, and enables lock switch interrupt for opposite side.
 *
 * @param side Side to activate lock for - 0 for driver, 1 for passenger
 * @param direction Direction to activate lock - 0 for lock, 1 for unlock
 */
void start_lock_motors(bool side, bool direction)
{
    // side = 0 for driver, 1 for passenger
    // direction = 0 for lock, 1 for unlock
    // check which side is calling
    bool lock_state; // init lock status variable
    bool opposite_side = !side;
    // byte lock_speed = 140;
    // Serial.print("Lock side test - ");
    // (side == 0) ? Serial.print("Driver ") : Serial.print("Passenger ");
    // Serial.print(side);
    // Serial.print(",  ");
    // (!side == 0) ? Serial.print("Driver ") : Serial.print("Passenger ");
    // Serial.println(!side);

    // test if opposite side is already in proper state
    lock_state = digitalRead(button[opposite_side][4 + direction]);
    // (side == 0) ? Serial.print("Driver ") : Serial.print("Passenger ");
    // (direction == 0) ? Serial.print("Lock state is ") : Serial.print("Unlock state is  ");
    // Serial.println(lock_state);
    if (lock_state != 0)
    {
        // disable all switch pin interrupts except for the activated motor switch pin
        // disable other side interrupts
        ////detachInterrupt((button[!side][4])); // disable lock switch interrupt
        ////detachInterrupt((button[!side][5])); // disable unlock switch interrupt

        // disable this side interrupts
        ////detachInterrupt((button[side][5 - direction])); // disable lock or unlock switch interrupt

        // set motor control pins for direction, 0=lock, 1=unlock
        //digitalWrite(Door_Lock_pin, !direction);
        //digitalWrite(Door_Unlock_pin, direction);

        // start lock motor pwm, speed
        switch (side)
        {
            case Driver_Side:
                // driver side mode, start passenger lock motor
                ledcWrite(Driver_Lock_PWM_channel, 0);
                ledcWrite(Passenger_Lock_PWM_channel, lock_speed);
                //ledcWrite(Driver_Lock_PWM_channel, lock_speed);
                //ledcWrite(Passenger_Lock_PWM_channel, 0);
                break;

            case Passenger_Side:
                // passenger side mode, start driver lock motor
                // ledcWrite(Driver_Lock_PWM_channel, lock_speed);
                ledcWrite(Driver_Lock_PWM_channel, 200);
                ledcWrite(Passenger_Lock_PWM_channel, 0);
                break;
        }
        //set the interrupts for active lock switch
        set_active_lock_switch_interrupts(opposite_side, direction);
        // set flags 0=lock, 1=unlock
        door_lock_direction[opposite_side] = direction;
        //door_lock_direction[side] = 0;
        // set flags 0=driver, 1=passenger
        door_lock_side = opposite_side;
        // set flags 0=not active, 1=active
        door_lock_active[opposite_side] = true;
        // start the door lock emergency timer
        // start lock timer
        lock_motor_start_time[0] = current_loop_time; // save current time
        lock_emergency_timer_active[side_select] = true; // set timer active flag
    }
    else
    {
        // lock is already in proper position
        // no action required
        // (side == 0) ? Serial.print("Driver ") : Serial.print("Passenger ");
        // Serial.print("no action required, lock already in position");
        // Serial.println(lock_state);
        // set flags 0=not active, 1=active
        door_lock_active[opposite_side] = false;
        // reset last button value
        // button_last_valid[!side][4 + direction] = 1;
        // enable interrupts
        set_inactive_lock_switch_interrupts(side, switch_number);
        // Serial.print("Button restore state:");
        // Serial.print(button_state[side_select][switch_number]);
        // Serial.print(", Button restore last state:");
        // Serial.println(button_last_valid[side_select][switch_number]);
    }
    return;
}

void stop_lock_motors(bool side, bool direction)
{
    // stop the lock motors
    ledcWrite(Driver_Lock_PWM_channel, 0);
    ledcWrite(Passenger_Lock_PWM_channel, 0);
    //digitalWrite(Door_Lock_pin, HIGH);
    //digitalWrite(Door_Unlock_pin, HIGH);

    // stop the emergency timer
    lock_emergency_timer_active[side_select] = false;

    //set the interrupts for inactive lock switches
    set_inactive_lock_switch_interrupts(side, direction);
    // set flags 0=lock, 1=unlock
    door_lock_direction[side] = direction;
    //door_lock_direction[side] = 0;
    // set flags 0=driver, 1=passenger
    door_lock_side = side;
    // set flags 0=not active, 1=active
    door_lock_active[side] = true;
    
}

void set_active_lock_switch_interrupts(bool side, byte switch_number)
{
    switch((side * 2) + (switch_number - 4))
    {
        case 0: //driver lock
            Serial.println("Driver lock set interrupts");
            // attachInterrupt(button[Driver_Side][4], button7_ISR, FALLING);
            detachInterrupt(button[Driver_Side][5]);
            detachInterrupt(button[Passenger_Side][4]);
            detachInterrupt(button[Passenger_Side][5]);
            // button_last_valid[Driver_Side][4 + 0] = 1;
            // button_state[Driver_Side][4 + 0] = 0;
            break;
        case 1: //driver unlock
            Serial.println("Driver unlock set interrupts");
            detachInterrupt(button[Driver_Side][4]);
            // attachInterrupt(button[Driver_Side][5], button8_ISR, FALLING);
            detachInterrupt(button[Passenger_Side][4]);
            detachInterrupt(button[Passenger_Side][5]);
            // button_last_valid[Driver_Side][4 + 0] = 1;
            // button_state[Driver_Side][4 + 0] = 0;
            break;
        case 2: //passenger lock
            Serial.println("Passenger lock set interrupts");
            // attachInterrupt(button[Passenger_Side][4], button9_ISR, FALLING);
            detachInterrupt(button[Passenger_Side][5]);
            detachInterrupt(button[Driver_Side][4]);
            detachInterrupt(button[Driver_Side][5]);
            // button_last_valid[Passenger_Side][4 + 0] = 1;
            // button_state[Passenger_Side][4 + 0] = 0;
            break;
        case 3: //passenger unlock
            Serial.println("Passenger unlock set interrupts");
            detachInterrupt(button[Passenger_Side][4]);
            // attachInterrupt(button[Passenger_Side][5], button10_ISR, FALLING);
            detachInterrupt(button[Driver_Side][4]);
            detachInterrupt(button[Driver_Side][5]);
            // button_last_valid[Passenger_Side][4 + 0] = 1;
            // button_state[Passenger_Side][4 + 0] = 0;
            break;
    }
}

void set_inactive_lock_switch_interrupts(bool side, byte switch_number)
{
    switch((side * 2) + (switch_number - 4))
    {
        case 0: //driver lock
            detachInterrupt(button[Driver_Side][4]);
            // attachInterrupt(button[Driver_Side][5], button8_ISR, FALLING);
            detachInterrupt(button[Passenger_Side][4]);
            // attachInterrupt(button[Passenger_Side][5], button10_ISR, FALLING);
            // button_last_valid[Driver_Side][4 + 0] = 1;
            // button_state[Driver_Side][4 + 0] = 0;
            break;
        case 1: //driver unlock
            // attachInterrupt(button[Driver_Side][4], button7_ISR, FALLING);
            detachInterrupt(button[Driver_Side][5]);
            // attachInterrupt(button[Passenger_Side][4], button9_ISR, FALLING);
            detachInterrupt(button[Passenger_Side][5]);
            // button_last_valid[Driver_Side][4 + 0] = 1;
            // button_state[Driver_Side][4 + 0] = 0;
            break;
        case 2: //passenger lock
            detachInterrupt(button[Passenger_Side][4]);
            // attachInterrupt(button[Passenger_Side][5], button10_ISR, FALLING);
            detachInterrupt(button[Driver_Side][4]);
            // attachInterrupt(button[Driver_Side][5], button8_ISR, FALLING);
            // button_last_valid[Passenger_Side][4 + 0] = 1;
            // button_state[Passenger_Side][4 + 0] = 0;
            break;
        case 3: //passenger unlock
            // attachInterrupt(button[Passenger_Side][4], button9_ISR, FALLING);
            detachInterrupt(button[Passenger_Side][5]);
            // attachInterrupt(button[Driver_Side][4], button7_ISR, FALLING);
            detachInterrupt(button[Driver_Side][5]);
            // button_last_valid[Passenger_Side][4 + 0] = 1;
            // button_state[Passenger_Side][4 + 0] = 0;
            break;
    }
}







