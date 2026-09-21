/*	
AUTHOR: Stephen Clarke (S# 40083147)
LANGUAGE: Embedded C for the pic18f722
DATE STARTED: 15/12/13
*/

//Funtions declarations
//Set up functions
void chip_setup(void);	//Sets the PIC18F722's ports up from the rest of the program

//Game mode functions
unsigned char mode_menu(void); 		//Displays the mode selection menu and returns a values based on the selection
unsigned char games_menu(void);		//Displaye the number of games manu and reutrns the selected value
unsigned char PVP_game (unsigned char startdir);		//The basic two player version of the game.
unsigned char SKILLMODE(void);

//Sub game functions
void My_ISR(void); 		//Handles the button and timer interupts, flagged them in flags
void wait_for_press(void);
unsigned char player_select(void); 	//Returns which button was pressed first
unsigned char step_ball(unsigned char direction);	//Updates the LED display according the unsigned char that was passed to it
void display_winner(unsigned char left, unsigned char right, unsigned char AI_side);	//Displayes the winner
void display_score(unsigned char score);	//Displayes the winnerr

