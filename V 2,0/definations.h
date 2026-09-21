/*
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 15/12/13
*/

#include <p18f8722.h>

#define FLAGS_B1 0b00000001
#define FLAGS_B2 0b00000100
#define FLAGS_TT 0b0001000

#define LEDS PORTD
#define P1 128
#define P2 1

#define PRESSED 0
#define P1BUTTON PORTBbits.RB0
#define P2BUTTON PORTAbits.RA5
#define P1B_FLAG INTCONbits.INT0IF
#define P2B_FLAG INTCON3bits.INT2IF

#define TURN_TIMER_FLAG INTCONbits.TMR0IF
#define TURN_TIMER_ACTV T0CONbits.TMR0ON
