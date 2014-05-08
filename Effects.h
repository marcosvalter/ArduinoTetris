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
 * This module make some video animation and effects on an matrix array 16x8
 * like transitions or number rolling
 */

#ifndef Effects_h
#define Effects_h

/*
 * Plays the intro animation
 */
void intro();

/*
 * Blinks a line in a field
 * Params:
 * unsigned char* field     - source field
 * unsigned char line       - line to blink [0,15]
 */
void blinkLine(unsigned char* field, unsigned char line);

/*
 * Blinks a block in a field
 * Params:
 * unsigned char* field     - source field
 * unsigned char line       - line of block to blink [0,15]
 * unsigned char collumn    - collum of block to blink
 */
void blinkBlock(unsigned char* field, unsigned char line, unsigned char collumn);

/*
 * Makes a transition beetween the actual field and 
 * game over field
 * Params:
 * unsigned char* field     - source field
 */
void gameOver(unsigned char* field);

/*
 * Annimation beetween levels
 * Params:
 * unsigned char* field     - source field
 */
void shiftLeftAll(unsigned char *image);

/*
 * Annimation beetween speed changes
 * Params:
 * unsigned char* field     - source field
 */
void pushLeftAll(unsigned char *image);

#endif
