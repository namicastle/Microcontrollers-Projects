#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>

#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF

void main() 
{ 
    char in = 0;                    // Use variable ?in? as char 
    TRISA = 0xFF;                   // set TRISA as input so set every bit to 1 which is in hex is 0xFF 
    TRISB = 0x00;                   // set TRISC as output so set all bit to 0 which is in hex is 0x00
    ADCON1 = 0x0F;                  //  Make it digital
 
 while (1) 
{ 
    in = PORTA;                     // read data from PORTA and save it 
    in = in & 0x07;                 // Mask out the unused upper four bits 
                                    // while preserving the lower 3-bits 
    PORTC = in;                     // Output the data to PORTC
} 
} 