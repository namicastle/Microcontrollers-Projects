#define _XTAL_FREQ      8000000

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02


#define FANEN_LED       PORTAbits.RA4
#define FAN_EN          PORTAbits.RA5
#define FAN_PWM         PORTCbits.RC2
#define RTC_ALARM_NOT   PORTBbits.RB0

#define Setup_Time_Key        	12                                              // number 1 button on remote
#define Setup_Alarm_Key       	13                                              // number 2 button on remote
#define Setup_Fan_Temp_Key    	14                                              // number 3 button on remote
#define Toggle_Fan_Key      	5                                               // play/pause button on remote
#define Prev            		3                                               // << button on remote
#define Next            		4                                               // >>  button on remote
#define Minus           		6                                               //  - button on remote
#define Plus            		7                                               // +  button on remote
#define Ch_Plus            		2                                               // CH+  button on remote
#define Ch_Minus				0                                               // CH-  button on remote



