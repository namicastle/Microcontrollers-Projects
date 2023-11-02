;  THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF 
; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM 
; TO THE PINS 0 THROUGH 3 OF PORT B 
    
#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 
 ORG 0x0000 
 
START:			; Code starts here 
 MOVLW 0x0F		; Load 0x0F to W
 MOVWF ADCON1		; Make ADCON1 digital with value above
 MOVLW 0xFF		; Load 0xFF to W
 MOVWF TRISA		; Make TRISA inputs
 MOVLW 0x00		; Load 0x00 to W
 MOVWF TRISB		; Make TRISB output
 
MAIN_LOOP:		; Main loop starts here
 MOVF PORTA, W		; Read content of PORTA into W
 ANDLW 0x0F		; AND the contents from PORTA with 0x0F then result is kept in WREG
 MOVWF PORTB		; Initialized PORTB with the result from ANDLW instruction
 GOTO MAIN_LOOP		; go back to main loop for the infinite loop
 END


