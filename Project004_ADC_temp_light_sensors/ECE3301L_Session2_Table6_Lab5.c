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
#define D1_RED PORTCbits.RC0                                                    // for PORTC bit 0 to 2 (D1)
#define D1_GREEN PORTCbits.RC1                                                  
#define D1_BLUE PORTCbits.RC2 

#define D2_RED PORTCbits.RC3                                                    // for PORTC bit 3 to 5 (D2)
#define D2_GREEN PORTCbits.RC4 
#define D2_BLUE PORTCbits.RC5 

#define D3_RED PORTEbits.RE0                                                    // for PORTE bit 0 & 1 (D3)
#define D3_BLUE PORTEbits.RE1 
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
void Init_TRIS()                                            
{
    TRISA = 0x0F;                                                               // Set PORTA lower bit as input
    TRISB = 0x00;                                                               // Set PORTB as output
    TRISC = 0x00;                                                               // Set PORTC as output
    TRISD = 0x00;                                                               // Set PORTD as output
    TRISE = 0x00;                                                               // Set PORTE as output
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
    ADCON1=0x1B ;                                                               // select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage 
    ADCON2=0xA9;                                                                // right justify the result. Set the bit conversion time (TAD) and acquisition time 
}
void SET_D1_OFF()
{                                                                               // D1 LED OFF function                                                                                    
    D1_RED = 0;                                                                 // PORTC bit 0 = 0   
    D1_GREEN = 0;                                                               // PORTC bit 1 = 0   
    D1_BLUE = 0;                                                                // PORTC bit 2 = 0
}
void SET_D1_RED()                                                               // D1 LED color RED function
{                                                                               //
    D1_RED = 1;                                                                 // PORTC bit 0 = 1   
    D1_GREEN = 0;                                                               // PORTC bit 1 = 0   
    D1_BLUE = 0;                                                                // PORTC bit 2 = 0
}
void SET_D1_GREEN()                                                             // D1 LED color GREEN function
{                                                                               //
    D1_RED = 0;                                                                 // PORTC bit 0 = 0   
    D1_GREEN = 1;                                                               // PORTC bit 1 = 1 
    D1_BLUE = 0;                                                                // PORTC bit 2 = 0
}
void SET_D1_YELLOW()                                                            // D1 LED color YELLOW function
{                                                                               //
    D1_RED = 1;                                                                 // PORTC bit 0 = 1 
    D1_GREEN = 1;                                                               // PORTC bit 1 = 1   
    D1_BLUE = 0;                                                                // PORTC bit 2 = 0
}
void SET_D1_BLUE()                                                              // D1 LED color BLUE function
{                                                                               //
    D1_RED = 0;                                                                 // PORTC bit 0 = 0   
    D1_GREEN = 0;                                                               // PORTC bit 1 = 0   
    D1_BLUE = 1;                                                                // PORTC bit 2 = 1
}
void SET_D1_PURPLE()                                                            // D1 LED color PURPLE function
{                                                                               //
    D1_RED = 1;                                                                 // PORTC bit 0 = 1   
    D1_GREEN = 0;                                                               // PORTC bit 1 = 0   
    D1_BLUE = 1;                                                                // PORTC bit 2 = 1
}
void SET_D1_CYAN()                                                              // D1 LED color CYAN function
{                                                                               //
    D1_RED = 0;                                                                 // PORTC bit 0 = 0   
    D1_GREEN = 1;                                                               // PORTC bit 1 = 1   
    D1_BLUE = 1;                                                                // PORTC bit 2 = 1
}
void SET_D1_WHITE()                                                             // D1 LED color WHITE function
{                                                                               //
    D1_RED = 1;                                                                 // PORTC bit 0 = 1 
    D1_GREEN = 1;                                                               // PORTC bit 1 = 1   
    D1_BLUE = 1;                                                                // PORTC bit 2 = 1
}
void SET_D2_WHITE()                                                             // D2 LED color WHITE function
{                                                                               //
    D2_RED = 1;                                                                 // PORTC bit 3 = 1   
    D2_GREEN = 1;                                                               // PORTC bit 4 = 1   
    D2_BLUE = 1;                                                                // PORTC bit 5 = 1 
}
void SET_D3_OFF()                                                               // D3 LED OFF function
{                                                                               //
    D3_RED = 0;                                                                 // PORTE bit 0 = 0
    D3_BLUE = 0;                                                                // PORTE bit 1 = 0
}
void SET_D3_RED()                                                               // D3 LED color RED function
{                                                                               //
    D3_RED = 1;                                                                 // PORTE bit 0 = 1
    D3_BLUE = 0;                                                                // PORTE bit 1 = 0
}
void SET_D3_BLUE()                                                              // D3 LED color BLUE function
{                                                                               //
    D3_RED = 0;                                                                 // PORTE bit 0 = 0
    D3_BLUE = 1;                                                                // PORTE bit 1 = 1
}
void SET_D3_PURPLE()                                                            // D3 LED color PURPLE function
{                                                                               //
    D3_RED = 1;                                                                 // PORTE bit 0 = 1
    D3_BLUE = 1;                                                                // PORTE bit 1 = 1
}
void displayD1(int temp)                                                        // OUTPUT FUNCTION FOR D1
{                                                                               // 
    if (temp < 40)                                                              // D1 will display different color LED by calling
        SET_D1_OFF();                                                           // the functions above "SET_D1_(COLOR)". 
    if (temp >= 40 && temp <= 49)                                               // the function that will be called depends on
        SET_D1_RED();                                                           // the value of the temperature computed
    if (temp >= 50 && temp <= 59)                                               // from the main function in Fahrenheit. 
        SET_D1_GREEN();                                                         //
    if (temp >= 60 && temp <= 69)                                               //
        SET_D1_BLUE();                                                          //
    if (temp == 70 || temp == 71)                                               //
        SET_D1_PURPLE();                                                        //
    if (temp == 72 || temp == 73)                                               //
        SET_D1_CYAN();                                                          //
    if (temp > 73)                                                              //
        SET_D1_WHITE();                                                         //
}
void displayD2(int digit)                                                       // OUTPUT FUNCTION FOR D2
{                                                                               // We focus on the upper digit of the temperature and compare it to the wanted color output
    if(digit >= 7) digit = 7;                                                   // if the upper digit is more than or equal to 7 then set digit to 7 and shift 0111 to the left 3x  
    PORTC = digit <<3;                                                          // and output the value to PORTC
}
void displayD3(float volt)                                                      // OUTPUT FUNCTION FOR D3
{                                                                               //
    if (volt < 2600)                                                            // D3 will display different color by calling the functions above
        SET_D3_OFF();                                                           // "SET_D3_(COLOR)". The function will be called depending on
    if (volt >= 2600 && volt < 3000)                                            // the value of the volt calculated in mV in main function
        SET_D3_RED();                                                           //
    if (volt >= 3000 && volt < 3300)                                            //
        SET_D3_BLUE();                                                          //
    if (volt >= 3300)                                                           //
        SET_D3_PURPLE();                                                        //
}
void main()                                                                     // THIS IS THE START OF MAIN FUNCTION
{                                                                               // 
    Init_UART();                                                                // call function to properly use the serial port and TeraTerm software
    Init_ADC();                                                                 // call function to initialize ADC pins
    Init_TRIS();                                                                // call the function to know which ports are inputs and outputs

    while(1)                                                                    // Infinite loop starts here
    {                                                                           // 
        Set_ADCON0 (0);                                                         // Get the output from temperature sensor that is connected to AN0
        int numStep = get_full_ADC();                                           // measure the analog voltage
        float voltage = numStep * 4.0;                                          // compute the voltage
        float tempC = (1030.5 - voltage) / (5.50);                              // compute the temperature in Celsius
        float tempF = 1.80* tempC + 32.0;                                       // then convert to Farehnheit
        int tempFar = (int)tempF;                                               // cast int to float
        char upper = tempFar / 10;                                              // store the upper digit
        char lower = tempFar % 10;                                              // store the lower digit 
        int digit = tempFar / 10;                                               // store the upper digit in int
        DisplayLowerDigit(lower);                                               // send upper value to the function
        DisplayUpperDigit(upper);                                               // send lower value to the function

        displayD2(digit);                                                       // send digit value to the function
        displayD1(tempFar);                                                     // send tempFar to the function
        delay_one_sec();                                                        // delay 1 sec

        printf("Steps = %d \r\n", numStep);                                     // the values of numStep, voltage, tempFar prints in TeraTerm
        printf("Voltage = %f (mV) \r\n", voltage);                              //  
        printf("Temperature = %d F \r\n", tempFar);                             //

        Set_ADCON0 (1);                                                         // Get the output connected to AN1
        int numStep = get_full_ADC();                                           // measure the analog voltage
        float voltage1 = numStep * 4.0;                                         // compute the voltage1 of the photo sensor
        printf("Light Voltage = %f (mV) \r\n\n", voltage1);                     // print the following in TeraTerm
        displayD3(voltage1);                                                    // send voltage1 to function
        delay_one_sec();                                                        // delay 1 sec
    }
} 
