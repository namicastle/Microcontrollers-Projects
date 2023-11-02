#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 
                                                                                // Constant Declerations
#define D1_RED PORTEbits.RE0                                                    // for PORTE bit 0 & 2 (D1)
#define D1_GREEN PORTEbits.RE2                                                  

#define D2_RED PORTCbits.RC2                                                    // for PORTC bit 2 to 4 (D2)
#define D2_GREEN PORTCbits.RC3 
#define D2_BLUE PORTCbits.RC4 

#define DP PORTDbits.RD7                                                        // for DP bit from 7segment display

char LED_7seg[10] = {0x01, 0x4F, 0x12, 0x06,0x4C,0x24,0x20,0x0F,0x00,0x04};     // array for 10 different colors of LED

void Init_UART()                                                                // This routine is to initialize the UART                 
{                                                                               // function to use TeraTerm
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &                            //
            USART_ASYNCH_MODE & USART_EIGHT_BIT &                               //
            USART_CONT_RX & USART_BRGH_HIGH, 25);                               //
    OSCCON = 0x60;                                                              //
} 
void putch (char c)                                                             // This routine must always go with the Init_UART()
{                                                                               // 
    while (!TRMT);                                                              //
    TXREG = c;                                                                  //
} 
void Set_ADCON0 (char AN_pin)                                                   // A/D conversion function
{                                                                               //
    ADCON0 = AN_pin * 4 + 1;                                                    //
} 
unsigned int get_full_ADC(void) 
{ 
    int result;
    ADCON0bits.GO=1;                                                            // Start Conversion 
    while(ADCON0bits.DONE==1);                                                  // wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                                         // combine result of upper byte and lower byte into result                                        
    return result;                                                              // return the result. 
}
void Init_ADC(void) 
{ 
    ADCON0=0x01;                                                                // select channel AN0, and turn on the ADC subsystem 
    ADCON1=0x1A ;                                                               // select pins AN0 through AN4 as analog signal, Vref+ and Vref- as reference voltage 
    ADCON2=0xA9;                                                                // right justify the result. Set the bit conversion time (TAD) and acquisition time 
}
void Init_TRIS()                                            
{
    TRISA = 0x0F;                                                               // Set PORTA lower bit as input
    TRISB = 0x00;                                                               // Set PORTB as output
    TRISC = 0x00;                                                               // Set PORTC as output
    TRISD = 0x00;                                                               // Set PORTD as output
    TRISE = 0x00;                                                               // Set PORTE as output
}
void delay_one_sec()                                                            // delay function
{                                                                               //
    for(int i =0; i<17000; i++);                                                //
}
void DisplayLowerDigit(char digit)                                              // Function to display the lower digit
{                                                                               //
    PORTD = LED_7seg[digit];                                                    //
}                                                                               
void DisplayUpperDigit(char digit)                                              // Function to display the upper digit
{                                                                               //
    PORTB = LED_7seg[digit];                                                    //
} 
void main()                                                                     // THIS IS THE START OF MAIN FUNCTION
{                                                                               // 
    Init_UART();                                                                // call function to properly use the serial port and TeraTerm software
    Init_ADC();                                                                 // call function to initialize ADC pins
    Init_TRIS();                                                                // call the function to know which ports are inputs and outputs

    while(1)                                                                    // Infinite loop starts here
    {       
        delay_one_sec(); 
        Set_ADCON0 (0);                                                         // Get the output from potentiometer that is connected to AN0
        int numStep = get_full_ADC();                                           // measure the analog voltage
        float VL = (numStep * 4.0)/1000;                                        // compute the voltage
        int upper = (int)VL;                                                    // only get the upper digit of the voltage
        int lower = (int)(VL*10) % 10;                                          // only get the lower digit of the voltage
        DisplayLowerDigit(lower);                                               // send upper value to the function
        DisplayUpperDigit(upper);                                               // send lower value to the function
        printf("Voltage Input = %f (V) \r\n", VL);                              // print output with tera term
    }
} 
