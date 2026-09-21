/*	
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 06/12/13
*/

//#define DEBUG

#include <p18f8722.h>
#include <delays.h>

#include "pragma_settings.h"

#define PRESSED 0
#define P1BUTTON PORTBbits.RB0
#define P2BUTTON PORTAbits.RA5
#define PX_MASK 0b10000001
#define P1_MASK 0b10000000
#define P2_MASK 0b00000001
#define LEDS PORTD

#ifend DEBUG
	unsigned char check_store;
#endif

unsigned char poll_buttons(void);
unsigned char step_ball(unsigned char direction);

void main (void)
{
	//Varibles
	unsigned char step_dir;  	//0 = >> , 1 = <<
	unsigned char winner; 		//Holders the playermask of the winning character
	
	unsigned char loop_upper;	//Looping test varible for when more than 255 repearts are required
	unsigned char loop;	//Common looping test varible
	
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
		switch(poll_buttons())
		{
		case P1_MASK:
			LEDS = P1_MASK;		 //Set the ball to p1 end
			step_dir = 0;			//Set ball travel direction, right
			break;
		case P2_MASK:
			LEDS = P2_MASK;		//Set the ball to p2 end
			step_dir = 1;			//Set ball travel direction, left
			break;
		}
	}
	Delay10KTCYx(250); //1/2 s delay
	
	
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
		
		//Poll for input and while delaying
		loop_upper = 9; loop = 0xC4; //Set loop to , ~= 1/2 second delay
		while(loop_upper && !winner)
		{
			while(loop)
			{
				//Check for input
				switch(poll_buttons())
				{
				case P1_MASK: //P1's button was active
				
					#ifdef DEBUG //Check piont 4
						check_store = LEDS;
						LEDS = (0b00010000);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						LEDS = check_store;
					#endif
						
					if(LEDS == P1_MASK) //P1's time to swing
					{
						step_dir = 0;
						//Delay for reaction time 1/20 s
						Delay10KTCYx(25);
					}
					else //P1 wasn't meant to push
					{
						winner = P2_MASK;
						//Skip the rest of the delay
						loop = 0;
					}
					break;
				case P2_MASK: //P2's button was active
				
					#ifdef DEBUG //Check piont 5
						check_store = LEDS;
						LEDS = (0b00010100);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						LEDS = check_store;
					#endif
					
					if(LEDS == P2_MASK) //P1's time to swing
					{
						step_dir = 1;
						//Delay for reaction time 1/20 s
						Delay10KTCYx(25);
						
					}
					else //P2 wasn't meant to push
					{
						winner = P1_MASK;
						//Skip the rest of the delay
						loop = 0;
					}
					break;
				default:
					
					#ifdef DEBUG //Check piont 6
						check_store = LEDS;
						LEDS = (0b00011000);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						Delay10KTCYx(250);
						LEDS = check_store;
					#endif
					
					break;
				}
				
				//Slight delay
				Delay1KTCYx(1);
				loop--;
			}
			//Set up for next inter loop,
			loop = 255;
			loop_upper--;
		}
		
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
		if(LEDS == P1_MASK && step_dir && !winner)
			{winner = P2_MASK; } //P1 one has missed the ball, so P2 wins
		else if(LEDS == P2_MASK && !step_dir && !winner)
			{ winner = P1_MASK; } //P2 one has missed the ball, so P1 wins
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

unsigned char poll_buttons(void)
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
	
	if(P1BUTTON == PRESSED)
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
		
		return P1_MASK;
	}
	else if(P2BUTTON == PRESSED)
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
		
		return P2_MASK;
	}
	else 
		{ return 0; }
}

unsigned char step_ball(unsigned char direction)
{
	if(direction)
		{ return LEDS << 1; }
	else
		{ return LEDS >> 1; }
}