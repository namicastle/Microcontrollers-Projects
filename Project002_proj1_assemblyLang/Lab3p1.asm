; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS 
; CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B 
    
#include<P18F4620.inc> 
config OSC = INTIO67 
config WDT = OFF 
config LVP = OFF 
config BOREN = OFF 

Delay1 equ 0xFF		    ; Constant declarations 
Delay2 equ 0XFF		    ;
Counter_L equ 0x20	    ;
Counter_H equ 0x21	    ;
ORG 0x0000		    ;
  
START:			    ; CODE STARTS FROM HERE
 MOVLW 0x0F		    ; Load W with 0xF0 or move 0x0F into WREG
 MOVWF ADCON1		    ;Initialize ADCON1 with 0xF0 which makes is digital
 MOVLW 0x00		    ; Load W with 0x00
 MOVWF TRISB		    ; Initialize TRISB with 0x00 which makes TRISB output
 
MAIN_LOOP:		    ; Start of main loop. Note that this is an infinite loop
MOVLW 0x05		    ; Load W with 0x05
MOVWF PORTB		    ; Initialize PORTB with 0x05 or output PORTB with 0x05
; 
CALL DELAY_ONE_SEC	    ; wait for 1 sec
MOVLW 0x0A		    ; Load W with 0x0A
MOVWF PORTB		    ; Output PORTB with 0x0A
CALL DELAY_ONE_SEC	    ; wait for 1 sec
GOTO MAIN_LOOP		    ; Go back to the start of main loop
    
DELAY_ONE_SEC:		    
MOVLW Delay1 ; 
MOVWF Counter_H		    ; Delay1 = Counter_H
    
LOOP_OUTER: 
NOP			    ; no operation just to delay time
MOVLW Delay2 ; 
MOVWF Counter_L		    ; Delay2 = Counter_H
    
LOOP_INNER: 
NOP			    ; no operation just to delay time
DECF Counter_L,F	    ; decrement Counter_L
BNZ LOOP_INNER		    ; branch to Loop_Inner if not equal to 0
DECF Counter_H,F	    ; decrement Counter_H
BNZ LOOP_OUTER		    ; branch to Loop_Outer if not equal to 0
RETURN 
 END





