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
 * unsigned char value          - value to set [0,15]
 */
void setCounter(unsigned char *counter, unsigned char value) {

    // assert the line value
    if (value < 0x00 || value > 0x0f) {
        Serial.print("Error: Param= ");
        Serial.print(value, DEC);
        Serial.println(" value is out the range [0,15]");
        Serial.println("in setCounter(unsigned char *counter, unsigned char value)");
        return;
    }

    counter[0] = value & 0x01;
    counter[1] = (value & 0x02) >> 1;
    counter[2] = (value & 0x04) >> 2;
    counter[3] = (value & 0x08) >> 3;
}

/*
 * sets the next piece LED status
 * Params:
 * unsigned char *piece         - LED status array
 * unsigned char value          - value to set[0,7]
 */
void setPiece(unsigned char *piece, unsigned char value) {

    // assert the line value
    if (value < 0x00 || value > 0x07) {
        Serial.print("Error: Param= ");
        Serial.print(value, DEC);
        Serial.println(" value is out the range [0,7]");
        Serial.println("in setPiece(unsigned char *piece, unsigned char value)");
        return;
    }

    piece[0] = value & 0x01;
    piece[1] = (value & 0x02) >> 1;
    piece[2] = (value & 0x04) >> 2;
}
