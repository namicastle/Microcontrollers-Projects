; THE FIFTH ASSEMBLY GENERATE LAB 2 PART 5
 #include <P18F4620.inc> 
 config OSC = INTIO67 
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF
 
Delay1 equ 0xFF		    ; Constant declaration
Delay2 equ 0xFF		    ;
Counter_L equ 0x20	    ;
Counter_H equ 0x21	    ;
Color_Value equ 0x28	    ;
Loop_Count equ 0x29	    ;
startValue equ 0x50	    ;Array values {0x85,0xA7,0xE3,0x06,0x21,0x62,0xC4,0x40};
 
 
 ORG 0x0000 
START:			    ; CODE STARTS FROM HERE
 ORG 0x0000 
START: 
 MOVLW 0x0F		    ; Load W with 0x0F
 MOVWF ADCON1		    ; Make ADCON1 digital
 MOVLW 0x00		    ; Load W with 0x00
 MOVWF TRISC		    ; Makes TRISC output
 MOVWF TRISD		    ; Makes TRISD output
 
 MOVLW 0x85		    ; Load W with Ox85 which is the first value of the array from lab2part5
 MOVWF startValue	    ; Load it to startValue location which is 0x50
 MOVLW 0xA7		    ; Load W with second value of the array 
 MOVWF startValue +1	    ; Load it at 0x51 location
 MOVLW 0xE3		    ; Load W with 3rd value of the array
 MOVWF startValue +2	    ; Load it at 0x52 location
 MOVLW 0x06		    ; Load W with the 4th value of the array
 MOVWF startValue +3	    ; Load it at 0x53 location
 MOVLW 0x21		    ; Load W with the 5th value of the array
 MOVWF startValue +4	    ; Load it at 0x54 location
 MOVLW 0x62		    ; Load W with the 6th value of the array
 MOVWF startValue +5	    ; Load it at 0x55 location
 MOVLW 0xC4		    ; Load W with the 7th value of the array
 MOVWF startValue +6	    ; Load it at 0x56 location
 MOVLW 0x40		    ; Load W with the 8th value of the array
 MOVWF startValue +7	    ; Load it at 0x57 location
 

WHILE_LOOP: 
 LFSR 0,startValue;
 
 MOVLW 0x00		    ; Move 0x00 into W
 MOVWF Color_Value	    ; Initizialize Color_Value with 0x00
 
 MOVLW 0x08		    ; Move 0x08 into W
 MOVWF Loop_Count	    ; Initialize Loop_Count with 0x08
 
FOR_LOOP: 
 MOVF Color_Value, W	    ; Read content of Color_Value into W
 MOVWF PORTC		    ; Initialize PORTC with Color_Value content which is now in W
 MOVF INDF0,W		    ; Move value addressed by FSR0 into W
 MOVWF PORTD		    ; Initialize PORTC with W
 
 CALL DELAY_ONE_SEC	    ; Wait 1 sec
 INCF FSR0L		    ; Increments the contents of FSR0L
 INCF Color_Value,F	    ; Increment by 1, store result in F
 DECF Loop_Count,F	    ; Decrement by 1, store result in F
 BNZ FOR_LOOP		    ; brach to loop if values are not 0 or 8
 ; 
 GOTO WHILE_LOOP	    ; go back to while loop for the infinite loop
 
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