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
  this module generates a very poor random number beetween [minValue, maxValue[ 
  using the noise on a non-used arduino's pin. 
 */
#include "TRandom.h"
#include "Arduino.h"

/*
 * generates a 8bits random number using the 
 * magnetic noise in a not plugged pin
 * Params:
 * unsigned char minValue        - minimum value
 * unsigned char maxxValue       - maximum value
 */
unsigned char tRandom(unsigned char minValue, unsigned char maxValue) {
    int value, t1, t2, t3;
    value = analogRead(20);
    t1 = value % 10 + random(0, 9);
    t2 = value % 100 - t1 + random(0, 9);
    t3 = value % 1000 - t1 - t2 + random(0, 9);
    return minValue + ((t1 + t2 / 10 + t3 / 100 + random(0, 9)) % (maxValue - minValue));
}
