#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF
;
Delay1 equ 0xFF			; Constant Declerations
Delay2 equ 0xFF			;
Counter_L equ 0x20		;
Counter_H equ 0x21		;
Color_Value equ 0x28		;
Loop_Count equ 0x29		;
 ORG 0x0000 

START:				; CODE STARTS FROM HERE 
 ORG 0x0000 
START:				; Start of the code
 MOVLW 0x0F			; Load W with 0x0F
 MOVWF ADCON1			; Make ADCON1 digital
 MOVLW 0x00			; Load W with 0x00
 MOVWF TRISC			; Make TRISC output
 
WHILE_LOOP:			; Start of while loop
 MOVLW 0x00			; Load W with 0x00
 MOVWF Color_Value		; Initialize Color_Value with 0x00 (min for the array)
 MOVLW 0x08			; Load W with 0x08
 MOVWF Loop_Count		; Initialize Loop_Count with 0x08 (max for the array)
 
FOR_LOOP:			; Start of for loop
 MOVF Color_Value,W		; Read content of Color_Value into W
 MOVWF PORTC			; Initialize PORTC with W from above
 CALL DELAY_ONE_SEC		; wait 1 sec
 INCF Color_Value,F		; Increments the contents of Color_Value by 1 and result is stored in data reg
 DECF Loop_Count,F		; Decrements the contents of Loop_Count by 1 and result is stored in data reg
 BNZ FOR_LOOP			; branch to loop if its not equal to either 0 or 8
 ; 
 GOTO WHILE_LOOP		; Go back to while loop
 
DELAY_ONE_SEC:			; Start of delay one sec
MOVLW Delay1 ; 
MOVWF Counter_H			; Delay1 = Counter_H
	
LOOP_OUTER: 
NOP				; no operation just to delay time
MOVLW Delay2 ; 
MOVWF Counter_L			; Delay2 = Counter_H
    
LOOP_INNER: 
NOP				; no operation just to delay time
DECF Counter_L,F		; decrement Counter_L
BNZ LOOP_INNER			; branch to Loop_Inner if not equal to 0
DECF Counter_H,F		; decrement Counter_H
BNZ LOOP_OUTER			; branch to Loop_Outer if not equal to 0
RETURN 
 END