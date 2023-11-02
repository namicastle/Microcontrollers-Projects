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
    ADCON1=0x1B ;                                                               // select pins AN0 through AN4 as analog signal, Vref+ and Vref- as reference voltage 
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
void Activate_Buzzer()                                                          // function to activate buzzer
{                                                                               //
    PR2 = 0b11111001;                                                           //
    T2CON = 0b00000101;                                                         //
    CCPR2L = 0b01001010;                                                        //
    CCP2CON = 0b00111100;                                                       //
}
void Deactivate_Buzzer()                                                        // function to deactivate buzzer
{                                                                               //
    CCP2CON = 0x0;                                                              //
    PORTCbits.RC1 = 0;                                                          //
} 
void DisplayLowerDigit(char digit)                                              // Function to display the lower digit
{                                                                               //
    PORTD = LED_7seg[digit];                                                    //
}
void DisplayUpperDigit(char digit)                                              // Function to display the upper digit
{                                                                               //
    PORTB = LED_7seg[digit];                                                    //
} 
void SET_DP_ON()                                                                // DP ON function
{                                                                               //
    DP = 0;                                                                     // DP = 0   
}
void SET_DP_OFF()                                                               // DP OFF function
{                                                                               //
    DP = 1;                                                                     // DP = 1   
}
void SET_D1_OFF()                                                               // D1 LED color OFF function
{                                                                               //
    D1_RED = 0;                                                                 // PORTE bit 0 = 0   
    D1_GREEN = 0;                                                               // PORTE bit 2 = 0   
    Deactivate_Buzzer();                                                        // call function to deactivate buzzer
}
void SET_D1_RED()                                                               // D1 LED color RED function
{                                                                               //
    D1_RED = 1;                                                                 // PORTE bit 0 = 1   
    D1_GREEN = 0;                                                               // PORTE bit 2 = 0   
    Deactivate_Buzzer();                                                        // call function to deactivate buzzer
}
void SET_D1_GREEN()                                                             // D1 LED color GREEN function
{                                                                               //
    D1_RED = 0;                                                                 // PORTE bit 0 = 0   
    D1_GREEN = 1;                                                               // PORTE bit 2 = 1 
    Activate_Buzzer();                                                          // call function to activate buzzer
}
void SET_D2_OFF()
{                                                                               // D2 LED OFF function                                                                                    
    D2_RED = 0;                                                                 // PORTC bit 2 = 0   
    D2_GREEN = 0;                                                               // PORTC bit 3 = 0   
    D2_BLUE = 0;                                                                // PORTC bit 4 = 0
}
void SET_D2_RED()                                                               // D2 LED color RED function
{                                                                               //
    D2_RED = 1;                                                                 // PORTC bit 2 = 1   
    D2_GREEN = 0;                                                               // PORTC bit 3 = 0   
    D2_BLUE = 0;                                                                // PORTC bit 4 = 0
}
void SET_D2_GREEN()                                                             // D1 LED color GREEN function
{                                                                               //
    D2_RED = 0;                                                                 // PORTC bit 2 = 0   
    D2_GREEN = 1;                                                               // PORTC bit 3 = 1 
    D2_BLUE = 0;                                                                // PORTC bit 4 = 0
}
void SET_D2_YELLOW()                                                            // D1 LED color YELLOW function
{                                                                               //
    D2_RED = 1;                                                                 // PORTC bit 2 = 1 
    D2_GREEN = 1;                                                               // PORTC bit 3 = 1   
    D2_BLUE = 0;                                                                // PORTC bit 4 = 0
}
void SET_D2_BLUE()                                                              // D1 LED color BLUE function
{                                                                               //
    D2_RED = 0;                                                                 // PORTC bit 2 = 0   
    D2_GREEN = 0;                                                               // PORTC bit 3 = 0   
    D2_BLUE = 1;                                                                // PORTC bit 4 = 1
}
void SET_D2_PURPLE()                                                            // D1 LED color PURPLE function
{                                                                               //
    D2_RED = 1;                                                                 // PORTC bit 2 = 1   
    D2_GREEN = 0;                                                               // PORTC bit 3 = 0   
    D2_BLUE = 1;                                                                // PORTC bit 4 = 1
}
void SET_D2_CYAN()                                                              // D1 LED color CYAN function
{                                                                               //
    D2_RED = 0;                                                                 // PORTC bit 2 = 0   
    D2_GREEN = 1;                                                               // PORTC bit 3 = 1   
    D2_BLUE = 1;                                                                // PORTC bit 4 = 1
}
void SET_D2_WHITE()                                                             // D1 LED color WHITE function
{                                                                               //
    D2_RED = 1;                                                                 // PORTC bit 2 = 1 
    D2_GREEN = 1;                                                               // PORTC bit 3 = 1   
    D2_BLUE = 1;                                                                // PORTC bit 4 = 1
}
void DisplayD1(float volt)
{                                                                               //
   if(volt < .07)                                                               // volt < 70ohm or .07kOhm = GREEN light and activate buzzer
    SET_D1_GREEN();                                                             // 
   if(volt >= 0.07 && volt <= 0.1)                                              // volt between 0.07 & 0.1kohm = RED light and deactivate buzzer
    SET_D1_RED();                                                               //
   if(volt > 0.1)                                                               // volt > 0.1kohm = OFF light and deactivate buzzer
    SET_D1_OFF();                                                               //
}
void DisplayD2(float RL)                                                        // Display D2 function
{                                                                               //
    int upper = ((int)RL)/10;                                                   // only get the upper digit of the value of resistor from main
    if(upper >= 7) upper = 7;                                                   // if the upper digit is more than or equal to 7 then set digit to 7 and shift 0111 to the left 2x  
        PORTC = upper << 2;                                                     // so it is in the proper range bit for PORTC
}
void lower10k(float RL)                                                         // function if resistor is less than 10kohm
{                                                                               //
    int upper = (int)RL;                                                        // down cast RL and store only the upper digit of RL (converting the value of RL to int instead of float)
    int lower = (int)((RL-upper)*10);                                           // down cast RL and calculate to store only the tenth place of RL (converting the value of RL to int instead of float) 
    DisplayLowerDigit(lower);                                                   // send lower value to the function display
    DisplayUpperDigit(upper);                                                   // send upper value to the function display
    DP = 0;                                                                     // turn on DP
}
void upper10k(float RL)                                                         // function if resistor is more then 10kohm
{                                                                               //
    int upper = ((int)RL)/10;                                                   // down cast RL and calculate to store only the upper digit of RL
    int lower = ((int)RL)%10;                                                   // down cast RL and calculate to store only the lower digit of RL
    DisplayLowerDigit(lower);                                                   // send lower value to the function display
    DisplayUpperDigit(upper);                                                   // send upper value to the function display
    DP = 0;                                                                     // turn off DP
}
void check(float res)                                                           // function to check if resistor is more or less than 10kohm
{                                                                               //
    if (res < 10.0)                                                             // if less than 10kohm then go to lower10k function
        lower10k(res);                                                          //
    if (res >= 10.0)                                                            // if more than 10kohm then go to upper10k function
        upper10k(res);
}
void main()                                                                     // THIS IS THE START OF MAIN FUNCTION
{                                                                               // 
    Init_UART();                                                                // call function to properly use the serial port and TeraTerm software
    Init_ADC();                                                                 // call function to initialize ADC pins
    Init_TRIS();                                                                // call the function to know which ports are inputs and outputs

    while(1)                                                                    // Infinite loop starts here
    {       
        delay_one_sec();                                                        // wait 1 sec
        Set_ADCON0 (2);                                                         // Get the output connected to AN2
        int numStep = get_full_ADC();                                           // measure the analog voltage
        float VL = ((numStep * 4.0)/1000.0);                                    // calculate the voltage (V)
        float RL = ((1 * VL)/(4.096 - VL));                                     // calculate the resistor RL = Rref * VL/ (Vref - VL)
        
        check(RL);                                                              // call the functions
        DisplayD2(RL);                                                          //
        DisplayD1(RL);                                                          //
        
        printf("Voltage = %f (V) \r\n\n", VL);                                  // print the following on tera term
        printf("RL2 = %f (KOhm) \r\n\n", RL);                                   //
        delay_one_sec();                                                        // wait 1 sec
    }
} 
