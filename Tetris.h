/*
 * This module is part of a small programming project just for fun
 * You may freely use and modify this code, NO WARRANTY, blah blah blah,
 * as long as you give proper credit to the original and subsequent authors.
 *
 * Date:        Author:
 * 2014-04-09   Marcos Valter Vidal Russo Alegre <m.valter@ua.pt>
 *
 */

/*
 * This module plays a Tetris Game clone based on the original arcade game and
 * adapted for a 8x16 Led Matrix display. the public field is an array[16] of chars
 * each one represents one line of the array.
 * You can start/restart and play the game and get the actual field to display and 
 * get the timeEnd for game speed purpose.
 */

#ifndef Tetris_h
#define Tetris_h

/*
 * Initialises the game(intro and startGame)
 */
void initGame();

/*
 * Just starts a new game
 */
void startGame();

/*
 * Rotates the actual piece CCW with collision detection
 */
void rotate();

/*
 * Moves the piece to rigth with collision detection
 */
void goRigth();

/*
 * Moves the piece to left with collision detection
 */
void goLeft();

/*
 * Moves the piece down with collision detection
 */
void goDown();

/*
 * Gets the actual game field
 * Returns:
 * unsigned char array with the current game field
 */
unsigned char *getField();

/*
 * Gets time end flag for speed control
 * Returns:
 * unsigned char for time end flag.
 */
unsigned char getTimeEnd();

/*
 * returns if game is paused
 * Returns:     - unsigned char for game is paused
 */
unsigned char isGamePaused();

/*
 * returns the actual level
 * Returns:     - unsigned char for level
 */
unsigned char getLevel();

/*
 * returns the actual score
 * Returns:     - unsigned long int for score
 */
unsigned int getScore();

/*
 * returns the next piece num
 * Returns:     - unsigned char for next piece
 */
unsigned char getNextPiece();

#endif
