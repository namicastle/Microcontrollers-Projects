#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>
#include "ST7735_TFT.h"
#include "Interrupt.h"
#include "I2C.h" 
#include "I2C_Support.h" 

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

char tempSecond = 0xff;                                                         // constant declarations
char second = 0x00;                                                             
char minute = 0x00;                                                             
char hour = 0x00;                                                               
char dow = 0x00;                                                                
char day = 0x00;                                                                
char month = 0x00;                                                              
char year = 0x00;                                                               
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;                        
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;                  

#define _XTAL_FREQ  8000000                                                     // Set operation for 8 Mhz
#define KEY    PORTCbits.RC0                                                    // define Key LED

void Initialize_Screen();
void wait_one_sec();

short Nec_OK = 0;
char Nec_Button;
unsigned long long Nec_code;
unsigned char Nec_State = 0;

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define Circle_Size     20                                                      // Size of Circle for Light
#define Circle_X        60                                                      // Location of Circle
#define Circle_Y        80                                                      // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1                                                       // Size of Normal Text
#define TS_2            2                                                       // Size of Big Text


char buffer[31];                                                                // general buffer for display purpose
char *nbr;                                                                      // general pointer used for buffer
char *txt;

char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,        // array from teraterm when the keys from remote are pressed
                0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};				// 

char txt1[21][4] ={"CH-\0"," CH\0" ,"CH+\0","VL-\0","VL+\0","P/P\0",            // array to display the name of the remote keys
" - \0"," + \0"," EQ\0"," 0 \0","100\0","200\0"," 1 \0"," 2 \0",
" 3 \0"," 4 \0"," 5 \0"," 6 \0"," 7 \0"," 8 \0"," 9 \0"};

										
int color[21]={RD,RD,RD,BU,BU,GR,MA,MA,MA,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK,BK};	// add more value into this array

int colord1[21]={0x01,0x01,0x01,0x04,0x04,0x02,0x05,0,0,0,0,0,0,0,0,0,0,0,0,0,0};// array to display D1 LED
int colord2[21]={0,0,0,0,0,0,0,0x28,0x28,0x38,0x38,0x38,0x38,0x38,0,0,0,0,0,0,0};// array to display D2 LED
int colord3[21]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x70,0x70,0x70,0x70,0x070,0x70,0x70};// array to display D3 LED

void init_UART()                                                                // This routine is to initialize the UART                 
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
void main()
{
    init_UART();
    OSCCON = 0x70;                                                              // 8 Mhz
    nRBPU = 0;                                                                  // Enable PORTB internal pull up resistor
	TRISB = 0x07;                                                               // PORTB as input first 3 bits
    TRISC = 0x00;                                                               // PORTC as output
    TRISD = 0x00;                                                               // PORTD as output
    TRISE = 0x00;                                                               // PORTE as output
    ADCON1 = 0x0F;                                                              // set digital
    Initialize_Screen();                                                        // call routine to display on LCD screen
	
    I2C_Init(100000);                                                           // initialize I2C interface
    DS1621_Init();                                                              // call interrupt routine
	Init_Interrupt();                                                           // call interrupt
    
    TMR1H = 0;                                                                  // Reset Timer1
    TMR1L = 0;                                                                  // 
    PIR1bits.TMR1IF = 0;                                                        // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                                        // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                                        // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                                         // Enable global interrupts
    Nec_OK = 0;                                                                 // Clear flag
    Nec_code = 0x0;                                                             // Clear code
    
    while(1)                                                                    // while loop
    {
        DS3231_Read_Time();                                                     // call read time function
        if(tempSecond != second)                                                // to display the time and date and
        {                                                                       // temperature in fahrenheit and celcius
            tempSecond = second;                                                // 
            signed char tempC = DS1621_Read_Temp();                             // readout of temparature
            signed char tempF = (tempC * 9 / 5) + 32;                           //
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
        }      
        
        if (Nec_OK == 1)                                                        // if Nec_OK is 1
        {
            Nec_OK = 0;                                                         // set it back to 0
            printf ("NEC_Button = %x\r\n", Nec_Button);                         // print the following to tera term
            Enable_INT_Interrupt();                                             // call routine
            
            char found = 0xff;                                                  // set found to FF in hex
                
            for(int i = 0; i< 21 ;i++)                                          // search for match of Nec_Button with entry of array1
            {
                if(Nec_Button == array1[i])                                     // search through array
                    found = i;                                                  //
            }
            printf ("FOUND = %d\r\n", found);                                   // print the location on teraterm
            if (found == 0)                                                     // if the first button is pressed
            {
                DS3231_Setup_Time();                                            // then reset to the original time initialized 
                printf ("Key Location = %d \r\n\n", found);                     // print the key location on teraterm
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
				
                PORTE = PORTE & 0;                                              // masked PORTE
                PORTB = PORTB & 0x4;                                            // masked PORTB
                PORTD = PORTD & 0x7;                                            // masked PORTD
                
                PORTE = PORTE + colord1[found];                                 // output the colors with OR
                PORTB = PORTB + colord2[found];     
                PORTD = PORTD + colord3[found];
                              
                KEY = 1;                                                        // turn on KEY PRESSED LEDs
                Activate_Buzzer();                                              // generate beep tone
				wait_one_sec();                                                 // wait 1 second
				Deactivate_Buzzer();                                            // remove beep tone
				KEY = 0;        
            }
            else                                                                // if other buttons are pressed then continue and no reset
            {
				printf ("Key Location = %d \r\n\n", found);                     // print the key location on teraterm
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
				
                PORTE = PORTE & 0;                                              // masked PORTE
                PORTB = PORTB & 0x4;                                            // masked PORTB
                PORTD = PORTD & 0x7;                                            // masked PORTD
                
                PORTE = PORTE + colord1[found];                                 // output the colors with OR
                PORTB = PORTB + colord2[found];     
                PORTD = PORTD + colord3[found];
                              
                KEY = 1;                                                        // turn on KEY PRESSED LEDs
                Activate_Buzzer();                                              // generate beep tone
				wait_one_sec();                                                 // wait 1 second
				Deactivate_Buzzer();                                            // remove beep tone
				KEY = 0;                                                        // turn off KEY PRESSED
            }            
        }    
    }  
}
void wait_one_sec()                                                             // wait one sec routine
{
    for (int k=0;k<30000;k++);
}
void Initialize_Screen()                                                        // initialize screen routine
{   
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Sp23/S2/T06");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 11 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}


