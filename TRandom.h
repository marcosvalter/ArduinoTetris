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

#ifndef TRandom_h
#define TRandom_h
/*
 * generates a 8bits random number using the 
 * magnetic noise in a not plugged pin
 * Params:
 * unsigned char minValue        - minimum value
 * unsigned char maxValue       - maximum value
 */
unsigned char tRandom(unsigned char minValue, unsigned char maxValue);
#endif
