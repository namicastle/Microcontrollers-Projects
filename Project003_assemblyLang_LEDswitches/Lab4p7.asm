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
 MOVLW 0x07		    ;
 MOVWF TRISD		    ; TRISD = 0x07 
 MOVLW 0x00		    ; 
 MOVWF TRISC		    ; TRISC = 0x00 
 MOVWF TRISE		    ; TRISE = 0x00 

MAIN_LOOP:		    ; MAIN_LOOP STARTS FROM HERE
BTFSC PORTD, 2		    ; If bit 2 of PORTD is 0 skip next line otherwise go to next line
GOTO PORTD2EQ1		    ;
GOTO PORTD2EQ0		    ;
 
PORTD2EQ1:		    ; PORTD2EQ1 Starts from here
GOTO TASK_BCD		    ; Go to TASK_BCD
 
PORTD2EQ0:		    ; PORTD2EQ0 starts from here
BTFSC PORTD, 1		    ; If bit 1 of PORTD is 0 skip next line otherwise go to next line 
GOTO PORTD21EQ01	    ;
GOTO PORTD21EQ00	    ;
 
PORTD21EQ01:		    ; PORTD21EQ01 starts from here
BTFSC PORTD, 0		    ; If bit 0 of PORTD is 0 skip next line otherwise go to next line
GOTO TASK_XOR		    ;
GOTO TASK_AND		    ;
    
PORTD21EQ00:		    ; PORTD21EQ00 starts from here
BTFSC PORTD, 0		    ; If bit 0 of PORTD is 0 skip next line otherwise go to next line
GOTO TASK_ADD		    ;	
GOTO TASK_COMP		    ;

TASK_COMP: 
BCF PORTD, 7		    ; This is to clear the Blue LED of the RGB 
BCF PORTD, 6		    ; This is to clear the Green LED of the RGB 
BCF PORTD, 5		    ; This is to clear the RED LED of the RGB 
CALL SUB_COMP 
GOTO MAIN_LOOP 
	
TASK_ADD:		    ; ADD task starts here
 BCF PORTD, 7		    ; Clear BLUE LED of RBG
 BCF PORTD, 6		    ; Clear GREEN LED of RBG
 BSF PORTD, 5		    ; Set RED LED of RBG to 1
 CALL SUB_ADD		    ; call the function to perfom ADD function
 GOTO MAIN_LOOP
 
 TASK_AND:		    ; AND task starts here
 BCF PORTD, 7		    ; Clear BLUE LED of RBG
 BSF PORTD, 6		    ; Set GREEN LED of RGB to 1
 BCF PORTD, 5		    ; Clear RED LED of RBG
 CALL SUB_AND		    ; Call the function to perfrom AND function
 GOTO MAIN_LOOP
 
 TASK_XOR:		    ; XOR task starts here
 BCF PORTD, 7		    ; Clear BLUE LED of RBG
 BSF PORTD, 6		    ; Set GREEN LED of RBG to 1
 BSF PORTD, 5		    ; Set RED LED of RBG to 1
 CALL SUB_XOR		    ; Call the function to perfrom XOR function
 GOTO MAIN_LOOP
 
 TASK_BCD:		    ; BCD task starts here
 BSF PORTD, 7		    ; Set BLUE LED of RBG to 1
 BCF PORTD, 6		    ; Clear BLUE LED of RBG
 BCF PORTD, 5		    ; Clear BLUE LED of RBG
 CALL SUB_BCD		    ; Call the function to perfrom BCD function
 GOTO MAIN_LOOP

SUB_COMP: 
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
COMF InA,W		    ; Complement contents from W then store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF Result		    ; Move contents into Result from W
MOVFF Result,PORTC	    ; Move contents into PORTC from Result				
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE0		    ; If Z = 1 call TEST_DONE otherwise execute code below
GOTO DONE0		    ; goto to DONE0 to end COMP function
TEST_DONE0:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
DONE0:			    
RETURN			    ; Finishes the COMP function
    
SUB_ADD: 
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
MOVF PORTB, W		    ; Read contents of PORTB, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InB		    ; Move contents into InB from W
ADDWF InA,W		    ; Add InA to components of W
MOVWF Result		    ; Move contents into Result from W
MOVFF Result,PORTC	    ; Move contents into PORTC from Result			    ;
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE1		    ; If Z = 1 call TEST_DONE otherwise execute code below		    
GOTO DONE1		    ; goto to DONE0 to end ADD function
TEST_DONE1:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
DONE1:
RETURN			    ; Finishes the ADD function
   
SUB_AND: 
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
MOVF PORTB, W		    ; Read contents of PORTB, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InB		    ; Move contents into InB from W
ANDWF InA,W		    ; Add InA to components of W
MOVWF Result		    ; Move contents into Result from W
MOVFF Result,PORTC	    ; Move contents into PORTC from Result			    ;
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE2		    ; If Z = 1 call TEST_DONE otherwise execute code below		    
GOTO DONE2		    ; goto to DONE0 to help end AND function
TEST_DONE2:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
DONE2:
RETURN			    ; Finishes the AND function
    
SUB_XOR: 
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
MOVF PORTB, W		    ; Read contents of PORTB, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InB		    ; Move contents into InB from W
XORWF InA,W		    ; Add InA to components of W
MOVWF Result		    ; Move contents into Result from W
MOVFF Result,PORTC	    ; Move contents into PORTC from Result			    ;
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE3		    ; If Z = 1 call TEST_DONE otherwise execute code below		    
GOTO DONE3		    ; goto to DONE3 to help end AND function
TEST_DONE3:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
DONE3:
RETURN			    ; Finishes the XOR function
    
SUB_BCD: 
MOVF PORTA, W		    ; Read contents of PORTA, store in W
ANDLW 0x0F		    ; Mask out 4 upper bits
MOVWF InA		    ; Move contents into InA from W
MOVLW 0x09		    ; Move contents to W
CPFSGT InA,1		    ; If 0x09 >= Ina skip next line otherwise go to the next time
GOTO LESS		    ; Go if if 9 < InA
GOTO GREATER		    ; Go if 9 >= InA
    
GREATER:		    ; GREATER loop starts from here
MOVLW 0x06		    ; Move contents 0x06 to W
ADDWF InA		    ; Add W with InA
MOVFF InA, Result	    ; Move contents into Result from InA
MOVFF Result, PORTC	    ; Move contents into PORTC from Result
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE4		    ; Branch to TEST_DONE4
GOTO SUB_BCD  

LESS:			    ; LESS loop starts from here
MOVFF InA, Result	    ; Move contents into Result from InA
MOVFF Result,PORTC	    ; Move contents into PORTC from Result
BCF PORTE,0		    ; Clear contents of PORTE 
BZ TEST_DONE4		    ; Branch to TEST_DONE4
GOTO SUB_BCD
    
TEST_DONE4:		    ; Start of TEST_DONE
BSF PORTE,0		    ; Set PORTE bit 0 to 1
GOTO DONE4		    ; goto to DONE4 to help end BCD function
DONE4:
RETURN			    ; Finishes the BCD function

END