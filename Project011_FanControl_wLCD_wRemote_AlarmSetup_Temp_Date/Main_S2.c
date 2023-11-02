#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>
#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

void test_alarm();
void Monitor_Fan();
int time_matched();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char fan_set_temp = 75;
unsigned char Nec_state = 0;
float volt;
char INT1_flag, INT2_flag;

short Nec_OK = 0;
char Nec_code1;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED,color;

char txt1[21][4] ={"CH-\0"," CH\0" ,"CH+\0","VL-\0","VL+\0","P/P\0",            // array to display the name of the remote keys
" - \0"," + \0"," EQ\0"," 0 \0","100\0","200\0"," 1 \0"," 2 \0",
" 3 \0"," 4 \0"," 5 \0"," 6 \0"," 7 \0"," 8 \0"," 9 \0"};

char buffer[31]     = " ECE3301L Sp'23 Final\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_Set_Temp_Txt[] = "075F";
char Fan_SW_Txt[]   = "OFF";                                                    // text storage for Heater Mode
char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
   
char DC_Txt[]       = "000";                                                    // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                      
char RPM_Txt[]      = "0000";                                                   // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_set_text[]   = "075F";

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}
void Do_Init()                                                                  // Initialize the ports 
{ 
    init_UART();                                                                // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                                                                // Set oscillator to 8 MHz 
    
    ADCON1=0x0E;                                                                // ADCON1
    TRISA = 0x01;                                                               // PORTA bit 0 as input the rest output
    TRISB = 0x07;                                                               // PORTB bit 0-2 as input the rest output
    TRISC = 0x01;                                                               // PORTC bit 0 as input the rest output
    TRISD = 0x00;                                                               // PORTD as output
    TRISE = 0x00;                                                               // PORTE as output
    PORTE = 0x00;
    Initialize_Screen();                                                        // initialize screen    
    FAN = 0;
    RBPU=0;

    TMR3L = 0x00;                                                               // clear timer
    T3CON = 0x03;                                                               // initialize T3CON
    I2C_Init(100000); 

    DS1621_Init();  
    Init_Interrupt();
    Turn_Off_Fan();
    fan_set_temp = 75;                                                          // initialize fan_set_temp
}

