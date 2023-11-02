#include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 
 
InA equ 0x20		    ; Constant declaration
InB equ 0x21		    ;
Result equ 0x22		    ;
ORG 0x0000 
 
START:			    ; CODE STARTS FROM HERE 
 MOVLW 0x0F		    ; 
 MOVWF ADCON1		    ; ADCON1 = 0x0F 
 MOVLW 0xFF		    ; 
 MOVWF TRISA		    ; TRISA = 0xFF 
 MOVWF TRISB		    ; TRISB = 0xFF
 MOVLW 0x07
 MOVWF TRISD		    ; TRISD = 0x07 
 MOVLW 0x00		    ; 
 MOVWF TRISC		    ; TRISC = 0x00 
 MOVWF TRISE		    ; TRISE = 0x00 
 
MAIN_LOOP:		    ; Start of main loop. Note that this is an infinite loop
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
MOVLW 0x09		    ; Move contents to W
CPFSGT InA,1		    ; If 0x09 >= Ina skip next line otherwise go to the next time
GOTO LESS		    ; Go if if 9 < InA
GOTO GREATER		    ; Go if 9 >= InA
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE		    ; Branch to TEST_DONE
	
GREATER:		    ; GREATER loop starts from here
MOVLW 0x06		    ; Move contents 0x06 to W
ADDWF InA		    ; Add W with InA
MOVFF InA, Result	    ; Move contents into Result from InA
MOVFF Result, PORTC	    ; Move contents into PORTC from Result

LESS:			    ; LESS loop starts from here
MOVFF InA, Result	    ; Move contents into Result from InA
MOVFF Result,PORTC	    ; Move contents into PORTC from Result			   
				    	
TEST_DONE:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
GOTO MAIN_LOOP    
    
END			    ; CODE ENDS HERE
    