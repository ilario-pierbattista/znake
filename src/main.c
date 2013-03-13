/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Ilario Pierbattista 2011 <ulven101@gmail.com>
 * 
 * znake is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * znake is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define PROGRAM "ZNAKE"
#define PROGRAMMER "Ulven101 <ulven101@gmail.com>"
#define VERSION 1.18

#define CHAR '#'
#define OVER "GAME OVER"
#define PAUSE "PAUSE"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "windows.h"

typedef struct coordinates 
{
	unsigned int l;
	unsigned int c;
} coordinates;

//Variables
static coordinates snake[9000];
static coordinates food;
static int i,len=7,cicle=TRUE, score;

//Functions
coordinates snake_move(struct coordinates snake, int direction);	   		//move the snake
coordinates randomfood(int nlines, int ncols); 		//get food coordinates random
int loopcollision(struct coordinates *snake, int nlines, int ncols, int len);
int avoid_retrofront(int direction, int old_direction);
extern void soviet();

int main () 
{
	int ch,old;
	//initialisation
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK); // green
	init_pair(2, COLOR_RED, COLOR_BLACK);   // red
	init_pair(3, COLOR_WHITE, COLOR_BLACK); // white
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	// infinite loop
	while (1) 
	{
	//Starting windows
	menu();
	start(1);
	//Prepare the snake
	for (i=0; i<len; i++) 
	{ 
		snake[i].l=(LINES/2)+i;
		snake[i].c=(COLS/2);
	}
	refresh();
	food=randomfood(LINES, COLS); // prepare the food
	old=KEY_UP; // set 1st move of the snake
	
	//main cicle
	attron(COLOR_PAIR(1));
	//write the snake
	for (i=0; i<len; i++) 
	{
		mvprintw(snake[i].l,snake[i].c,"%c", CHAR); 
	}
	while (cicle) 
	{
		timeout(170-(15*(score/100))); //timeout settings according on the score
		attron(COLOR_PAIR(3)); 
		box(stdscr, ACS_VLINE, ACS_HLINE);
		// score, length and Local Position System  
		mvprintw(0,4," SCORE: %4d ",score);
		mvprintw(0,20," LENGTH: %3d ",len);
		mvprintw(LINES-1,4," LPS x: %3d y: %3d ",snake[0].c,snake[0].l);
		// 
		refresh();
		ch=getch(); // GET THE KEY PRESSED
		// forward keys 
		if ((ch==KEY_UP)|| 
		    (ch==KEY_DOWN)||
		    (ch==KEY_LEFT)||
		    (ch==KEY_RIGHT)) 
		{
		  	if (avoid_retrofront(ch, old)) 
		  	{
				old=ch; // update precedent move direction with current one
				snake = snake_move(snake, ch); // move snake to current direction
			}
		}
		else if(ch==104) 
		{ //H (help) code
			help(); 
		}
		else if(ch==112)
		{ //P (pause) code
			pause(PAUSE, 3); 
		} 
		else if(ch==109) 
		{ //M (menu) code
			erase();     // clean screen
			cicle=TRUE;  // set the main loop
			score=0;     // clean score
			len=7;       // clean length
			break; 
		} 
		else if(ch==27) 
		{ //ESC (exit) code 
			endwin(); // end ncurses
			exit(0);  // quit the program 
		} 
		else if(ch==ERR) 
		{ // No key pressed
			snake = snake_move(snake, old); // follow the last direction 
		}
		attron(COLOR_PAIR(1));
		for (i=0; i<len; i++) 
		{ // Write the snake
			mvprintw(snake[i].l, snake[i].c, "%c", CHAR);
		}
		mvprintw(snake[len].l, snake[len].c, " "); //Cancel the last piece of the snake
		attron(COLOR_PAIR(2)); 
		mvprintw(food.l, food.c, "%c", CHAR); //print the food
		if ((snake[0].l==food.l)&&
		    (snake[0].c==food.c)) 
		{ //If the snake eats the food
			len+=1;             //Increase length by 1
			score+=10;          //Increase score by 10
			food=randomfood(LINES, COLS); //get new food
		}
		if (!loopcollision(snake, LINES, COLS, len)) 
		{ //if the snake hits himself of walls
			over(OVER);
			cicle=FALSE;
		}
	} //end
	erase();     // clean screen
	refresh();
	cicle=TRUE;  // set main loop on
	score=0;     // clean score
	len=7;       // clean length
	} //END INFINITE LOOP
	endwin();    // end ncurses
	return 0;    // all correct
}

coordinates snake_move(struct coordinates snake, int direction) 
{
	for (i=len; i>0; --i) 
	{ 
		snake[i]=snake[i-1];
	}
	if (direction==KEY_UP) 
	{
		snake[0].l-=1; 
	}
	if (direction==KEY_DOWN) 
	{
		snake[0].l+=1; 
	}
	if (direction==KEY_LEFT) 
	{
		snake[0].c-=1; 
	}
	if (direction==KEY_RIGHT) 
	{
		snake[0].c+=1; 
	}
	return snake;
}

int avoid_retrofront(int new_direction, int old_direction) 
{
	if ( (new_direction==KEY_UP) && (old_direction==KEY_DOWN) ) 
	{
		return FALSE;
	} 
	else if ( (new_direction==KEY_DOWN) && (old_direction==KEY_UP) ) 
	{
		return FALSE;
	} 
	else if ( (new_direction==KEY_RIGHT) && (old_direction==KEY_LEFT) ) 
	{
		return FALSE;
	} 
	else if ( (new_direction==KEY_LEFT) && (old_direction==KEY_RIGHT) ) 
	{
		return FALSE;
	} 
	else 
	{
		return TRUE;
	}
}


/************************************************************
 * RANDOM FOOD
 ************************************************************/
 
coordinates randomfood(int nlines, int ncols) 
{
	srand( (time(NULL)/2) );    //It's not easy to get random number
	food.l=rand()%(nlines-3)+2; //this is a try to increase entropy
	srand( (time(NULL)*2) );    //of the seed
	food.c=rand()%(ncols-3)+2;
	return food;
}


/************************************************************
 * LOOP COLLISION
 ************************************************************/
 
int loopcollision(struct coordinates *snake, int nlines, int ncols, int len) 
{
	int r;
	if ((snake[0].l==0)||
	    (snake[0].l==nlines-1)||
	    (snake[0].c==0)||
	    (snake[0].c==ncols-1)) 
	{ // walls
		r = FALSE;
	}
	for (i=1; i<len; i++) 
	{ //check the snake
		if((snake[0].l==snake[i].l)&&
		   (snake[0].c==snake[i].c)) 
		{
			r = FALSE;
			break;
		} //ENDIF
	} //ENDFOR
	return r;
} //ENDFUNC
