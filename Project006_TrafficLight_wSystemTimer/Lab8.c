#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <string.h>

#include "ST7735_TFT.h"
#include "main.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

unsigned int get_full_ADC(void);
void Init_ADC(void);
void Init_TRIS(void);
void init_UART(void);
void putch (char);
void Set_ADCON0(char ch);

void Set_EW(char color);
void Set_EW_LT(char color);
void Set_NS(char color);
void Set_NS_LT(char color);


void PED_Control( char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();

void Wait_One_Second();
void Wait_Half_Second();
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep();

void update_LCD_color(char , char );
void update_LCD_PED_Count(char direction, char count);
void Initialize_Screen();
void update_LCD_misc();
void update_LCD_count(char, char);
void Get_Light_Readout();

#define _XTAL_FREQ  8000000                                                     // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4                                                // Timer Clock 2 Mhz

//colors
#define OFF             0                                                       // Defines OFF as decimal value 0
#define RED             1                                                       // Defines RED as decimal value 1
#define GREEN           2                                                       // Defines GREEN as decimal value 2
#define YELLOW          3                                                       // Defines YELLOW as decimal value 3

#define NS              0                                                       // Number definition of North/South
#define NSLT            1                                                       // Number definition of North/South Left Turn
#define EW              2                                                       // Number definition of East/West
#define EWLT            3                                                       // Number definition of East/West Left Turn

#define Circle_Size     7                                                       // Size of Circle for Light
#define Circle_Offset   15                                                      // Location of Circle
#define TS_1            1                                                       // Size of Normal Text
#define TS_2            2                                                       // Size of PED Text
#define Count_Offset    10                                                      // Location of Count

#define XTXT            30                                                      // X location of Title Text 
#define XRED            40                                                      // X location of Red Circle
#define XYEL            60                                                      // X location of Yellow Circle
#define XGRN            80                                                      // X location of Green Circle
#define XCNT            100                                                     // X location of Sec Count

#define Color_Off       0                                                       // Number definition of Off Color
#define Color_Red       1                                                       // Number definition of Red Color
#define Color_Green     2                                                       // Number definition of Green Color
#define Color_Yellow    3                                                       // Number definition of Yellow Color

#define NS_Txt_Y        20
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      50
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define EW_Txt_Y        80
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      110
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define PED_NS_Count_Y  30
#define PED_EW_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140

#define PED_Count_NS    8
#define PED_Count_EW    9


char buffer[31];                                                                // general buffer for display purpose
char *nbr;                                                                      // general pointer used for buffer
char *txt;

char NS_Count[]     = "00";                                                     // text storage for NS Count
char NSLT_Count[]   = "00";                                                     // text storage for NS Left Turn Count
char EW_Count[]     = "00";                                                     // text storage for EW Count
char EWLT_Count[]   = "00";                                                     // text storage for EW Left Turn Count

char PED_NS_Count[] = "00";                                                     // text storage for NS Pedestrian Count
char PED_EW_Count[] = "00";                                                     // text storage for EW Pedestrian Count

char SW_NSPED_Txt[] = "0";                                                      // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";                                                      // text storage for NS Left Turn Switch
char SW_EWPED_Txt[] = "0";                                                      // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";                                                      // text storage for EW Left Turn Switch
char SW_MODE_Txt[]  = "D";                                                      // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";                                                     // text storage for Actual Mode
char FlashingS_Txt[] = "0";                                                     // text storage for Flashing Status
char FlashingR_Txt[] = "0";                                                     // text storage for Flashing Request

char dir;
char Count;                                                                     // RAM variable for Second Count
char PED_Count;                                                                 // RAM variable for Second Pedestrian Count

char SW_NSPED;                                                                  // RAM variable for NS Pedestrian Switch
char SW_NSLT;                                                                   // RAM variable for NS Left Turn Switch
char SW_EWPED;                                                                  // RAM variable for EW Pedestrian Switch
char SW_EWLT;                                                                   // RAM variable for EW Left Turn Switch
char SW_MODE;                                                                   // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volt;
int nStep;


void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}

void Set_ADCON0 (char ch)
{
    ADCON0 = ch * 4 + 1;
}

void Initialize_Screen()
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L SD23-S2-T06");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* EMERGENCY REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* EMERGENCY STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
      
  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
      
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "NSP NSLT EWP EWLT MR");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void main(void)
{
    Init_ADC();
    init_UART();
    Init_TRIS();
    OSCCON = 0x70;                                                              // set the system clock to be 1MHz 1/4 of the 4MHz
    RBPU = 0;
    Initialize_Screen();                                                        // Initialize the TFT screen

    Set_ADCON0 (0);
         
    Get_Light_Readout();
    SW_MODE = volt < 3.5 ? 1:0;                                                 // Mode = 1, Day_mode, Mode = 0 Night_mode 
printf ("Day Mode \r\n");
    while(1)                                                                    // forever loop
    {
        if (SW_MODE)    
        {
            Day_Mode();                                                         // calls Day_Mode() function
        }
        else
        {
            Night_Mode();                                                       // calls Night_Mode() function
        } 
    } 
}

