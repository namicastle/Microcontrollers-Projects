#include <p18f4620.h>
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;
  
int get_RPM()
{
    int RPS = TMR3L / 2;                                                        // read the count. Since there are 2 pulses per rev then RPS = count /2                                                                       
    TMR3L = 0;                                                                  // clear out the count
    return (RPS * 60);                                                          // return RPM = 60 * RPS
}
void Turn_Off_Fan()                                                                 
{   
    FAN = 0;                                                                    // 
    FAN_EN = 0;                                                                 // turn on fan
    FAN_EN_LED = 0;                                                             // turn off FAN led
}
void Turn_On_Fan()
{
    FAN = 1;                                                                    //
    do_update_pwm(duty_cycle);                                                  // update pwm
    FAN_EN = 1;                                                                 // turn on fan
    FAN_EN_LED = 1;                                                             // turn on FAN led
}
void Toggle_Fan()
{
    if(FAN == 0)                                                                //
        Turn_On_Fan();                                                          // turn on fan
    else                                                                        //
        Turn_Off_Fan();                                                         // turn off fan if FAN == 1
}
void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;
    int dc_I; 
    PR2 = 0b00000100 ;                                                          // set the frequency for 25 Khz 
    T2CON = 0b00000111 ;                                                        // 
    dc_f = ( 4.0 * duty_cycle / 20.0) ;                                         // calculate factor of duty cycle versus a 25 Khz signal
    dc_I = (int) dc_f;                                                          // get the integer part
    if (dc_I > duty_cycle) dc_I++;                                              // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100; 
    CCPR1L = (dc_I) >> 2; 
} 
void Increase_Speed()
{
    if(duty_cycle == 100)                                                       //
    {                                                                           
        Do_Beep();                                                              // beep 2x
        Do_Beep();                                                              //
        do_update_pwm(duty_cycle);                                              // call the function so it wont kill the pwm
    }
    else
        do_update_pwm(duty_cycle += 5);                                         // increment duty cycle by 5
}
void Decrease_Speed()
{
    if(duty_cycle == 0)                                                         //
    {
        Do_Beep();                                                              // beep 2x
        Do_Beep();                                                              //
        do_update_pwm(duty_cycle);                                              // call the function so it wont kill the pwm
    }
    else    
        do_update_pwm(duty_cycle -= 5);                                         // decrement duty cycle by 5         
}
void Set_DC_RGB(int duty_cycle)
{
    int color = duty_cycle/10;                                                  // we only need the first digit of the duty cycle value
    if (color >= 7) PORTE = 7;                                                  // then if the first digit is more than seven then it is white
    PORTE = color;                                                              // if not then PORTE displays whatever color is
}
void Set_RPM_RGB(int rpm)
{
    if(rpm == 0) PORTB = 0;                                                     // if rpm is 0 then turn off led
    if(rpm>= 3000) PORTB = 0x31;                                                // if more than 3k then white led
    if(rpm > 0 && rpm < 3000) PORTB = ((rpm / 500) + 1) << 3;                   // if in between then divide by 500 then plus 1 then shift to left 3x
}

