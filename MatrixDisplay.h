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
 * This module drives a dual MAX7219 Led Matrix Controller
 * Please read MAXIM-7219 Manual for further information
 * and OPCode settings.
 */

#ifndef MatrixDisplay_h
#define MatrixDisplay_h

/*
 * Updates the two displays with a char array[16]
 * Params: 
 * unsigned char *field    - the first element of char array
 */
void updateDisplay(unsigned char *field);

/*
 * Initialises the display (full control)
 * Params:
 * unsigned char bcd          - decode: 0x00=false
 * unsigned char brite        - brightness value [0,15]
 * unsigned char scanLimit    - limits the scan to nº line [0,15]
 * unsigned char onOff        - On/Off the display: 0x00=Off
 */
void initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test);

/*
 * Initialises the display (brightness control)
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void initDisplay(unsigned char brite);

/*
 * Sets the display brightness
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void setDisplayBrite(unsigned char brite);

/*
 * Sets the display BCD
 * Params:
 * unsigned char bcd        - decode: 0x00=false, 0x01=true
 */
void setDisplayBCD(unsigned char bcd);

/*
 * Sets the display pins
 * Params:
 * int CSPin         - number of CS pin
 * int CLKPin        - number of CLK pin
 * int DINPin        - number of DIN pin
 */
void setDisplayPins(int CSPin, int CLKPin, int DINPin);

/*
 * Rolls an image on the display one time
 * Params:
 * unsigned char *image            - the first element of image
 * unsigned char linesPerSecond    - lines per second
 */
void rollImage(unsigned char *image, unsigned int imageSize, unsigned char linesPerSecond, int hold);

/*
 * Rolls a number on the display one time
 * Params:
 * unsigned char prefix            - prefix {p,l}
 * int num                         - number to print
 * unsigned char linesPerSecond    - lines per second
 */
void rollNumber(unsigned char prefix, unsigned long int num, unsigned char linesPerSecond, int hold);

#endif