void Init_ADC()
{
    ADCON0 = 0x01;
    ADCON1= 0x0E; 
    ADCON2= 0xA9;
}

void Init_TRIS()
{
    TRISA = 0x1F;                                                               // inputs RA0/AN0-RA4 inputs,RA5 output
    TRISB = 0x07;                                                               // outputs
    TRISC = 0x00;                                                               // set PORTC as outputs
    TRISD = 0x00;                                                               // set PORTD as outputs
    TRISE = 0x00;                                                               // set PORTE as outputs
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                                                             // Start Conversion
   while(ADCON0bits.DONE==1);                                                   // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                                          // combine result of upper byte and
                                                                                // lower byte into result
   return result;                                                               // return the result.
}

void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;                                   // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;                                   // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;                                 // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;                                // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    direction = NSLT;
    update_LCD_color(direction, color);
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;                               // Turns off NS_LT LED
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;                               // Sets NS_LT LED RED
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;                             // Sets NS_LT LED GREEN
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;                            // Sets NS_LT LED YELLOW
    }
}

void Set_EW(char color)
{
    direction = EW;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;                                   // Turns off EW LED
        case RED: EW_RED =1;EW_GREEN=0;break;                                   // Sets EW LED RED
        case GREEN: EW_RED =0;EW_GREEN=1;break;                                 // Sets EW LED GREEN
        case YELLOW: EW_RED =1;EW_GREEN=1;break;                                // Sets EW LED YELLOW
    }
}

void Set_EWLT(char color)
{
    direction = EWLT;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;                               // Turns off EW_LT LED
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;                               // Sets EW_LT LED RED
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;                             // Sets EW_LT LED GREEN
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;                            // Sets EW_LT LED YELLOW
    }
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}

void PED_Control( char direction, char Num_Sec)
{ 
    for(char Sec = Num_Sec-1;Sec>0; Sec--)
    {
        update_LCD_PED_Count(direction, Sec);
        Wait_One_Second_With_Beep();                                            // hold the number on 7-Segment for 1 second
    }
    
    update_LCD_PED_Count(direction, 0);                                         //
    Wait_One_Second_With_Beep();                                                // leaves the 7-Segment off for 1 second
}
void Mode_Intro()                                                               // Night_Mode() and Day_Mode()
{                                                                               // both have the same beginning so
    Set_NS(1);                                                                  // I created a function instead of typing 4 lines again
    Set_NSLT(1);                                                                // 
    Set_EWLT(1);                                                                //
    Set_EW(2);                                                                  //
}
void Day_Mode()
{
    printf ("Day Mode1 \r\n");
    MODE = 1;                                                                   // turns on the MODE_LED
    MODE_LED = 1;
    Act_Mode_Txt[0] = 'D';
   
     Mode_Intro();                                                              // call function
                                                                                
    if(EWPED_SW == 1)                                                           // if switch for EWPD is 0 then skip if not then 
    {                                                                           // Pedestrian crossing in East-West direction
        PED_Control(1,7);                                                       // for 7 seconds
    }
    
    Wait_N_Seconds(7);                                                          // wait 7 seconds
    Set_EW(3);                                                                  // set EW LED to YELLOW
    Wait_N_Seconds(2);                                                          // for 2 seconds
    Set_EW(1);                                                                  // set EW LED back to RED
    
    if(NSLT_SW == 1)                                                            // if switch for NSLT is 0 then skip if not
    {                                                                           // then 
        Set_NSLT(2);                                                            // set NSLT LED to green
        Wait_N_Seconds(7);                                                      // for 7 seconds
        Set_NSLT(3);                                                            // set NSLT to YELLOW
        Wait_N_Seconds(2);                                                      // for 2 seconds
        Set_NSLT(1);                                                            // set NSLT back to RED
    }
    
    Set_NS(2);                                                                  // set NS LED to GREEN
    
    if(NSPED_SW == 1)                                                           // if switch for NSPED is 0 then skip if not then
    {                                                                           // pedestrian crossing in North-South direction
        PED_Control(0,7);                                                       // for 7 seconds
    }                                                                           
    
    Wait_N_Seconds(6);                                                          // wait 6 seconds
    Set_NS(3);                                                                  // set NS to YELLOW
    Wait_N_Seconds(2);                                                          // for 2 seconds
    Set_NS(1);                                                                  // set NS to RED
    
    if(EWLT_SW == 1)                                                            // if switch for EWLT is 0 then skip if not then
    {                                                                           //
        Set_EWLT(2);                                                            // set EWLT to GREEN
        Wait_N_Seconds(9);                                                      // for 9 seconds
        Set_EWLT(3);                                                            // set EWLT
        Wait_N_Seconds(2);                                                      // for 2 seconds
        Set_EWLT(1);                                                            // set EWLT back to RED
    }
}
void Night_Mode()
{ 
    printf ("Night Mode \r\n");
    MODE = 0;                                                                   // turns off the MODE_LED
    MODE_LED = 0;
    Act_Mode_Txt[0] = 'N';
    while(1);
            
    Mode_Intro();                                                               // call the function
    Wait_N_Seconds(8);                                                          // wait 8 seconds
    Set_EW(3);                                                                  // set EW LED to YELLOW
    Wait_N_Seconds(2);                                                          // for 2 seconds
    Set_EW(1);                                                                  // set EW LED back to RED
    
    if (NSLT_SW == 1)                                                           // If the switch for NSLT is 1 or 'on' 
    {                                                                           // if not then skip inside if statement
        Set_NSLT(2);                                                            // set NSLT LED to GREEN
        Wait_N_Seconds(6);                                                      // for 6 seconds
        Set_NSLT(3);                                                            // set NSLT LED to YELLOW
        Wait_N_Seconds(2);                                                      // for 2 seconds
        Set_NSLT(1);                                                            // set NSLT LED back to RED
    }                                                                           
    
    Set_NS(2);                                                                  // set NS LED to GREEN
    Wait_N_Seconds(8);                                                          // for 8 seconds
    Set_NS(3);                                                                  // set NS LED to YELLOW
    Wait_N_Seconds(2);                                                          // for 2 seconds
    Set_NS(1);                                                                  // set NS LED back to RED
    
    if(EWLT_SW == 1)                                                            // If switch for EWLT is 0 or 'off' then skip inside if statements 
    {                                                                           // if switch is on then continue 
        Set_EWLT(2);                                                            // set EWLT LED to GREEN
        Wait_N_Seconds(8);                                                      // for 8 seconds
        Set_EWLT(3);                                                            // set EWLT to YELLOW
        Wait_N_Seconds(2);                                                      // for 2 seconds
        Set_EWLT(1);                                                            // set EWLT back to RED
    }
}

