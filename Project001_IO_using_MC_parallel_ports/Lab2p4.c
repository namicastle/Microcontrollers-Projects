#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>

#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF


void delay_one_sec()                    //delay function
{                                       //
    for(int i =0; i<17000; i++);        //
}

void main() 
{ 
    TRISC = 0x00;                       // set TRISC as output 
    ADCON1 = 0x0F;                      // Make it digital
    TRISD = 0x00;                       // set TRISD as output 
    
    while (1)                           // inifite loop
    {   
        for(int i = 0; i < 8; i++)
        {
            PORTC = i;                  // output of PORTC
            PORTD = 7-i;                // inverse of the output of PORTC
            delay_one_sec();            // delay function so we see the transition between different colors
        }
    }   
} 
