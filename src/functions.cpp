#include "functions.h"


//-----------------------------------------------------------------
//                     led blink function
//-----------------------------------------------------------------
/**
 * Blinks an LED a specified number of times at a specified blink rate.
 *
 * @param side Which LED to blink (0 for driver, 1 for passenger, 2 for none)
 * @param blinks Number of blinks
 * @param rate Blink rate in milliseconds
 */
void LedBlink(byte side, byte blinks, int rate)
{
    if(side == 0)
    {
        // driver side
        digitalWrite(LedPin, HIGH);
        delay(80);
        digitalWrite(LedPin, LOW);
        delay(400);
    }
    else if(side == 1)
    {
        // passenger side
        digitalWrite(LedPin, HIGH);
        delay(80);
        digitalWrite(LedPin, LOW);
        delay(100);
        digitalWrite(LedPin, HIGH);
        delay(80);
        digitalWrite(LedPin, LOW);
        delay(400);
    }
    else
    {
        //no blinks
    }
    
    for (byte i = 1; i <= blinks; i++)
    {
        //if (side == 0)
        //{
            // driver side
            digitalWrite(LedPin, HIGH);
            delay(rate * .33);
            digitalWrite(LedPin, LOW);
            delay(rate * .67);
        //}
        //else
        //{
            // passenger side
            //digitalWrite(Passenger_ledPin, HIGH);
            //delay(rate * .33);
            // delay(50);
            //digitalWrite(Passenger_ledPin, LOW);
            //delay(rate * .67);
            // delay(25);
        //}
    }
}

//-----------------------------------------------------------------
//                     button  debounce function
//-----------------------------------------------------------------

/**
 * @brief Validates if a button press on the given side and switch is valid, accounting for debouncing.
 *
 * @param side_selected Side of the vehicle (0 for driver, 1 for passenger)
 * @param switch_selected Switch number on given side
 * @return True if button press is valid, false if invalid or still debouncing
 */
