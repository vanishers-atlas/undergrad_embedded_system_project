/*	
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 13/12/13
*/

#include <p18f8722.h>
#include <delays.h>

#include "pragma_settings.h"

#define FLAGS_B1 0b00000001
#define FLAGS_B2 0b00000100
#define FLAGS_TT 0b0001000

#define LEDS PORTD
#define P1_END 128
#define P2_END 1

#define PRESSED 0
#define P1BUTTON PORTBbits.RB0
#define P2BUTTON PORTAbits.RA5
#define P1B_FLAG INTCONbits.INT0IF
#define P2B_FLAG INTCON3bits.INT2IF

#define TURN_TIMER_FLAG INTCONbits.TMR0IF
#define TURN_TIMER_ACTV T0CONbits.TMR0ON

//#define DEBUG

//Funtions declarations
unsigned char step_ball(unsigned char direction);

void My_ISR(void);

//Global coustum flags varible
//0: P1B has been pressed 1: P1B is in cooldown
//2: P2B has been pressed 3: P2B is in cooldown
//4: The turn timer has tripped
unsigned char flags;

#ifdef DEBUG
	unsigned char check_store;
#endif


void main (void)
{
	//Varibles
	unsigned char step_dir;  	//0 = >> , 1 = <<
	unsigned char winner; 		//Holders the playermask of the winning character
	
	unsigned char loop;			//Common looping test varible
	
	//Setup buttons
	ADCON1bits.PCFG0 = 1;
	ADCON1bits.PCFG1 = 1;
	ADCON1bits.PCFG2 = 1;
	ADCON1bits.PCFG3 = 1;
	TRISAbits.RA5 = 1;
	TRISBbits.RB0 = 1;
	
	//Setup ouputs
	TRISD = 0;
	LEDS  = 0;
	
	// Set up Timers
	T0CON = 0b00000101;	//Timer0, 16 bit, 1:62 prscalar
	TMR0H = 0x67; TMR0L = 0x69;	//Set timer0 to 1/2 second

	//Clear flags
	flags = 0;
	TURN_TIMER_FLAG = 0;
	P1B_FLAG = 0;
	P2B_FLAG = 0;
	
	//Set up interrupts
	INTCON2bits.INTEDG0 = 0; 	//Set P1Button's interupt to be negative edge triggered
	INTCON2bits.INTEDG1 = 0; 	//Set P2Button's interupt to be negative edge triggered
	INTCONbits.TMR0IE  = 1;		//Enable Timer0's interrupt, (turn timer)
	INTCONbits.INT0IE  = 1;		//Enable P1button's interrupt
	INTCON3bits.INT2IE = 1;		//Enable P2button's interrupt
	INTCONbits.GIE = 1; 		//Enable all interrupts globally
	
	
	#ifdef DEBUG  //Check piont 1
		check_store = LEDS;
		LEDS = (0b00000100);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		LEDS = check_store;
	#endif
	
	//Wait for a player to start game
	while(!LEDS)
	{
		if(flags & FLAGS_B1) //Button one was pressed
		{
			LEDS = P1_END;	//Set the ball to p1 end
			step_dir = 0;	//Set ball travel direction, right
		}
		else if(flags & FLAGS_B2)
		{
			LEDS = P2_END;	//Set the ball to p2 end
			step_dir = 1;	//Set ball travel direction, left
		}
	}
	Delay10KTCYx(250); 	//1/2 s delay
	flags = 0; 			//clear flags
	
	
	#ifdef DEBUG  //Check piont 2
		check_store = LEDS;
		LEDS = (0b00001000);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		LEDS = check_store;
	#endif

	//Game Loop
	winner = 0;	
	while(!winner)
	{
		//Update desplay
		LEDS = step_ball(step_dir);
		
		//Check piont 3
		#ifdef DEBUG
			check_store = LEDS;
			LEDS = (0b00001100);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			LEDS = check_store;
		#endif
		
		//Turn loop
		TURN_TIMER_ACTV = 1; //Turn on the turn timer
		while(!(flags & FLAGS_TT)) //While turn flag not active
		{
			//Check for input
			if(flags & FLAGS_B1) //B1 was pressed
			{
				#ifdef DEBUG //Check piont 4
					check_store = LEDS;
					LEDS = (0b00010000);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					LEDS = check_store;
				#endif
				
				if(LEDS == P1_END) //P2's time to swing
					{ step_dir = 0; }
				else //P2 wasn't meant to push
					{ winner = P2_END; }
				flags ^= FLAGS_B1; 	//Clear the B1 flag
			}
			else if(flags & FLAGS_B2) //B2 was pressed
			{
				#ifdef DEBUG //Check piont 5
					check_store = LEDS;
					LEDS = (0b00010100);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					LEDS = check_store;
				#endif
				
				if(LEDS == P2_END) //P2's time to swing
					{ step_dir = 1; }
				else //P2 wasn't meant to push
					{ winner = P1_END; }
				flags ^= FLAGS_B2; 	//Clear the B2 flag
			}
			else
			{
				#ifdef DEBUG //Check piont 6
					check_store = LEDS;
					LEDS = (0b00011000);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					LEDS = check_store;
				#endif
			}
		}
		flags ^= FLAGS_TT; 	//Clear the turn end flag
		
		#ifdef DEBUG //Check piont 7
			check_store = LEDS;
			LEDS = (0b00011100);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			LEDS = check_store;
		#endif
		
		//Check for if the ball is about to leave the screen
		if(LEDS == P1_END && step_dir && !winner)
			{
				#ifdef DEBUG //Check piont 8
					check_store = LEDS;
					LEDS = (0b00100000);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					LEDS = check_store;
				#endif
				winner = P2_END; 
			} //P1 one has missed the ball, so P2 wins
		else if(LEDS == P2_END && !step_dir && !winner)
			{
				#ifdef DEBUG //Check piont 9
					check_store = LEDS;
					LEDS = (0b00100100);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					Delay10KTCYx(250);
					LEDS = check_store;
				#endif
				
				winner = P1_END; 
			} //P2 one has missed the ball, so P1 wins
	}
	
	#ifdef DEBUG //Check piont A
		check_store = LEDS;
		LEDS = (0b00101000);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		LEDS = check_store;
	#endif
					
	//Display winner
	for(loop = 5; loop; loop--)
	{
		LEDS = winner;
		Delay10KTCYx(250); //1/2 s delay
		LEDS = 0;
		Delay10KTCYx(250); //1/2 s delay
	}
}

