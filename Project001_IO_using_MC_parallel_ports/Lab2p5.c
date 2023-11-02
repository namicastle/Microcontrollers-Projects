#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>

#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF


void delay_one_sec()                                                //delay function
{                                                                   //
    for(int i =0; i<17000; i++);                                    //
}

void main()                                                         //
{                                                                   //
    TRISC = 0x00;                                                   // set TRISC as output
    ADCON1 = 0x0F;                                                  // Make it digital
    TRISD = 0x00;                                                   // set TRISC as output
    char lightD23[8] = {0x85,0xA7,0xE3,0x06,0x21,0x62,0xC4,0x40};   // pattern of colors in hex for both D2 and D3 combined together
                                                                    // because they are both in PORTD. Last 3 bits are D3 first 3 bits for D2
    while (1)                                                       // infinite loop
    { 
        for(int i = 0; i < 8; i++)                                  //
        {                                                           //
            PORTC = i;                                              // output of PORTC
            PORTD = lightD23[i];                                    // set the values of array to PORTD depending on i
            delay_one_sec();                                        // delay function so we see the transition between different colors
        }
    }   
} 
