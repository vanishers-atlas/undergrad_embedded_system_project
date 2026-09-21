/*	
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 15/12/13
*/

#include <p18f8722.h>
#include <delays.h>

#include "pragma_settings.h"
#include "PIC18LCD.h"
#include "definations.h"
#include "V2,0 functions.h"

#ifdef DEBUG
	unsigned char check_store_main;
#endif

void main(void)
{
	unsigned char startingplayer;
	unsigned char rounds; //Tracks the number of rounds remaining in a tormatent
	unsigned char leftscore  = '0'; //Tranks P1'a score
	unsigned char rightscore = '0'; //Tranks P1'a score
	unsigned char temp; //Varible for passing data between functions
	char score_header[] = {'B','E','S','T',' ','O','F',' ','*','\n'};
	char score_values[] = {'P','1',' ',':',' ','*',' ',' ','P','2',' ',':',' ','*','\n'};
	
	//Setup
	chip_setup();
	
	//Get the game mode
	switch(mode_menu())
	{
	case 1: //PVP mode
		rounds = games_menu(); //Get the number of rounds
		startingplayer = player_select(); //Get the starting player
		
		//Display header for scores
		LCDClear();
		score_header[8] = rounds + 48; //The 48 coverts the number of rounds to the ascii vaalue of it's figure
		LCDLine_1();
		d_write_line(score_header);
		
		for(; rounds; rounds--)
		{
			//Prapare and display scores
			score_values[5]  = leftscore;
			score_values[13] = rightscore;
			LCDLine_2();
			d_write_line(score_values);
			
			//Setup game and play
			LEDS = startingplayer;
			wait_for_press();
			temp = PVP_game(startingplayer != P1);
			
			//Update the winning points score
			if(temp == P1)
				{ leftscore++; }
			else
				{ rightscore++; }
		}
		display_winner(leftscore, rightscore, 0);
	break;
	case 2: //Endurance mode
		//Setup game and play
		LEDS = P2;
		temp = SKILLMODE();
		
		//Display score
		display_score(temp);
	break;
	}	
}