//Function definations
#pragma code My_HighPriority_Interrupt=0x08
void My_HighPriority_Interrupt(void)
{
	_asm
		GOTO My_ISR
	_endasm
}

#pragma interrupt My_ISR
void My_ISR(void)
{
	#ifdef DEBUG //Check piont D
		check_store = LEDS;
		LEDS = (0b00110100);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		Delay10KTCYx(250);
		LEDS = check_store;
	#endif
	
	//Check button 1
	if(P1B_FLAG )// && !(flags & COOL1_FLAG))
	{
		#ifdef DEBUG //Check piont D
			check_store = LEDS;
			LEDS = (0b00110100);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			LEDS = check_store;
		#endif
		
		flags |= 0b00000011; //Set both the B1 pressed and cooldown flags
		//P1_COOLDOWN_ACTV = 1; //Start the C1 timer
		Delay10KTCYx(50); //temp, remove once B1 cooldown timer is working
		P1B_FLAG = 0;
	}
	//Check button 2
	else if(P2B_FLAG )// && !(flags & COOL2_FLAG))
	{
		#ifdef DEBUG //Check piont E
			check_store = LEDS;
			LEDS = (0b00111000);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			LEDS = check_store;
		#endif
		
		flags |= 0b00001100; //Set both the B2 pressed and cooldown flags
		//COOLDOWNP2_ACTV = 1; //Start the C2 timer
		Delay10KTCYx(50);//temp, remove once B2 cooldown timer is working
		P2B_FLAG = 0;
	}
	//Check timer0  (the turn timer)
	else if(TURN_TIMER_FLAG)
	{
		#ifdef DEBUG //Check piont F
			check_store = LEDS;
			LEDS = (0b00111100);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			Delay10KTCYx(250);
			LEDS = check_store;
		#endif
		flags |= FLAGS_TT; //Set the turn flags high
		TURN_TIMER_ACTV = 0; //Turn off the turn timer
		TURN_TIMER_FLAG = 0; //Clear the turn timer flag
	}
}

unsigned char step_ball(unsigned char direction)
{
	if(direction)
		{ return LEDS << 1; }
	else
		{ return LEDS >> 1; }
}