void main() 
{
    Do_Init();                                                                  // Initialization  
     
    FAN_EN = 0;                                                                 // Turn Fan on
    FAN_EN_LED = 0;                                                             // Turn Fan LED on
    
    do_update_pwm(duty_cycle);                                                  // Update duty cycle
    
    alarm_mode = 0;                                                             
    ALARMEN = 0;
    Set_RGB_Color(0);                                                           // off LED
                
    DS3231_Turn_Off_Alarm();                                    
    DS3231_Read_Alarm_Time();                                                   // Read alarm time for the first time

    tempSecond = 0xff;
    while (1)
    {
        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();                                                    // measure the fan's rpm
            volt = read_volt();                                                 // measure the voltage of light sensor
            DS1621_tempC = DS1621_Read_Temp();                                  // measure the ambient temp
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
            Set_RPM_RGB(rpm);                                                   // display proper LED color that matched the rpm value
            Set_DC_RGB(duty_cycle);                                             // // display proper LED color that matched the rpm value
                
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            
            Monitor_Fan();                                                      // call function of Monitor_Fan();
            test_alarm();
            Update_Screen();
        }
        
        if (check_for_button_input() == 1)
        {
            Nec_OK = 0;
            printf ("FOUND = %d\r\n", found);  
            switch (found)
            {
                case Setup_Time_Key:        
                    Do_Beep_Good();                                             // beep
                    Do_Setup_Time();                                            // call the setup time function
                    break;
                case Setup_Alarm_Key:           
                    Do_Beep_Good();
                    Do_Setup_Alarm_Time();                                      // call the setup alarm function
                    break;                  
                case Setup_Fan_Temp_Key:
                    Do_Beep_Good();
                    Setup_Temp_Fan();                                           // call the setup fan function
                    break;                  
                case Toggle_Fan_Key:
                    Do_Beep_Good();
                    Toggle_Fan();                                               // call the toggle function, turn on and off the fan
                    break;           
                default:
                    Do_Beep_Bad();                                              // if other keys are pressed on the remote then beep bad
                    break;
            }
        } 

        if (INT1_flag == 1)                                                     
        {
            INT1_flag = 0;                                                      // int1 flag to 0
            if (ALARMEN == 0) ALARMEN = 1;                                      // if ALARMEN is 0 then set it to 1
            else ALARMEN = 0;                                                   // if its 1 then set it to 0
        }        
    }
}
void Monitor_Fan()                                                  
{
    if(FAN == 0) Turn_Off_Fan();                                                // checks to turn on and off the fan
    else Turn_On_Fan();
    
    float dc_float;
    if(DS1621_tempF >= fan_set_temp)                                            
        duty_cycle = 0;
    
    float diff_temp = fan_set_temp - DS1621_tempF;                              // calculate the diff_temp
    if (DS1621_tempF < fan_set_temp)
    {
        if (diff_temp > 0)                                                      // if more than 0 
            duty_cycle=0;                                                       // duty cycle is at 0 also
        if (35<=diff_temp && diff_temp < 50)                                    // if in between 35 and 50
        {   
            dc_float = diff_temp* 2.0;                                          // calculate dc_float and convert calculated dc to int and 
            duty_cycle = (int)dc_float;                                         // set it equal to duty_cycle
        }
        if (25<=diff_temp && diff_temp < 35)                                    // if in between 25 and 35 
        {   
            dc_float = diff_temp*(3/2);                                         // then calculate new dc_float
            duty_cycle = (int)dc_float;                                         // set it equal to duty cycle
        }
        if (10<=diff_temp && diff_temp < 25)                                    // if between 10 and 25
        {
            dc_float = diff_temp*(1.25);                                        // then calculate new dc_float
            duty_cycle = (int)dc_float;                                         // then set it equal to duty cycle
        }
        if (0<=diff_temp && diff_temp < 10)                                     // if between 0 and 10
            duty_cycle = (int)diff_temp;                                        // the diff temp equals to duty cycle
    }
    do_update_pwm(duty_cycle);                                                  // update duty cucle
}
void test_alarm()
{  
    if((alarm_mode == 0) && (ALARMEN == 1))     
    {
        DS3231_Turn_On_Alarm();                                                 // turn on alarm
        alarm_mode = 1;                                                         // alarm mode is on
        MATCHED = 0;                                                            
    }
    if((alarm_mode == 1) && (ALARMEN == 0)) 
    {
        DS3231_Turn_Off_Alarm();                                                // turn off alarm
        alarm_mode = 0;                                                         // alarm mode is off
        Deactivate_Buzzer();                                                    // deact buzzer
        Set_RGB_Color(0);                                                       // off LED
        MATCHED = 0;                                                            // it still not matched
    }
    if((alarm_mode == 1) && (ALARMEN == 1)) 
    {
        if(RTC_ALARM_NOT == 0 || time_matched() == 1)
        {
            Activate_Buzzer();                                                  //  activate buzzer
            MATCHED = 1;
        }   
        if(MATCHED == 1)
        {
            Set_RGB_Color(color++);                                             // set LED from no color to white
                if (color>7)
                    color = 0;
                volt = read_volt();                                             // read volt
                if(volt>=3.0)                                                   // if more than 3
                {
                    MATCHED = 0;                                                // set MATCHED back to 0
                    Set_RGB_Color(0);                                           // turn off LED
                    Deactivate_Buzzer();                                        // deact buzzer
                    do_update_pwm(duty_cycle);                                  // update duty cycle
                    alarm_mode = 0;                                             // alarm mode is off
                }
                else Activate_Buzzer();                                         // if not then activate buzzer
            }             
    }
}
int time_matched()                                                              // function if the time match on the alarm and system clock
{
    if((hour == alarm_hour)                                                     // if matches all hour, minute, and second
       && (minute == alarm_minute) 
       && (second == alarm_second))
    {
        RTC_ALARM_NOT = 0;                                                      // then set RTC to 0 and return a value of 1
        return 1;
    } 
    else 
    {
        RTC_ALARM_NOT = 1;                                                      // if not then RTC to 1 and return a value of 0
        return 0;   
    }
}






