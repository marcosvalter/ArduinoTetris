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
 * this controls the LED counters status
 */
#include "LEDControl.h"

/*
 * sets the level LED status
 * Params:
 * unsigned char *counter       - LED status array
 * unsigned char value          - value to set
 */
void setCounter(unsigned char *counter, unsigned char value) {
    
    counter[0] = value & 0x01;
    counter[1] = (value & 0x02) >> 1;
    counter[2] = (value & 0x04) >> 2;
    counter[3] = (value & 0x08) >> 3;
}

/*
 * sets the next piece LED status
 * Params:
 * unsigned char *piece         - LED status array
 * unsigned char value          - value to set
 */
void setPiece(unsigned char *piece, unsigned char value) {
    piece[0] = value & 0x01;
    piece[1] = (value & 0x02) >> 1;
    piece[2] = (value & 0x04) >> 2;
}
