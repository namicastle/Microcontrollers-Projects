#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f4620.h>
#include "utils.h"

//#define KEY_PRESSED     PORTDbits.RD3
#define SCL             PORTDbits.RD7
#define SDA             PORTCbits.RC4


void wait_one_sec()                                                             // wait one sec routine
{
    for (int k=0;k<30000;k++);
}
void Activate_Buzzer()                                                          // activate buzzer function
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}
void Deactivate_Buzzer()                                                        // deactivate buzzer function
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}
void Do_Beep()
{
    Activate_Buzzer();
    wait_one_sec();
    Deactivate_Buzzer();                                               // need so activate beep wont destroy the original pwm
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