void Wait_One_Second()                                                          // creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_One_Second_With_Beep()                                                // creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Activate_Buzzer();
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Deactivate_Buzzer();
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                                                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                               // set the lower byte of TMR
    TMR0H = 0x0B;                                                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                       // turn off the Timer 0
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        update_LCD_count(direction, I);  
        Wait_One_Second();                                                      // calls Wait_One_Second for x number of times
      
    }
    update_LCD_count(direction, 0);   
}

void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = NS_Cir_Y + direction * 30;    
    
    if (color == Color_Off)                                                     // if Color off make all circles black but leave outline
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)                                                     // if the color is red only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    if (color == Color_Yellow)                                                  // if the color is yellow only fill the yellow circle with yellow
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                
    }
          
    if (color == Color_Green)                                                   // if the color is green only fill the green circle with green
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                      
    }
         
}

void update_LCD_count(char direction, char count)
{
   switch (direction)                                                           // update traffic light no ped time
   {
      case NS:       
        NS_Count[0] = count/10  + '0';
        NS_Count[1] = count%10  + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2); 
        break;
      
      case NSLT:      
        NSLT_Count[0] = count/10  + '0';
        NSLT_Count[1] = count%10  + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);          
        break;
      
      case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
            
      case EWLT:       
        EWLT_Count[0] = count/10  + '0';
        EWLT_Count[1] = count%10  + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);        
        break;
    }  
}

void update_LCD_PED_Count(char direction, char count)
{

   switch (direction)
   {
      case NS:       
        PED_NS_Count[0] = count/10  + '0';                                      // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';                                      // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
                                                                                // Put counter digit on screen
        break;
        
      case EW:       
        PED_EW_Count[0] = count/10  + '0';                                      // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';                                      // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;
   }
   
}

void Get_Light_Readout()
{
    nStep = get_full_ADC();                                                     // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                   // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
}

void update_LCD_misc()
{
    Get_Light_Readout();
    SW_MODE = volt < 3.5 ? 1:0;                                                 // Mode = 1, Day_mode, Mode = 0 Night_mode  
    SW_NSPED = NSPED_SW;
    SW_NSLT = NSLT_SW;
    SW_EWPED = EWPED_SW;
    SW_EWLT = EWLT_SW;
      
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';       // Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';    
   
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);// Show switch and sensor states at bottom of the screen
    drawtext(32,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(87,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);            
}




