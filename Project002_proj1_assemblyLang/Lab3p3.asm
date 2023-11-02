; THIS THIRD ASSEMBLY LANGUAGE PROGRAM WILL IMPLEMENT PART 2 OF LAB2
    
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
 MOVWF TRISC		; Make TRISC output
 
MAIN_LOOP:		; Start of main loop (infinite loop)
 MOVF PORTA, W		; Read content of PORTA into W
 ANDLW 0x07		; AND the contents from PORTA with 0x0F then result is kept in WREG
 MOVWF PORTC		; Initialized PORTC with the result from ANDLW instruction
 GOTO MAIN_LOOP		; go back to the beginning of main loop
 END


