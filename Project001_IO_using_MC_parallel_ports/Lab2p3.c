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
{
    for(int i =0; i<17000; i++);
}

void main() 
{ 
    TRISC = 0x00;                       // Set TRISC as an output 
    ADCON1 = 0x0F;                      // fill out the ?? with the proper values 
                                        
    while (1)                           // use infinite loop
    { 
        for(int i = 0; i < 8; i++)      // portc will display the 7 colors including no light
        {                               // with for loop. The color code depends on the decimal from the manual
            PORTC = i;
            delay_one_sec();            // delay function so we see the transition between different colors
        }
    }   
} 
