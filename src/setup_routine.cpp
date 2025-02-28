#include "setup_routine.h"
#include "esp_task_wdt.h"
// this header is needed for Bluetooth Serial -> works ONLY on ESP32
//#include "BluetoothSerial.h" 

void applyCurrentCalculation(int adc_value, float slope, float intercept) {
    float current = adc_value * slope + intercept;
    Serial.print("ADC Value: ");
    Serial.print(adc_value);
    Serial.print(", Current: ");
    Serial.println(current);
    }

void setup_routine()
{
    
    Serial.begin(115200);
    delay(100);
    Serial.println("Begin Setup Routine");
    //noInterrupts();

    //delay(500);
    // assign button input types
    //assign driver side window button inputs
    pinMode(button[0][1], INPUT_PULLUP);                   // switch 1, manual down
    pinMode(button[0][2], INPUT_PULLUP);                   // switch 2, manual up
    pinMode(button[0][3], INPUT_PULLUP);                   // switch 3, auto
    //assign passenger side window button inputs
    pinMode(button[1][1], INPUT_PULLUP);                   // switch 4, manual down
    pinMode(button[1][2], INPUT_PULLUP);                   // switch 5, manual up
    pinMode(button[1][3], INPUT_PULLUP);                   // switch 6, auto
    //assign window motor curent monitor pins
    pinMode(Driver_motor_current_pin, INPUT);              // motor current for driver window
    pinMode(Passenger_motor_current_pin, INPUT);           // motor current for passenger window 
    //asign window remote control pins
    pinMode(Remote_Window_Down_pin, INPUT_PULLUP);
    pinMode(Remote_Window_Up_pin, INPUT_PULLUP);
    //assign door lock/unlock input pins
    pinMode(Driver_Lock_Switch_pin, INPUT_PULLUP);
    pinMode(Driver_Unlock_Switch_pin, INPUT_PULLUP); 
    pinMode(Passenger_Lock_Switch_pin, INPUT_PULLUP);
    pinMode(Passenger_Unlock_Switch_pin, INPUT_PULLUP); 
    //assign door lock/unlock motor control pins
    pinMode(Driver_Door_Lock_Control_pin, OUTPUT);                     //DRV8871 IN1
    pinMode(Passenger_Door_Lock_Control_pin, OUTPUT);                   //DRV8871 IN2
    //assign door lock remote control pins
    pinMode(Remote_Lock_Switch_pin, INPUT_PULLUP);
    pinMode(Remote_Unlock_Switch_pin, INPUT_PULLUP);
    //assign reset button pin
    pinMode(Reset_pin, INPUT_PULLUP);
    //assign LED pin       
    pinMode(LedPin, OUTPUT);
    //blink led 0.5 sec
    digitalWrite(LedPin, HIGH);
    delay(500);
    digitalWrite(LedPin, LOW);  

    
    //pinMode(Driver_IBT2_UP_pin, OUTPUT);
    //

    // configure LED PWM functionalities
    //ledcSetup(Driver_Down_PWM_channel, freq, PWM_resolution);
    //ledcSetup(Driver_Up_PWM_channel, freq, PWM_resolution);
    //ledcSetup(Passenger_Down_PWM_channel, freq, PWM_resolution);
    //ledcSetup(Passenger_Up_PWM_channel, freq, PWM_resolution);
    ledcSetup(Beeper_PWM_channel, Beeper_freq, PWM_resolution);  
    ledcSetup(Driver_Lock_PWM_channel, 4000, PWM_resolution);  
    ledcSetup(Passenger_Lock_PWM_channel, 4000, PWM_resolution);
  
    // attach the channel to the GPIO to be controlled
    //ledcAttachPin(Driver_IBT2_DOWN_pin, Driver_Down_PWM_channel);
    //ledcAttachPin(Driver_IBT2_UP_pin, Driver_Up_PWM_channel);
    //ledcAttachPin(Passenger_IBT2_DOWN_pin, Passenger_Down_PWM_channel);
    //ledcAttachPin(Passenger_IBT2_UP_pin, Passenger_Up_PWM_channel);
    ledcAttachPin(Driver_Lock_PWM_pin, Driver_Lock_PWM_channel);
    ledcAttachPin(Passenger_Lock_PWM_pin, Passenger_Lock_PWM_channel);
    ledcAttachPin(Beeper_Pin, Beeper_PWM_channel);

    //set motors OFF
    //digitalWrite(Driver_IBT2_en_pin, LOW);
    //ledcWrite(Driver_Down_PWM_channel, Speed);
    //ledcWrite(Driver_Up_PWM_channel, Speed);
    //ledcWrite(Passenger_Down_PWM_channel, Speed);
    //ledcWrite(Passenger_Up_PWM_channel, Speed);
    ledcWrite(Driver_Lock_PWM_channel, Lock_Motor_Speed);
    ledcWrite(Passenger_Lock_PWM_channel, Lock_Motor_Speed);
    ledcWrite(Beeper_PWM_channel, Beep_Duty_Cycle);

    //init door lock states
    button_last_valid[Driver_Side][4] =  digitalRead(button[Driver_Side][4]);
    button_last_valid[Driver_Side][5] =  digitalRead(button[Driver_Side][5]);
    button_last_valid[Passenger_Side][4] =  digitalRead(button[Passenger_Side][4]);
    button_last_valid[Passenger_Side][5] =  digitalRead(button[Passenger_Side][5]);
    
   
    
    //config ADC
    // Set the sample bits and resolution.
    // It can be a value between 9 (0 – 511) and 12 bits (0 – 4095).
    // analogSetWidth(10); //10 bits width
    // Attach a pin to the ADC (also clears any other active analog mode).
    // Returns a TRUE or FALSE result.
    adcAttachPin(Driver_motor_current_pin);
    adcAttachPin(Passenger_motor_current_pin);

    


 
    //*********************************TESTING****************************************
    disableCore0WDT();  // For Core 0
    disableCore1WDT();  // For Core 1 
    //*********************************TESTING****************************************

    

    //set up EEPROM
    // Allocate The Memory Size Needed
    EEPROM.begin(EEPROM_SIZE);

    //test if EEPROM has been written to
    //if not then write default values
    
    // writeIntIntoEEPROM(0,300);
    // writeIntIntoEEPROM(1,750);
    // writeIntIntoEEPROM(2,100);
    // writeIntIntoEEPROM(3,0xFFFF);

    //read eeprom & check values
    //all 1's means eeprom is new & needs initial values to be stored
    //values less than 500 also require initial values
    for (byte loop_count = 0; loop_count < 9; loop_count ++)
    {
        //read eeprom value from address loop count
        switch (loop_count)
        {
            unsigned int eeprom_value;
            case 0: case 1: case 2: case 3:
                //motor average current
                //read eeprom value from address loop count
                eeprom_value = readIntFromEEPROM(loop_count);
                if (eeprom_value == 0xFFFF || eeprom_value < 500)
                {
                    writeIntIntoEEPROM(loop_count, 1500);     //set average current initial values
                }
                break;
            
            case 4: case 5: case 6: case 7:
                //window run time
                //read eeprom value from address loop count
                eeprom_value = readIntFromEEPROM(loop_count);
                if (eeprom_value == 0xFFFF )
                {
                    writeIntIntoEEPROM(loop_count, 4000);     //set average run time initial values
                }
                break;

            case 8:
                //window position
                //read eeprom value from address loop count
                eeprom_value = readIntFromEEPROM(loop_count);
                if (eeprom_value == 0xFFFF )
                {
                    writeIntIntoEEPROM(loop_count, 0);     //set window position initial values
                }
                break;
        }        
    }

    //read 4 integers from eeprom
    //to recall motor average current 
    //then apply auto stop sensitivity value to create current alarm
    motor_current_alarm[0][0] = readIntFromEEPROM(0) * auto_stop_sensitivity;   //get drivers side down motor average current
    motor_current_alarm[0][1] = readIntFromEEPROM(1) * auto_stop_sensitivity;   //get drivers side up motor average current
    motor_current_alarm[1][0] = readIntFromEEPROM(2) * auto_stop_sensitivity;   //get passenger side down motor average current
    motor_current_alarm[1][1] = readIntFromEEPROM(3) * auto_stop_sensitivity;   //get passenger side up motor average current

    // writeIntIntoEEPROM(4, 5000);
    // writeIntIntoEEPROM(5, 5000); 
    // writeIntIntoEEPROM(6, 5000);  
    // writeIntIntoEEPROM(7, 5000);
    // writeIntIntoEEPROM(8, 0000);

    //read 4 integers from eeprom
    //to recall window run time alarms
    window_run_time_alarm[0][0] = readIntFromEEPROM(4) + window_overrun_timeout;   //get drivers side window down run time average
    window_run_time_alarm[0][1] = readIntFromEEPROM(5) + window_overrun_timeout;   //get drivers side window up run time average
    window_run_time_alarm[1][0] = readIntFromEEPROM(6) + window_overrun_timeout;   //get passenger side window down run time average
    window_run_time_alarm[1][1] = readIntFromEEPROM(7) + window_overrun_timeout;   //get passenger side window up run time average
    
    //read 2 bytes from eeprom
    //to recall window position flags
    position_mode[0] = EEPROM.read(8 * 2);       //get drivers side window position value
    position_mode[1] = EEPROM.read(8 * 2 + 1);   //get passenger side window position value
    
    #ifdef DEBUG
        Serial.println("Window Motor Current Alarm Thresholds ");
        Serial.print("EEPROM_0, Driver Down: ");
        Serial.println(motor_current_alarm[0][0]);
        Serial.print("EEPROM_1, Driver Up: ");
        Serial.println(motor_current_alarm[0][1]);
        Serial.print("EEPROM_2, Passenger Down: ");
        Serial.println(motor_current_alarm[1][0]);
        Serial.print("EEPROM_3, Passenger Up: ");
        Serial.println(motor_current_alarm[1][1]);
        Serial.println("Window Run Time Alarm Thresholds ");
        Serial.print("EEPROM_4, Driver Down: ");
        Serial.println(window_run_time_alarm[0][0]);
        Serial.print("EEPROM_5, Driver Up: ");
        Serial.println(window_run_time_alarm[0][1]);
        Serial.print("EEPROM_6, Passenger Down: ");
        Serial.println(window_run_time_alarm[1][0]);
        Serial.print("EEPROM_7, Passenger Up: ");
        Serial.println(window_run_time_alarm[1][1]);
        Serial.println("Window Position Status ");
        Serial.print("Driver Window Position - ");
        switch(position_mode[0])
        {
            case 0:
                Serial.println("Partial");
                break;
            case 1:
                Serial.println("Down");
                break;
            case 2:
                Serial.println("Up");
                break;
        }
        Serial.print("Passenger Window Position - ");
        switch(position_mode[1])
        {
            case 0:
                Serial.println("Partial");
                break;
            case 1:
                Serial.println("Down");
                break;
            case 2:
                Serial.println("Up");
                break;
        }
    #endif

    //door lock config
    //set up door lock mode, 
    //1 = 2-5 pin motor/lock switches, 
    //2 = 1-5 pin motor/lock switch + 1 - 2 pin lock motor,
    //3 = driver, 2 door panel switches, passenger, 1 door panel switch + 2 - 2 pin lock motors
    //4 = driver, 1 door panel switch with passenger control, passenger, 1 door panel switch + 2 - 2 pin lock motors
    door_lock_mode = 2;
    //init door lock status
    switch (door_lock_mode)
    {
        case 1:
            Serial.println("Door Lock Mode 1");
            //2-5 pin motor/lock switches
            //enable interrupts for 5pin lock switches
            // attachInterrupt(digitalPinToInterrupt(button[0][4]), button7_ISR, RISING);      //driver side lock
            // attachInterrupt(digitalPinToInterrupt(button[0][5]), button8_ISR, RISING);      //driver side unlock
            // attachInterrupt(digitalPinToInterrupt(button[1][4]), button9_ISR, RISING);      //passenger side lock
            // attachInterrupt(digitalPinToInterrupt(button[1][5]), button10_ISR, RISING);     //passenger side unlock

            //read & set last state values
            button_last_valid[0][4] =  digitalRead(button[0][4]);   //driver unlock switch
            button_last_valid[0][5] =  digitalRead(button[0][5]);   //driver lock switch
            button_last_valid[1][4] =  digitalRead(button[1][4]);   //passenger unlock switch
            button_last_valid[1][5] =  digitalRead(button[1][5]);   //passenger lock switch
            //set interrupts based on door lock position
            //at this point all interrupts are enabled as rising edge interrupts
            //only allow interrupt for the driver lock switch that is low
            button_last_valid[0][4] == 1 ? detachInterrupt(button[0][4]) : detachInterrupt(button[0][5]);
            //only enable interrupt for the passenger lock switch that is low
            button_last_valid[1][4] == 1 ? detachInterrupt(button[1][4]) : detachInterrupt(button[1][5 ]);
            break;

        case 2:
            Serial.println("Door Lock Mode 2");
            //1-5 pin motor/lock switches
            //enable interrupts for 5pin lock switch
            // attachInterrupt(digitalPinToInterrupt(button[0][4]), button7_ISR, RISING);      //driver side lock
            // attachInterrupt(digitalPinToInterrupt(button[0][5]), button8_ISR, RISING);      //driver side unlock
            
            //read & set last state values
            button_last_valid[0][4] =  digitalRead(button[Driver_Side][4]);   //driver lock switch
            button_last_valid[0][5] =  digitalRead(button[Driver_Side][5]);   //driver unlock switch
            //set driver side interrupts based on door lock position
            //at this point all interrupts are enabled as rising edge interrupts
            //only allow interrupt for the driver lock switch that is low
            button_last_valid[0][4] == 1 ? detachInterrupt(button[0][4]) : detachInterrupt(button[0][5]);
            break;
        case 3:
            Serial.println("Door Lock Mode 3");
            //2 door panel switches + 2 - 2 pin lock motors
            break;
    }
    

    #ifdef DEBUG
        
        if(door_lock_mode == 1)
        {
            //print door lock states
            Serial.println("Door Lock Statuses ");
            Serial.print("Driver Lock Status ");
            Serial.println(button_last_valid[0][4]==0?"Yes - 0":"No - 1");
            Serial.print("Driver Unlock Status ");
            Serial.println(button_last_valid[0][5]==0?"Yes - 0":"No - 1");
            Serial.print("Passenger Lock Status ");
            Serial.println(button_last_valid[1][4]==0?"Yes - 0":"No - 1");
            Serial.print("Passenger Unlock Status ");
            Serial.println(button_last_valid[1][5]==0?"Yes - 0":"No - 1");
        }
        else if(door_lock_mode == 2)
        {
            //print door lock states
            Serial.println("Door Lock Statuses ");
            Serial.print("Driver Lock Status, ");
            Serial.println(button_last_valid[0][4]==0?"Yes - 0":"No - 1");
            Serial.print("Driver Unlock Status ");
            Serial.println(button_last_valid[0][5]==0?"Yes - 0":"No - 1");
        }
    #endif

    //set gpio mode for button 1
    //needs delay as it is a boot strapping pin
    pinMode(button[0][1], INPUT_PULLUP);            // switch 1

    #ifndef DEBUG
        // Configure the watchdog timer if not in debug mode
        esp_task_wdt_init(WATCHDOG_TIMEOUT_SECONDS, true);
        esp_task_wdt_add(NULL); // Add the current task to the watchdog
    #endif

    //check power on reason
    esp_reset_reason_t reason = esp_reset_reason();
    //print power on reason

    switch ( reason)
    {
        case 1 : Serial.println ("POWERON_RESET");break;          /**<1, Vbat power on reset*/
        case 3 : Serial.println ("SW_RESET");break;               /**<3, Software reset digital core*/
        case 4 : Serial.println ("OWDT_RESET");break;             /**<4, Legacy watch dog reset digital core*/
        case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5, Deep Sleep reset digital core*/
        case 6 : Serial.println ("SDIO_RESET");break;             /**<6, Reset by SLC module, reset digital core*/
        case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7, Timer Group0 Watch dog reset digital core*/
        case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8, Timer Group1 Watch dog reset digital core*/
        case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9, RTC Watch dog Reset digital core*/
        case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
        case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
        case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
        case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
        case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
        case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
        case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
        default : Serial.println ("NO_MEAN");
    }

    #ifdef DEBUG
        // Code for debugging
        //assign psram pins as outputs
        // pinMode(33, OUTPUT);    //gpio is not available on esp32-s3 devkitC-1
        // pinMode(34, OUTPUT);    //gpio is not available on esp32-s3 devkitC-1
        // pinMode(35, OUTPUT);
        // pinMode(36, OUTPUT);
        // pinMode(37, OUTPUT);
        // digitalWrite(33, HIGH);     //gpio is not available on esp32-s3 devkitC-1
        // digitalWrite(34, LOW);      //gpio is not available on esp32-s3 devkitC-1
        // digitalWrite(35, HIGH);
        // digitalWrite(36, LOW);
        // digitalWrite(37, HIGH);
    #endif

   //calibrate the current adc output to voltage
    calculateMotorData();

    #ifdef DEBUG
        //test current display
        float current_data;

        // Passenger Down test
        current_data = calculateCurrent(2281, coefficientsPassengerDown.A, coefficientsPassengerDown.B);
        Serial.print("Passenger down current (3.75): ");
        Serial.print(current_data);
        Serial.print(" float to Notify: ");
        Serial.println(current_data * 1000);

        // Driver Down test
        current_data = calculateCurrent(1214, coefficientsDriverDown.A, coefficientsDriverDown.B);
        Serial.print("Driver down current (1.97): ");
        Serial.println(current_data, 2);

        // Driver Up test
        current_data = calculateCurrent(2744, coefficientsDriverUp.A, coefficientsDriverUp.B);
        Serial.print("Driver up current (5.13): ");
        Serial.println(current_data, 2);
    #endif

    //sound a beep alarm
    sound_assign(11);
    
    //setup complete
    Serial.println("Setup complete");


}