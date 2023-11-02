#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f4620.h>
#include "utils.h"

extern char found;
extern char Nec_code1;
extern short Nec_OK;
extern char array1[21];
extern char duty_cycle;

char check_for_button_input()
{       
    if (Nec_OK  == 1)
    {
            Nec_OK  = 0;
            printf ("NEC_Code = %x\r\n", Nec_code1);
            INTCON3bits.INT2IE = 1;                                             // Enable external interrupt
            INTCON2bits.INTEDG2 = 0;                                            // Edge programming for INT0 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_code1 == array1[j])                                     // look through array and if match
                {                                                               
                    found = j;                                                  // set found to the location of the array
                    j = 21;
                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
                return (0);
            }
            else
            {
                return (1);
            }
    }
}

char bcd_2_dec (char bcd)
{
    int dec;
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}

void Do_Beep()
{
    Activate_Buzzer();                                                          // activate buzzer
    PORTDbits.RD3 = 1;                                                          // turn on LED
    Wait_One_Sec();                                                             // wait 1 sec
    Deactivate_Buzzer();                                                        // deactivate buzzer
    PORTDbits.RD3 = 0;                                                          // turn off LED
    Wait_One_Sec();                                                             // wait 1 sec
    do_update_pwm(duty_cycle);                                                  // update duty cycle
}

void Do_Beep_Good()
{
    Activate_Buzzer_2KHz();                                                     // activate buzzer with 2Kz
    PORTDbits.RD3 = 1;                                                          // turn on LED
    Wait_One_Sec();                                                             // wait 1 sec
    Deactivate_Buzzer();                                                        // deactivate buzzer
    PORTDbits.RD3 = 0;                                                          // turn off LED
    Wait_One_Sec();                                                             // wait 1 sec
    do_update_pwm(duty_cycle);                                                  // update duty cycle
}

void Do_Beep_Bad()
{
    Activate_Buzzer_500Hz();                                                    // activate buzzer with 500hz
    PORTDbits.RD3 = 1;                                                          // turn on LED
    Wait_One_Sec();                                                             // wait 1 sec
    Deactivate_Buzzer();                                                        // deactivate buzzer
    PORTDbits.RD3 = 0;                                                          // turn off LED
    Wait_One_Sec();                                                             // wait 1 sec
    do_update_pwm(duty_cycle);                                                  // update duty cycle
}

void Wait_One_Sec()                                                             // delay 1 sec
{
    for (int k=0;k<0xffff;k++);
}

void Activate_Buzzer()                                                          // activate buzzer function
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_500Hz()                                                    // activate buzzer function for 500hz
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000111 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_2KHz()                                                     // activate buzzer function for 2khz
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_4KHz()                                                     // activate buzzer function for 4khz
{
    PR2 = 0b01111100 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b00111110 ;
    CCP2CON = 0b00011100 ;
}

void Deactivate_Buzzer()                                                        // deactivate buzzer function
{
    CCP2CON = 0x0;
	PORTBbits.RB3 = 0;
}
void Set_RGB_Color(char color)
{
    PORTD = (PORTD & 0x7) | (color << 4);
}
void Set_ADCON0 (char ch)
{
    ADCON0 = ch * 4 + 1;
}
float read_volt()
{
    Set_ADCON0(0);
    int nStep = get_full_ADC();                                                 // calculates the # of steps for analog conversion
    return (nStep * 5 /1024.0);
}
unsigned int get_full_ADC()
{
    unsigned int result;
    ADCON0bits.GO=1;                                                            // Start Conversion
    while(ADCON0bits.DONE==1);                                                  // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                                         // combine result of upper byte and                                                                             // lower byte into result
    return result;
}
void Init_ADC()                                                                 // ADCONx initializations 
{
    ADCON0 = 0x01;
    ADCON1 = 0x0B; 
    ADCON2 = 0xA9;
}



