/*	
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 15/12/13
*/

#include <p18f8722.h>
#include <delays.h>

#include "definations.h"
#include "V2,0 functions.h"
#include "PIC18LCD.h"

//Global coustum flags varible
//0: P1B has been pressed 1: P1B is in cooldown
//2: P2B has been pressed 3: P2B is in cooldown
//4: The turn timer has tripped
unsigned char flags;

//Function definations
//Setup functinons
#pragma code My_HighPriority_Interrupt=0x08
void My_HighPriority_Interrupt(void)
{
	_asm
		GOTO My_ISR
	_endasm
}

void chip_setup(void)
{
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
	T0CON = 0b00000100;	//Timer0, 16 bit, 1:32 prscalar
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
	
	//Setup the LCD
	LCDInit();
	LCDClear();
}

//Game modes functions
unsigned char mode_menu(void)
{
	unsigned char selection = 0;
	unsigned char wait; //Delay varible
	char clearline[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\n'};
	char modeselect[] = {'S','E','L','E','C','T',' ','A',' ','M','O','D','E','\n'};
	char PVP[]  = {'T','W','O',' ','P','L','A','Y','E','R','\n'};
	char PVAI[] = {'O','N','E',' ','P','L','A','Y','E','R','\n'};
	char ENDR[] = {'E','N','D','U','R','A','N','C','E','\n'};
	
	//Select mode menu
	LCDLine_1();
	d_write_line(modeselect);
	
	//Mode manu
	while(!selection) //Enables cycling through the menu more than once
	{
		//Clear of the last option
		LCDLine_2();
		d_write_line(clearline);
			
		//Draw the new function.
		LCDLine_2();
		d_write_line(PVP);
		
		//Wait for input
		wait = 1;
		while(wait && !selection)
		{
			if(flags & FLAGS_B1) //select button has been pressed
			{
				selection = 0x01; 	//Sets the game mode and will skip the rest of the mode menu
				flags ^= FLAGS_B1; 	//Clear the B1 flag
			}
			else if(flags & FLAGS_B2) //NExt button has been pressed
			{
				wait = 0; 			//Jump to next part of menu
				flags ^= FLAGS_B2; 	//Clear the B1 flag
			}
		}
		
		//Display the SKILL mode option
		if(!selection) //Enables skipping of the muno nodes
		{
			//Clear of the last option
			LCDLine_2();
			d_write_line(clearline);
			
			//Draw the new function.
			LCDLine_2();
			d_write_line(ENDR);
			
			//Wait for input
			wait = 1;
			while(wait && !selection)
			{
				if(flags & FLAGS_B1) //select button has been pressed
				{
					selection = 0x02; 	//Sets the game mode and will skip the rest of the mode menu
					flags ^= FLAGS_B1; 	//Clear the B1 flag
				}
				else if(flags & FLAGS_B2) //NExt button has been pressed
				{
					wait = 0; 			//Jump to next part of menu
					flags ^= FLAGS_B2; 	//Clear the B1 flag
				}
			}
		}
		
	}
	
	return selection;
}

unsigned char games_menu(void)
{

	//LOOK UP THE CODES FOR ASAII FIGURES AS AND ADD A DEFINATION
	char roundselect[] = {'H','O','W',' ','M','A','N','Y',' ','G','A','M','E','S','?','\n'};
	char ROUNDS[] = {' ','1',' ',' ','3',' ',' ','5',' ',' ','7',' ',' ','9',' ','\n'};
	unsigned char wait;
	unsigned char selection = 0;
	
	//Number of games selection
	LCDClear();
	LCDLine_1();
	d_write_line(roundselect);
	
	while(1)
	{
		//Clear the old star away
		if(selection)
		{
			ROUNDS[(selection-1)*3] = ' ';
			ROUNDS[(selection-1)*3 + 2] = ' ';
		}
		else
		{
			ROUNDS[12] = ' ';
			ROUNDS[14] = ' ';
		}
		
		//Add the stars to the options menu
		ROUNDS[(selection)*3] = '*';
		ROUNDS[(selection)*3 + 2] = '*'; 
		
		//Output
		LCDLine_2();
		d_write_line(ROUNDS);
		
		//Get input
		wait = 1;
		while(wait)
		{
			if(flags & FLAGS_B1) //select button has been pressed
			{
				flags ^= FLAGS_B1; 		 //Clear the B1 flag
				return ((2*selection) + 1); //Sets the game mode and will skip the rest of the mode menu
			}
			else if(flags & FLAGS_B2) //Next button has been pressed
			{
				if(selection == 4) //Loop the selection back  first option
					{ selection = 0; }
				else //Move the selection to the next value
					{ selection++; }
				
				wait = 0; 			//Jump to next part of menu
				flags ^= FLAGS_B2; 	//Clear the B1 flag
			}
		}
	}	
}

unsigned char PVP_game (unsigned char startdir)
{
	unsigned char step_dir = startdir;			//0 = >> , 1 = <<
	unsigned char winner; 			//Loop condistional and return value

	//Game Loop
	winner = 0;	
	while(!winner)
	{
		//Update desplay
		LEDS = step_ball(step_dir);
		
		//Turn loop
		TURN_TIMER_ACTV = 1; //Turn on the turn timer
		while(!(flags & FLAGS_TT)) //While turn flag not active
		{
			//Check for input
			if(flags & FLAGS_B1) //B1 was pressed
			{				
				if(LEDS == P1) //P1's time to swing
					{ step_dir = 0; }
				else //P1 wasn't meant to push
					{ winner = P2; }
				flags ^= FLAGS_B1; 	//Clear the B1 flag
			}
			else if(flags & FLAGS_B2) //B2 was pressed
			{
				if(LEDS == P2) //P2's time to swing
					{ step_dir = 1; }
				else //P2 wasn't meant to push
					{ winner = P1; }
				flags ^= FLAGS_B2; 	//Clear the B2 flag
			}
		}
		flags ^= FLAGS_TT; 	//Clear the turn end flag
		
		
		//Check for if the ball is about to leave the screen
		if(LEDS == P1 && step_dir && !winner)
			{ winner = P2; } //P1 one has missed the ball, so P2 wins
		else if(LEDS == P2 && !step_dir && !winner)
			{ winner = P1; } //P2 one has missed the ball, so P1 wins
	}
	
	return winner;
}

unsigned char SKILLMODE(void)
{
	unsigned char step_dir = 1;	//0 = >> , 1 = <<
	unsigned char score = 0;
	char line1[] = {'E','N','D','U','R','A','N','C','E',' ','M','O','O','D','\n'};
	char line2[] = {'S','C','O','R','E',' ',':',' ','*','*','*','\n'};
		
	//Prepare LCDscreen for score
	LCDClear();
	LCDLine_1();
	d_write_line(line1);
	
	//Prepare the score line and draw to screen
	line2[8]  = ((score - score%100)/100) + 48; //100s figure
	line2[9]  = ((score - score%10)/10) + 48; //10s figure
	line2[10] = (score%10) + 48; //1s figure
	LCDLine_2();
	d_write_line(line2);
	
	wait_for_press();
	
	while(1)
	{
		//Prepare the score line and draw to screen
		line2[8]  = ((score - score%100)/100) + 48; //100s figure
		line2[9]  = ((score - score%10)/10) + 48; //10s figure
		line2[10] = (score%10) + 48; //1s figure
		LCDLine_2();
		d_write_line(line2);
		
		//Update desplay
		LEDS = step_ball(step_dir);
		
		//Turn loop
		TURN_TIMER_ACTV = 1; //Turn on the turn timer
		while(!(flags & FLAGS_TT)) //While turn flag not active
		{
			if(flags & FLAGS_B2)
			{
				if(LEDS == P2) //Player is meant to hit the ball
				{
					step_dir = 1; 		//toggle the step_dir
					score++;
					flags ^= FLAGS_B2; 	//Clear the button flags
				}
				else //Wasn't meant to press
					{ return score; }
			}
			
		}
		flags ^= FLAGS_TT; 	//Clear the turn end flag
		
		//Check for the if the ball is at either end
		if(LEDS == P2 && !step_dir) //Player has passed
			{ return score; }
		else if(LEDS == P1)
			{ step_dir = 0; }
	}
}

//Sub game functions
#pragma interrupt My_ISR
void My_ISR(void)
{
	//Check button 1
	if(P1B_FLAG )// && !(flags & COOL1_FLAG))
	{
		flags |= 0b00000011; //Set both the B1 pressed and cooldown flags
		//P1_COOLDOWN_ACTV = 1; //Start the C1 timer
		Delay10KTCYx(50); //temp, remove once B1 cooldown timer is working
		P1B_FLAG = 0;
	}
	//Check button 2
	else if(P2B_FLAG )// && !(flags & COOL2_FLAG))
	{
		flags |= 0b00001100; //Set both the B2 pressed and cooldown flags
		//COOLDOWNP2_ACTV = 1; //Start the C2 timer
		Delay10KTCYx(50);//temp, remove once B2 cooldown timer is working
		P2B_FLAG = 0;
	}
	//Check timer0  (the turn timer)
	else if(TURN_TIMER_FLAG)
	{
		flags |= FLAGS_TT; //Set the turn flags high
		TURN_TIMER_ACTV = 0; //Turn off the turn timer
		TURN_TIMER_FLAG = 0; //Clear the turn timer flag
	}
}

void wait_for_press(void)
{
	while(!(flags & 0b00000101));
	flags = 0; //Clear button flags
}

unsigned char player_select(void)
{
	char temp1[] = {'S','T','A','R','T','I','N','G',' ','P','L','A','Y','E','R','\n'};
	char temp2[] = {'P','L','E','A','S','E',' ','P','R','E','S','S','\n'};
	
	//Dissplay message on LCD screen
	LCDClear();
	LCDLine_1();
	d_write_line(temp1);
	LCDLine_2();
	d_write_line(temp2);
	
	//Wait for a player to start game
	while(1)
	{
		if(flags & FLAGS_B1) //Button one was pressed
		{
			flags = 0;	//clear flags
			return P1;	//Will be used to set both the starting LED and traval direction
		}
		else if(flags & FLAGS_B2)
		{
			flags = 0;	//clear flags
			return P2;	//Will be used to set both the starting LED and traval direction
		}
	}
}

unsigned char step_ball(unsigned char direction)
{
	if(direction)
		{ return LEDS << 1; }
	else
		{ return LEDS >> 1; }
}

void display_winner(unsigned char left, unsigned char right, unsigned char AI_side)
{				
	unsigned char loop;
	char AIwon[]  = {'M','I','C','R','O','C','H','I','P',' ','W','O','N','\n'};
	char PLAYER[] = {'P','L','A','Y','E','R',' ','*',' ','W','O','N','\n'};
	
	LCDClear();
	
	//Display winner
	if(left > right) //Left side won
	{
		//Draw to screen
		if(AI_side == P1) //Ai side won
		{
			LCDLine_1();
			d_write_line(AIwon);
		}
		else //Player 1 won
		{
			PLAYER[7] = '1'; //Append the stared char in the -player won string
			LCDLine_1();
			d_write_line(PLAYER);
		}
		
		//Flask LEDS
		for(loop = 5; loop; loop--)
		{
			LEDS = P1;
			Delay10KTCYx(250); //1/2 s delay
			LEDS = 0;
			Delay10KTCYx(250); //1/2 s delay
		}
	}
	else //Other player won
	{
		//Draw to screen
		if(AI_side == P1) //Ai side won
		{
			LCDLine_1();
			d_write_line(AIwon);
		}
		else //Player 2 won
		{
			PLAYER[7] = '2'; //Append the stared char in the player won string
			LCDLine_1();
			d_write_line(PLAYER);
		}
		
		//Flask LEDS
		for(loop = 5; loop; loop--)
		{
			LEDS = P2;
			Delay10KTCYx(250); //1/2 s delay
			LEDS = 0;
			Delay10KTCYx(250); //1/2 s delay
		}
	}
}

void display_score(unsigned char score)
{
	unsigned char loop;
	char line1[] = {'E','N','D','U','R','A','N','C','E',' ','O','V','E','R','\n'};
	char line2[] = {'S','C','O','R','E',' ',':',' ','*','*','*','\n'};
	
	LCDClear();
	
	//Draw to screen
	LCDLine_1();
	d_write_line(line1);
	
	//Prepare the score line and draw to screen
	line2[8]  = ((score - score%100)/100) + 48; //100s figure
	line2[9]  = ((score - score%10)/10) + 48; //10s figure
	line2[10] = (score%10) + 48; //1s figure
	LCDLine_2();
	d_write_line(line2);
	
	//Flask LEDS
	for(loop = 5; loop; loop--)
	{
		LEDS = score;
		Delay10KTCYx(250); //1/2 s delay
		LEDS = 0;
		Delay10KTCYx(250); //1/2 s delay
	}
}