bool Valid_Button(byte side_selected, byte switch_selected)
{
    //create a variable to hold the debounce time
    static unsigned long debounce_start_time[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
    // check if a button is active
    // set by ISR
    if (button_change[side_selected][switch_selected] == true)
    {
        Serial.println("Switch Active");
        //delay(10);
        // button is active
        // check if debounce active
        // if not then start debounce checks
        if (debounce_active[side_selected][switch_selected] == false)
        {
            // set active flag & save current time
            debounce_active[side_selected][switch_selected] = true;
            debounce_start_time[side_selected][switch_selected] = current_loop_time; // time at start of debounce timer
            // debounce_start_time[side_selected][switch_number] = millis(); // time at start of debounce timer
            return false;
        }
        // debounce time test
        // check if current time is more then debounce time, about 20msec
        if (current_loop_time - debounce_start_time[side_selected][switch_selected] > debounce_time_period)
        {
            // approx 20msec has passed since button change
            // now compare if current button state and
            // past state are the same
            bool now_button = digitalRead(button[side_selected][switch_selected]);
           
           //test if switch has changed from last state
            if (button_state[side_selected][switch_selected] != now_button)
            {
                // buttons states do not match
                // do another 20msec debounce
                // save current button state
                button_state[side_selected][switch_selected] = now_button;
                // set new start time for debounce
                debounce_start_time[side_selected][switch_selected] = current_loop_time;
                // debounce_start_time[side_selected][switch_number] = millis();
                //  set button active flag
                button_active[side_selected][switch_selected] = false;
                return false;
            }
            else
            {
                // valid button change
                // turn off debounce and change flags
                debounce_active[side_selected][switch_selected] = false;
                // turn off button change flag
                button_change[side_selected][switch_selected] = false;
                //save the button current state in button last 
                //button_last_valid[side_select][switch_number] = now_button;
                // set button active flag
                button_active[side_selected][switch_selected] = true;
                Serial.print("Active button number: ");
                Serial.println(lock_motor_start_time[0]);
                return true;
            }
        }
        return false;
    }
    return false;
}

//-----------------------------------------------------------------
//                       set flags function
//-----------------------------------------------------------------

/**
 * Sets the window mode, motor mode, auto mode, and window position flags for the given side.
 *
 * @param set_window_mode New window mode: 0=idle, 1=manual, 2=auto, 9=no change
 * @param set_motor_mode New motor mode: 0=off, 1=starting, 2=running, 9=no change
 * @param set_direction_mode New auto mode: 0=off, 1=auto down, 2=auto up, 9=no change
 * @param set_position_mode New window position: 0=partial, 1=full down, 2=full up, 9=no change
 * @param set_side Side to set flags for (0 or 1): 0=driver side, 1=passenger side
 */
void set_mode_flags(byte set_window_mode, byte set_motor_mode, byte set_direction, byte set_window_position, byte set_side)
{
    // set window mode, 0=idle, 1=manual, 2=auto, 9=no change
    if (set_window_mode != 9)
    {
        window_mode[set_side] = set_window_mode;
    }
    // direction mode flag, 0=off, 1=down, 2=up, 9=no change
    if (set_direction != 9)
    {
        direction_mode[set_side] = set_direction;
    }   
    // set motor mode, 0=off, 1=starting, 2=running, 9=no change
    if (set_motor_mode != 9)
    {
        motor_mode[set_side] = set_motor_mode;
    }    
    // set window position, 0=partial, 1=full down, 2=full up, 9=no change
    if (set_window_position != 9)
    {
        position_mode[set_side] = set_window_position;
    }    
}


//-----------------------------------------------------------------
//                        EEPROM Write function
//-----------------------------------------------------------------
/**
 * Writes a 16-bit integer value into an EEPROM address.
 * Splits the int into two bytes and writes to two consecutive addresses.
 * Commits the write to EEPROM after writing both bytes.
 */
void writeIntIntoEEPROM(int address, int number)
{
    EEPROM.write((address * 2), (number >> 8));
    EEPROM.write(((address * 2) + 1), (number & 0xFF));
    EEPROM.commit();
}


//-----------------------------------------------------------------
//                        EEPROM Read function
//-----------------------------------------------------------------
/**
 * Reads a 16-bit integer from the given EEPROM address.
 * Reads the two bytes from the address and adjacent address,
 * shifts and combines them into the 16-bit int.
 */
int readIntFromEEPROM(int address)
{
    return (EEPROM.read(address * 2) << 8) + EEPROM.read((address * 2) + 1);
}


//-----------------------------------------------------------------
//             EEPROM Save Window Position function
//-----------------------------------------------------------------
/**
 * Saves the window position to EEPROM
 * if the new value is different from the existing value.
 * address 16 is used for the driver side
 * address 17 is used for the passenger side
 * Writes a 8-bit byte value into an EEPROM address.
 * Commits the write to EEPROM.
 */
void save_window_position(byte side, byte position)
{   
    
    // Serial.println("save_window_position");
    // (side == 0) ? Serial.print("Driver Side :: ") : Serial.print("Passenger Side :: ");
    // switch(position_mode[0])
    // {
    //     case 0:
    //         Serial.println("Partial");
    //         break;
    //     case 1:
    //         Serial.println("Down");
    //         break;
    //     case 2:
    //         Serial.println("Up");
    //         break;
    // }
    //test if the new value is different from the existing value
    if(position != EEPROM.read(16 + side))
    {
        //write the new value to EEPROM
        EEPROM.write(16 + side, position);
        EEPROM.commit();
    }
    
}

//-----------------------------------------------------------------
//                       moving average function
//-----------------------------------------------------------------
/**
 * Updates a moving average array with a new value.
 *
 * Shifts the values in the moving average array and adds the new value.
 * Uses a circular buffer approach to keep the memory footprint fixed.
 * The averaging window size is configured by the moving_avg_array_size definition.
 *
 * @param side The side (0 or 1) to update the moving average for.
 * @param new_value The new value to add to the moving average.
 * @param index_return If true, returns the current index in the moving average array instead of updating it.
 * @return If index_return is false, returns 255. If index_return is true, returns the current index in the moving average array.
 */
/**
 * Updates a moving average array with a new value.
 *
 * Shifts the values in the moving average array and adds the new value.
 * Uses a circular buffer approach to keep the memory footprint fixed.
 * The averaging window size is configured by the moving_avg_array_size definition.
 */
byte moving_average(byte side, int new_value, bool index_return)
{
    // create/init pointers for adding values for averaging
    static byte Index[2] = {0, 0};
    // check if index return is requested
    if (index_return == false)
    {
        // overwrite the oldest value with the newest value
        Readings[side][Index[side]] = new_value; // Add the newest reading to the window
        // Serial.print("Index = ");
        // Serial.print(Index[side]);
        // Serial.print(" value = ");
        // Serial.println(new_value);
        // increment the index for the next reading
        ((Index[side] < moving_avg_array_size -1 ) ? Index[side]++ : Index[side] = 0); // Wrap the index to 0 if it exceeds the window size
        return 255;
    }
    else
    {
        // return the current index in the moving average array
        // Serial.print("Index returned = ");
        // Serial.println(Index[side]);
        return Index[side];
    }
}
  

//-----------------------------------------------------------------
//                        average current function
//-----------------------------------------------------------------
//this routine will calc the average motor current
//from the 250 values saved in the READINGS array
//if there have been more than 3000pwm interrupts.
//do not use the last 50 readings in calculation
//as that would include max current readings during auto stop
int average_current()
{
    //initialize the summation variable
    int SUM = 0;  
    int average;
    Serial.println("interrupt count = " + String(interruptCount[side_select]));
    //test if more than 3000 interrupts have occurred
    //window will have been in motion for more then 1.5 seconds 
    //if(interruptCount[side_select] > 3000) 
    if(interruptCount[side_select] > 3000) //1.5 seconds
    {
        //get index pointer for latest moving average save
        byte reading_pointer =  moving_average(side_select, 0, true);
        //we want to not use the last 50 readings in the moving average
        //as that would include max current readings during auto stop
        for(int i = 0; i < moving_avg_array_size - 50; i++)
        //for(int i = 0; i < 200; i++)
        {
            SUM = SUM + Readings[side_select][reading_pointer];
            // Serial.print("Index = ");
            // Serial.print(reading_pointer);
            // Serial.print(" value = ");
            // Serial.println(Readings[side_select][reading_pointer]);
            //inc or wrap pointer
            reading_pointer >= 249 ? reading_pointer = 0 : reading_pointer ++;
        }
        //divide the sum by the window array size to get the average
        average = SUM / (moving_avg_array_size - 50);
        Serial.print("Calculated average current = ");
        Serial.println(average);
        //get the average current from EPROM
        //average the two and save the new average in EPROM
        return average;
    }
    else
    {
        //return 0 if less than 3000 interrupts have occurred
        return 0;
    }
    
}
//-----------------------------------------------------------------
//                        Average Current calc function
//-----------------------------------------------------------------
/**
 * gets the calculated average motor current based on the latest 250 current samples.
 * Compares the new average to the existing value and updates the EEPROM
 * if the difference is more than a threshold. Averages the old and new values
 * when updating to prevent large changes. Handles cases where there are not
 * enough samples yet. Returns the latest current alarm threshold or 0.
 */
int calc_motor_avg_current()
{
    // call the avg current calc function
    int new_average_current = average_current();
    // if new average current is 0, not enough samples yet
    // less than 3000 pwm cycles occurred and
    // will not calc new avg current 
    Serial.print("New average current: ");
    Serial.println(new_average_current);
    if (new_average_current > 0)
    {
        // get the saved current alarm threshold & / by 2 to get avg current
        int temp_avg_current = (motor_current_alarm[side_select][(direction_mode[side_select] - 1)]) / auto_stop_sensitivity;
        // check if latest average current value is different
        // by more than 50 from the saved value, saves on EEPROM writes
        //if ((new_average_current > (temp_avg_current + 50)) || (new_average_current < (temp_avg_current - 50)))
        if (abs(new_average_current - temp_avg_current) > 50)
        {
            // current is different by more than 50
            // average out old and new values so large changes don't occur
            new_average_current = (new_average_current + temp_avg_current) / 2;
            // save new  current threshold alarm value
            motor_current_alarm[side_select][direction_mode[side_select] - 1] = new_average_current * auto_stop_sensitivity;
            // save new average current value to EEPROM
            writeIntIntoEEPROM(((side_select * 2) + (direction_mode[side_select] - 1)), (new_average_current));
            Serial.print("Current is different by more than 50, updating average current to: ");
            Serial.println(motor_current_alarm[side_select][direction_mode[side_select] - 1]);
            return motor_current_alarm[side_select][direction_mode[side_select] - 1];
        }
        else
        {
            Serial.println("Current is within 50 of saved current alarm value, no update to current alarm");
            return 0;
        }
    }
    else
    {
        Serial.println("Less than 6000 PWM cycles, no update to current alarm");
        return 0;
    }
}

//-----------------------------------------------------------------
//             Calc Average Window Run Time function
//-----------------------------------------------------------------
/**
 * Calculates the average window run time for the specified side and direction.
 * Compares the new average to the existing value and updates the EEPROM if the
 * difference is more than 50ms. Averages the old and new values when updating
 * to prevent large changes. Handles cases where there are not enough samples
 * yet. Notifies BLE clients of the updated window run time.
 *
 * @param side The side of the window (Driver_Side or Passenger_Side).
 * @param direction The direction of the window (0 for down, 1 for up).
 */
void calc_window_avg_time(byte side, byte direction)
{   
    int run_time_raw, avg_time, saved_time;
    double run_time;
    //window run time alarm is the saved value that's in EEPROM + the window overrun timeout
    saved_time = window_run_time_alarm[side][direction] - window_overrun_timeout;
    // calculate window run time from end to end
    
    //calc window run time
    run_time_raw = current_loop_time - window_run_start_time[side_select];
    run_time = run_time_raw;    // convert to double for calculations
    #ifdef DEBUG
        Serial.print("Window run time: ");
        Serial.print(run_time/1000, 2);     // convert to seconds for printing
        Serial.println(" seconds");
    #endif
    int temp_time = run_time_raw - saved_time;
    //ensure the window run time is within plus or minus 1000msec of the saved time
    if(abs(temp_time) < 1000)      //to ensure this measurement is of an end to end run 
    {
        
        // check if the new run time is more or less than 50msec of the saved run time
        if(abs(temp_time) > 50) 
        {
            // if the run time is more or less than 50msec
            // of the saved time, update the saved run time
            // and save it to EEPROM
            // this will reduce saving minor changes to EEPROM
            // address 4-driver down, 5-driver up, 6-passenger down, 7-passenger up
            // average the new run time with the saved run time
            avg_time = (run_time_raw + saved_time) / 2;
            writeIntIntoEEPROM((4 + (side * 2) + direction), avg_time);
            // set the run time alarm to the run time
            window_run_time_alarm[side][direction] = avg_time + window_overrun_timeout;
            #ifdef DEBUG
                Serial.print("New window run time alarm saved: ");
                Serial.print(window_run_time_alarm[side][direction]);
                Serial.println(" msec");
            #endif
            //setup notify BLE data if enabled
            // if(notificationsEnabled)
            // {
            //     //notify BLE data for selection, controller, control, and side
            //     side_select == Driver_Side ? notify_data[3]=0x40:notify_data[3]=0x41;
            //     //notify BLE data for change runtime up or down to green
            //     direction+1 == 1 ? notify_data[2]=0x43:notify_data[2]=0x45;
            //     notifyClients2(notify_data[3], notify_data[2]); //send data
            // }
        }
        else
        {
            // if the run time is within 50msec of the saved run time
            // do nothing
            #ifdef DEBUG
                Serial.println("Window run time is within 50msec of saved run time, no update to run time alarm");
            #endif
        }
    }
    else
    {
        // if the run time is more or less than 1000msec of the saved run time
        // do nothing
        #ifdef DEBUG
            Serial.println("Window run time is more or less than 1000msec of saved run time, no update to run time alarm");
        #endif
    }
    return;
} 


//-----------------------------------------------------------------
//                        Sound Assign function
//-----------------------------------------------------------------
//this will create a series of beeps based on the beep_alarm array
void sound_assign(byte sound_number)
{
    //setup the beep_alarm array used by the sound alarm function
    //beep_alarm[index][0] = frequency
    //beep_alarm[index][1] = duration

    //sound number 1-10
    //sound 1-3 will be for the driver
    //sound 4-6 will be for the passenger
    //sound 7-9 will be for the window
    //sound 10 will be for the door
    //sound 11 will be for the emergency
    //sound 12 will be for the emergency stop
    //sound 13 will be for the emergency stop
    //sound 14 will be for the emergency stop
    //sound 15 will be for the emergency stop
    switch(sound_number)
    {
        case 1:
            beep_alarm[0][0] = 800;  //frequency
            beep_alarm[0][1] = 50;  //duration
            beep_alarm[1][0] = 0;    //no beep
            beep_alarm[1][1] = 40;  //duration
            beep_alarm[2][0] = 800;  //frequency
            beep_alarm[2][1] = 50;  //duration
            beep_alarm[3][0] = 0;    //no beep
            beep_alarm[3][1] = 40;  //duration
            beep_alarm[4][0] = 400; //frequency
            beep_alarm[4][1] = 80;  //duration
            beep_alarm[5][0] = 60; //frequency
            beep_alarm[5][1] = 180;  //duration
            beep_alarm[6][0] = 99;  //end flag
            break;
        case 2:
            beep_alarm[0][0] = 310;  //frequency
            beep_alarm[0][1] = 60;   //duration
            beep_alarm[1][0] = 0;    //frequency
            beep_alarm[1][1] = 40;   //duration
            beep_alarm[2][0] = 480;  //frequency
            beep_alarm[2][1] = 50;   //duration
            beep_alarm[3][0] = 280;  //frequency
            beep_alarm[3][1] = 60;   //duration
            beep_alarm[4][0] = 99;   //end
            break;
        case 3:
            beep_alarm[0][0] = 310;  //frequency
            beep_alarm[0][1] = 60;   //duration
            beep_alarm[1][0] = 440;    //frequency
            beep_alarm[1][1] = 40;   //duration
            beep_alarm[2][0] = 310;  //frequency
            beep_alarm[2][1] = 60;    //duration
            beep_alarm[3][0] = 440;  //frequency
            beep_alarm[3][1] = 60;   //duration
            beep_alarm[4][0] = 99;  //end
            break;
        case 4:
            beep_alarm[0][0] = 380;     //frequency
            beep_alarm[0][1] = 80;      //duration
            beep_alarm[1][0] = 520;     //frequency
            beep_alarm[1][1] = 100;     //duration
            beep_alarm[2][0] = 99;      //end
            break;
        case 5:
            beep_alarm[0][0] = 310;     //frequency
            beep_alarm[0][1] = 100;     //duration
            beep_alarm[1][0] = 99;      //end
            break;
        case 6:
            beep_alarm[0][0] = 800;     //frequency
            beep_alarm[0][1] = 80;      //duration
            beep_alarm[1][0] = 440;     //frequency
            beep_alarm[1][1] = 60;      //duration
            beep_alarm[2][0] = 0;       //frequency
            beep_alarm[2][1] = 40;      //duration
            beep_alarm[3][0] = 440;     //frequency
            beep_alarm[3][1] = 60;      //duration
            beep_alarm[4][0] = 99;      //end
            break;
        case 7:
            beep_alarm[0][0] = 2200;     //frequency
            beep_alarm[0][1] = 80;      //duration
            beep_alarm[1][0] = 3200;     //frequency
            beep_alarm[1][1] = 100;     //duration
            beep_alarm[2][0] = 0;     //frequency
            beep_alarm[2][1] = 40;      //duration
            beep_alarm[3][0] = 1600;     //frequency
            beep_alarm[3][1] = 60;     //duration
            beep_alarm[4][0] = 99;      //end
            break;
        case 8:
            beep_alarm[0][0] = 480;  //frequency
            beep_alarm[0][1] = 100;   //duration
            beep_alarm[1][0] = 0;     //no beep
            beep_alarm[1][1] = 20;   //duration
            beep_alarm[2][0] = 1420;  //frequency
            beep_alarm[2][1] = 120;  //duration
            beep_alarm[3][0] = 0;    //no beep
            beep_alarm[3][1] = 20;  //duration
            beep_alarm[4][0] = 1520;    //frequency
            beep_alarm[4][1] = 120;  //duration
            beep_alarm[5][0] = 0;    //no beep
            beep_alarm[5][1] = 40;  //duration
            beep_alarm[6][0] = 1520;    //frequency
            beep_alarm[6][1] = 160;  //duration
            beep_alarm[7][0] = 99;  //end flag
            break;
        case 9:
            beep_alarm[0][0] = 310;  //frequency
            beep_alarm[0][1] = 60;   //duration
            beep_alarm[1][0] = 0;    //frequency
            beep_alarm[1][1] = 40;   //duration
            beep_alarm[2][0] = 480;  //frequency
            beep_alarm[2][1] = 50;   //duration
            beep_alarm[3][0] = 280;  //frequency
            beep_alarm[3][1] = 60;   //duration
            beep_alarm[4][0] = 99;   //end
            break;
        case 10:
            beep_alarm[0][0] = 310;  //frequency
            beep_alarm[0][1] = 150;   //duration
            beep_alarm[1][0] = 0;    //frequency
            beep_alarm[1][1] = 20;   //duration
            beep_alarm[2][0] = 220;  //frequency
            beep_alarm[2][1] = 80;   //duration
            beep_alarm[3][0] = 0;  //frequency
            beep_alarm[3][1] = 40;   //duration
            beep_alarm[4][0] = 220;  //frequency
            beep_alarm[4][1] = 80;   //duration
            beep_alarm[5][0] = 99;   //end
            break;
        case 11:
            //alarm test
            beep_alarm[0][0] = 2100;     //frequency
            beep_alarm[0][1] = 80;       //duration
            beep_alarm[1][0] = 2800;     //frequency
            beep_alarm[1][1] = 80;       //duration
            beep_alarm[2][0] = 2100;     //frequency
            beep_alarm[2][1] = 100;      //duration
            beep_alarm[3][0] = 2800;     //frequency
            beep_alarm[3][1] = 100;      //duration
            beep_alarm[4][0] = 99;       //end
            break;
        case 12:
            beep_alarm[0][0] = 1280;     //frequency
            beep_alarm[0][1] = 80;     //duration
            beep_alarm[1][0] = 99;      //no beep
    }
    beep_alarm_enable = true;
}




//-----------------------------------------------------------------
//                        Sound Alarm function
//-----------------------------------------------------------------
/**
 * @brief Plays a sequence of beeps and tones to sound an alarm.
 * 
 * This function is responsible for playing a pre-defined sequence of beeps and tones to sound an alarm. 
 * It uses the `beep_alarm` array to determine the frequency and duration of each beep or tone in the sequence. 
 * The function keeps track of the current position in the sequence and updates the beep/tone accordingly. 
 * Once the entire sequence has been played, the function disables the alarm.
 */
void sound_alarm()
{
    //get the curent time
    static unsigned long time_stamp;
    static bool timing_active = false;
    static byte beep_count;

    //test if timing flag is active
    if(timing_active == false)
    {
        //test if the beep value is 99, end flag
         if(beep_alarm[0][0] == 99)
        {
            //the beep value is 99, end
            timing_active = false;
            beep_alarm_enable = false;
            return;
        }
        else
        {
            //set the timing flag
            timing_active = true;
            //set the time stamp
            time_stamp = current_loop_time;
            //initialize the beep count
            beep_count = 0;
            //read the initial beep values
            if(beep_alarm[beep_count][0] == 0)
            {
                ledcWriteTone(Beeper_PWM_channel, 0);
                //ledcWrite(Beeper_PWM_channel,0);
                return;  
            }
            else
            {
                ledcWriteTone(Beeper_PWM_channel, beep_alarm[beep_count][0]);
                //ledcSetup(Beeper_PWM_channel, beep_alarm[beep_count][0], PWM_resolution);
                ledcWrite(Beeper_PWM_channel,128);
                return;
            }
        }
    }
    //test if the time stamp is greater than the length of time to beep
    if((current_loop_time - time_stamp) > beep_alarm[beep_count][1])
    {
        //increment the beep count
        beep_count++;
        //test if next value is end flag
        if(beep_alarm[beep_count][0] == 99)
        {
            //this is the end of the beep sequence
            ledcWrite(Beeper_PWM_channel,0);
            timing_active = false;
            beep_alarm_enable = false;
            return;
        }
        else
        {
            //set the time stamp
            time_stamp = current_loop_time;
            //read the next beep value
            if(beep_alarm[beep_count][0] == 0)
            {
                ledcWriteTone(Beeper_PWM_channel, 0);
                //ledcWrite(Beeper_PWM_channel,0);
                return;
            }
            else
            {
                ledcWriteTone(Beeper_PWM_channel, beep_alarm[beep_count][0]);
                //ledcSetup(Beeper_PWM_channel, beep_alarm[beep_count][0], PWM_resolution);
                ledcWrite(Beeper_PWM_channel, 128);
            return;
            }
        }
    }
    else
    {
        //time stamp is not greater than the length of time to beep
        //do nothing
        return;
    }
}    


void print_status()
{
    Serial.println("----------------------------------------");
    Serial.print("side select: ");
        //side select, 0=driver or 1=passenger side
        switch(side_select)
        {
            case 0:
                Serial.println("driver side");
                break;
            case 1:
                Serial.println("passenger side");
                break;
        }
    Serial.print("switch: ");
        //switch number, 1=manual down, 2=manual up, 3=auto
        switch(switch_number)
            {
                case 1:
                    Serial.println("manual down");
                    break;
                case 2:
                    Serial.println("manual up");
                    break;
                case 3:
                    Serial.println("auto");
                break;
            }
    Serial.print("window mode: ");
    switch(window_mode[side_select])
    {
        //window mode, 0=idle, 1=manual, 2=auto
        case 0:
            Serial.println("idle");
            break;
        case 1: 
            Serial.println("manual");
            break;
        case 2:
            Serial.println("auto");
        break;

    }
    Serial.print("motor mode: ");
        //motor mode, 0=off, 1=starting, 2=running
        switch(motor_mode[side_select])
        {
            case 0:
                Serial.println("off");
                break;
            case 1:
                Serial.println("starting");
                break;
            case 2:
                Serial.println("running");
            break;
        }
        Serial.print("direction mode: ");
        //direction mode, 0=off, 1=down, 2=up
        switch(direction_mode[side_select])
        {
            case 0:
                Serial.println("off");
                break;
            case 1:
                Serial.println("down");
                break;
            case 2:
                Serial.println("up");
            break;
        }
        Serial.print("position mode: ");
        //position mode, 0=off, 1=down, 2=up
        switch(position_mode[side_select])
        {
            case 0:
                Serial.println("partial");
                break;
            case 1:
                Serial.println("full down");
                break;
            case 2:
                Serial.println("full up");
            break;
        }
        //motor speed, 0-255
        Serial.print("motor speed: ");
        Serial.println(motor_speed[side_select]);

        //motor current, 0-4095
        //Serial.print("motor current: ");
        //Serial.println(motor_current);
        
        //motor current alarm, 0-4095
        Serial.print("motor current alarm: ");
        Serial.println((motor_current_alarm[side_select][direction_mode[side_select] - 1]));
        //Serial.println((motor_current_alarm[side_select][direction_mode[side_select] - 1]) * 2);
        return;
}  

void print_Poverride_status()
{
    Serial.println("-----------------Poverride Status--------------");
    Serial.print("side select: ");
        //side select, 0=driver or 1=passenger side
        switch(side_select)
        {
            case 0:
                Serial.println("driver side");
                break;
            case 1:
                Serial.println("passenger side");
                break;
        }
    Serial.print("switch: ");
        //switch number, 1=manual down, 2=manual up, 3=auto
        switch(switch_number)
            {
                case 1:
                    Serial.println("manual down");
                    break;
                case 2:
                    Serial.println("manual up");
                    break;
                case 3:
                    Serial.println("auto");
                break;
            }
    Serial.print("passenger override timecheck: ");
    switch(passenger_override_timecheck)
    {
        case 1:
            Serial.println("true");
        case 0:
            Serial.println("false");
        break;
    }    

    Serial.print("passenger override active: ");
    switch(passenger_override_active)
    {
        case 1:
            Serial.println("true");
        case 0:
            Serial.println("false");
        break;
    }

    return;
}

//-----------------------------------------------------------------
//                        Override Timer Reset function
//-----------------------------------------------------------------
/**
 * Resets the passenger override timer to the current loop time, 
 * allowing for 15 seconds of override after the last operation.
 * This function is called when the passenger override is currently active.
 */
void reset_override_timer()
{
    //check if the passenger override is currently active
    if(passenger_override_active == true)
        {
            //passenger override is active
            //reset passenger override timer to digitalRead
            //new time stamp
            //this will allow for 15 seconds of override after last operation
            passenger_override_timeout_start_time = current_loop_time;
            //debug message
            Serial.println("passenger override timeout timer reset");
        }
    return;
}

//-----------------------------------------------------------------
//                        Calibrate Current function
//-----------------------------------------------------------------
/**
 * Calculates the motor data by performing exponential regression on the provided ADC and current values.
 * The function calculates the regression coefficients for the driver up, driver down, passenger up, and passenger down data sets.
 * The regression coefficients are stored in the global variables `coefficientsDriverUp`, `coefficientsDriverDown`, `coefficientsPassengerUp`, and `coefficientsPassengerDown`.
 */
void calculateMotorData() {
    // Data points for each IBT-2 output
    int adcValuesDriverUp[] = {1312, 2305, 2744, 2985};  // Driver Up 
    float currentValuesDriverUp[] = {1.98, 3.69, 5.13, 6.35};

    int adcValuesDriverDown[] = {1214, 2259, 2721, 2971};  // Driver Down
    float currentValuesDriverDown[] = {1.97, 3.67, 5.11, 6.34};

    int adcValuesPassengerUp[] = {1192, 2182, 2646, 2895};  // Passenger Up
    float currentValuesPassengerUp[] = {2.01, 3.76, 5.27, 6.56};

    int adcValuesPassengerDown[] = {1266, 2281, 2724, 2967};  // Passenger Down
    float currentValuesPassengerDown[] = {2.01, 3.75, 5.25, 6.55};

    // Calculate the number of elements in each array
    int nDriverUp = sizeof(adcValuesDriverUp) / sizeof(adcValuesDriverUp[0]);
    int nDriverDown = sizeof(adcValuesDriverDown) / sizeof(adcValuesDriverDown[0]);
    int nPassengerUp = sizeof(adcValuesPassengerUp) / sizeof(adcValuesPassengerUp[0]);
    int nPassengerDown = sizeof(adcValuesPassengerDown) / sizeof(adcValuesPassengerDown[0]);

    // Perform exponential regression for each data set
    coefficientsDriverUp = exponentialRegression(adcValuesDriverUp, currentValuesDriverUp, nDriverUp);
    coefficientsDriverDown = exponentialRegression(adcValuesDriverDown, currentValuesDriverDown, nDriverDown);
    coefficientsPassengerUp = exponentialRegression(adcValuesPassengerUp, currentValuesPassengerUp, nPassengerUp);
    coefficientsPassengerDown = exponentialRegression(adcValuesPassengerDown, currentValuesPassengerDown, nPassengerDown);
    
}


/**
 * Calculates the exponential regression coefficients for the given x and y data points.
 *
 * @param x The array of x-values (independent variable).
 * @param y The array of y-values (dependent variable).
 * @param n The number of data points.
 * @return A struct containing the calculated regression coefficients A and B.
 */
//RegressionResult exponentialRegression(int x[], float y[], int n);
// Function to calculate the exponential regression coefficients
RegressionResult exponentialRegression(int x[], float y[], int n) {
    float sumX = 0;
    float sumLnY = 0;
    float sumX2 = 0;
    float sumXLnY = 0;

    for (int i = 0; i < n; i++) {
        float lnY = log(y[i]);
        sumX += x[i];
        sumLnY += lnY;
        sumX2 += x[i] * x[i];
        sumXLnY += x[i] * lnY;
    }

    float denominator = n * sumX2 - sumX * sumX;
    float B = (n * sumXLnY - sumX * sumLnY) / denominator;
    float lnA = (sumLnY - B * sumX) / n;
    float A = exp(lnA);

    RegressionResult result = {A, exp(B)};
    return result;
}

/**
 * Calculates the current value based on the given ADC value and exponential regression coefficients.
 *
 * @param adcValue The ADC value to use for the calculation.
 * @param a The coefficient A from the exponential regression.
 * @param b The coefficient B from the exponential regression.
 * @return The calculated current value.
 */
//float calculateCurrent(int adcValue, float a, float b);
// Function to calculate current using the regression coefficients
float calculateCurrent(int adcValue, float a, float b) {
    return a * pow(b, adcValue);
}

//-----------------------------------------------------------------
//                        Notify Current function
//-----------------------------------------------------------------

/**
 * Notifies the connected BLE clients with the current motor value based on the 
 * calculated exponential regression coefficients.
 * 
 * This function calculates the current value using the `calculateCurrent()` function 
 * and the appropriate regression coefficients based on the current direction and side (driver or passenger). 
 * It then formats the current value and sends it to the connected BLE clients 
 * using the `notifyClients4()` function.
 * 
 * If the motor has not run long enough to calculate an average current, 
 * the function sends a "current data unavailable" notification to the BLE clients instead.
 */
void notify_current()
{
    //get motor average current
    int avg_current = average_current();
    if(avg_current > 0)
    {
        //apply current scaling
        float currentScaled;
        //driver or passenger side active?
        if(direction_mode[side_select] == 1)
            {
                //notify BLE data for current, down
                notify_data[2] = 0x62;    
                if(side_select == Driver_Side)
                {
                    currentScaled = calculateCurrent(avg_current, coefficientsDriverDown.A, coefficientsDriverDown.B);
                }
                else    //passenger side
                {
                    currentScaled = calculateCurrent(avg_current, coefficientsPassengerDown.A, coefficientsPassengerDown.B);
                }
            }
            else if(direction_mode[side_select] == 2)   //Up direction
            {
                //notify BLE data for current, up
                notify_data[2] = 0x64; 
                if(side_select == Driver_Side)
                {
                    currentScaled = calculateCurrent(avg_current, coefficientsDriverUp.A, coefficientsDriverUp.B);
                }
                else    //passenger side
                {
                    currentScaled = calculateCurrent(avg_current, coefficientsPassengerUp.A, coefficientsPassengerUp.B);
                }
            }      
        //shift decimal point 3 places, then convert float to integer
        int notify_current = static_cast<int>(currentScaled * 1000); 
        Serial.print("Notify value: ");
        Serial.println(notify_current); 
        //extract 2 lower bytes of run time integer and send to BLE as 2 separate bytes
        notify_data[1] = (notify_current >> 8) & 0xFF;
        notify_data[0] = notify_current & 0xFF;
        // notifyClients4(notify_data[3], notify_data[2], notify_data[1], notify_data[0]); //send data
    }
    else
    {
        //motor not on long enough for current average
        //notify BLE data for current
        direction_mode[side_select] == 1? notify_data[2] = 0x63 : notify_data[2] = 0x65;     //current data, unavailable, brown
        //notify_data[2] = 0x63;     //current data, down, brown
        // notifyClients2(notify_data[3], notify_data[2]); //send data
        Serial.println("Motor did not run long enough for current average");
    }         
}


    //-----------------------------------------------------------------
    //                          reset routine
    //----------------------------------------------------------------- 
    /**
     * Handles the reset button functionality, including debouncing, short press, and long press actions.
     * 
     * The reset routine checks the state of the reset pin and performs the following actions:
     * - Debounces the reset button press and release
     * - Detects a short press (less than 3 seconds) and ignores it
     * - Detects a long press (3-6 seconds) and performs a software reboot
     * - Detects a very long press (more than 6 seconds) and performs a factory reset, followed by a reboot
     * 
     * The factory reset function sets the default values for the window motor current and travel time, 
     * and resets the window position to partial.
     */
void reset_action()
{
    //test what state reset routine is in
    switch(reset_pin_active)
    {
        case 1:
            //save start debounce time
            reset_button_time = current_loop_time;
            reset_pin_active = 2;
            break;
        case 2:
            //test if debounce time has passed
            if(current_loop_time - reset_button_time > 30)
            {
                //read reset pin
                if(digitalRead(Reset_pin) == 0)
                {
                    //reset button pressed
                    reset_pin_active = 3;
                }
                else
                {
                    //reset button released
                    reset_pin_active = 4;
                }
                break;
            }
            else
            {
                break;  
            }
            
        case 3:
            //reset button pressed and debounced
            //save reset_start_time time stamp;
            reset_start_time = current_loop_time;
            reset_pin_active = 0;
            Serial.println("Reset button pressed, timer started");
            break;

        case 4:
            //reset button released and debounced
            //check if button held for more than 3 sec
            if((current_loop_time - reset_start_time) < 3000)
            {
                //reset button held less than 3 sec, ignore
                reset_pin_active = 0;
                Serial.println("Reset button released, IGNORED");
            }
            else if((current_loop_time - reset_start_time) < 6000)
            {
                //reset button held more than 3 sec, less than 6 sec
                //blink led 3 times and perform software reboot
                LedBlink(2, 3, 200);
                reset_pin_active = 0;
                Serial.println("Reset button released, rebooting");
                //software reset cpu
                ESP.restart();
            }
            else 
            {   
                Serial.println("Reset button, factory reset function");
                //reset button held for more than 6 sec
                //blink led 10 times and perform factory reset and reboot
                LedBlink(2, 10, 200);
                reset_pin_active = 0;
                writeIntIntoEEPROM(0, 1500);    //set default window motor current value for driver side down
                writeIntIntoEEPROM(1, 1500);    //set default window motor current value for driver side up 
                writeIntIntoEEPROM(2, 1500);    //set default window motor current value for passenger side down 
                writeIntIntoEEPROM(3, 1500);    //set default window motor current value for passenger side up
                writeIntIntoEEPROM(4, 4000);    //set default window travel time value for driver side down
                writeIntIntoEEPROM(5, 4000);    //set default window travel time value for driver side up 
                writeIntIntoEEPROM(6, 4000);    //set default window travel time value for passenger side down  
                writeIntIntoEEPROM(7, 4000);    //set default window travel time value for passenger side up  
                writeIntIntoEEPROM(8, 0);       //set window postion to partial for driver & passenger side
                Serial.println("Reset button released, Factory Reset, rebooting");
                //software reset cpu
                ESP.restart();                  //software reset cpu
            }
            break;
    } 
}

    //-----------------------------------------------------------------
    //                          external control routine
    //----------------------------------------------------------------- 
    /**
     * Handles external control functions, such as processing input from external switches.
     * This function is called when the `ext_control` flag is set, indicating that external
     * control input is available.
     * 
     * The function checks the state of the four external switches and updates the corresponding
     * internal state variables, such as `doorLockStates`, `BLEswitch`, `BLEside`, `RMT_control`, and
     * `BLEbyteReceived`. It also prints the state of each switch to the serial console.
     */
void ext_control_action()
{
    if (ext_control) {
        //ext_control = false; // Reset the flag
        
        //test the state of switches
        //if active, print which switch is active
        //perform the corresponding action to initiate the function
        
        if(ext_function & (1 << 0))
        {
           Serial.println("external unlock command received");
           //set doorLockStates to 2 to perform unlock function
           doorLockStates = 2;
        }

        if(ext_function & (1 << 1))
        {
           Serial.println("external lock command received");
           //set doorLockStates to  to perform lock function
           doorLockStates = 1;
        }


        if(ext_function & (1 << 2))
        {
            Serial.println("external windows down command received");
            //set values to perform windows down function 
            BLEswitch = 1;
            BLEside = Driver_Side;
            RMT_control = 2;
            BLEbyteReceived = true; //set flag to indicate BLE data received
        }
        // Serial.print("Switch 4: ");
        // Serial.println((ext_function & (1 << 3)) ? "Pressed" : "Released");
        if(ext_function & (1 << 3))
        {
            Serial.println("external windows up command received");
            //set values to perform windows up function 
            BLEswitch = 1;
            BLEside = Driver_Side;
            RMT_control = 1; 
            BLEbyteReceived = true; //set flag to indicate BLE data received
        }
    }
}

void ext_control_active()
{   
    static byte debounce_state = 0;
    static unsigned long debounce_time = 0;
    switch(debounce_state)
    {
        case 0:
            // Serial.print("ext control state: ");
            // Serial.println(debounce_state);
            //start debounce timer
            debounce_time = millis();
            debounce_state = 1;
            break;
        case 1:
            // Serial.print("ext control state: ");
            // Serial.println(debounce_state);
            //test if debounce time has elapsed, 20msec
            if((current_loop_time - debounce_time) > 20UL)
            {
                //debounce time has elapsed, next state
                debounce_state = 2;
            }
            else
            {
                //debounce time has not elapsed, no action
            }
            break;
        case 2:
            // Serial.print("ext control state: ");
            // Serial.println(debounce_state);
            //check if switch is active
            switch(remote_number)
            {
                case 1:
                    //remote lock
                    //test if switch value is valid
                    if(digitalRead(Remote_Lock_Switch_pin) == 0)
                    {
                        //valid switch value
                        Serial.println("Remote number is 1");
                        
                    }
                    else
                    {
                        //invalid switch value, reset state
                        debounce_state = 0;
                        remote_number = 0;
                    } 
                    break;   
                case 2:
                    //remote unlock
                    Serial.println("Remote number is 2");
                    break;
                case 3:
                    Serial.println("Remote number is 3");
                    break;
                case 4:
                    //remote window
                    Serial.println("Remote number is 4");
                    break;
            } 
            remote_number = 0;
            debounce_state = 0;       
            //remote window

    //determine which remote control is active
    //remote window - 0,0 & 0,1
    //remote lock - 1,0 & 1,1
    // bool found = false;
    // byte rmt_function = 0;
    // byte i=0;
    // byte j=0;
    // for(i = 0; i < 2 && !found; i++)  // Check both loops
    // {
    //     for(j = 0; j < 2; j++)
    //     {
    //         if(remote_control_state[i][j] == 1)
    //         {
    //             found = true;  // Set flag and break the outer loop
    //             break;
    //         }
    //         rmt_function++;
    //     }
    // }  
    // if (found) 
    // {
    //     i--;  // Decrement `i` to point to the correct position
    //     Serial.print("remote function: ");
    //     Serial.println(rmt_function);

    //     remote_control_state[i][j] = 0; // Reset remote control flag
    //     ext_control = false;
    //     remote_number = 0;
    //     debounce_state = 0;
    // } else {
    //     Serial.println("No active remote function found.");
    //     remote_number = 0;
    // }

    }
}

void printBinary(uint8_t value)
{
    for (int i = 7; i >= 0; i--) 
    {
        Serial.print((value >> i) & 0x01); // Shift and mask each bit
    }
    Serial.println(); // Move to the next line after printing the byte
}
