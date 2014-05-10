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

#include "MatrixDisplay.h"
#include "Arduino.h"

// MAX7219 pins
int CSPin;      // CS pin
int CLKPin;     // CLK pin
int DINPin;     // DIN pin

/*
 * digits representation
 */
unsigned char numbers[10][5] = {
    {0x3c, 0x24, 0x24, 0x24, 0x3c}, // 0
    {0x18, 0x08, 0x08, 0x08, 0x1c}, // 1
    {0x3c, 0x04, 0x3c, 0x20, 0x3c}, // 2
    {0x3c, 0x04, 0x1c, 0x04, 0x3c}, // 3
    {0x24, 0x24, 0x3c, 0x04, 0x04}, // 4
    {0x3c, 0x20, 0x3c, 0x04, 0x3c}, // 5
    {0x3c, 0x20, 0x3c, 0x24, 0x3c}, // 6
    {0x3c, 0x04, 0x0c, 0x08, 0x10}, // 7
    {0x3c, 0x24, 0x3c, 0x24, 0x3c}, // 8
    {0x3c, 0x24, 0x3c, 0x04, 0x3c}  // 9
};

// leters representation
unsigned char l[5] = {0x20, 0x22, 0x20, 0x22, 0x38}; // L:
unsigned char p[5] = {0x38, 0x2a, 0x38, 0x22, 0x20}; // P:

void writeLedMatrix(unsigned char addrUp, unsigned char dataUp, unsigned char addrDown, unsigned char dataDown);

/*
 * Updates the two displays with a char array[16]
 * Params: 
 * unsigned char *field    - the first element of char array
 */
void updateDisplay(unsigned char* field) {
    char i;
    for (i = 0; i < 8; i++) {
        writeLedMatrix(i + 1, field[i + 8], i + 1, field[i]);
    }
}

/*
 * Initialises the display (brightness control)
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void initDisplay(unsigned char brite) {

    // assert the brite value
    if (brite < 0x00 || brite > 0x0f) {
        Serial.print("Error: Param= ");
        Serial.print(brite, DEC);
        Serial.println(" brite is out the range [0,15]");
        Serial.println("in MatrixDisplay.initDisplay(unsigned char brite)");
        return;
    }

    writeLedMatrix(0x09, 0x00, 0x09, 0x00);         // BCD Off
    writeLedMatrix(0x0a, brite, 0x0a, brite);       // brightness
    writeLedMatrix(0x0b, 0x07, 0x0b, 0x07);         // scan limit = 7 (full scan)
    writeLedMatrix(0x0c, 0x01, 0x0c, 0x01);         // device On
    writeLedMatrix(0x0f, 0x00, 0x0f, 0x00);         // test = 0;
}

/*
 * Initialises the display (full control)
 * Params:
 * unsigned char bcd              - BCD (0x00=false)
 * unsigned char brite            - brightness value [0,15]
 * unsigned char scanLimit        - lines to scan limit [0,7]
 * unsigned char onOff            - display On/Off (0x00=false)
 * unsigned char test             - test displays (0x00=false)
 */
void initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test) {

    // assert the values
    if (brite < 0x00 || brite > 0x0f) {

        Serial.print("Error: Param brite= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,15]");
        Serial.println("in MatrixDisplay.initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test)");
        return;
    } else if (scanLimit < 0x00 || scanLimit > 0x07) {
        Serial.print("Error: Param scanLimit= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,7]");
        ;
        Serial.println("in MatrixDisplay.initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test)");
        return;
    }

    (bcd == 0x00) ? writeLedMatrix(0x09, 0x00, 0x09, 0x00) : writeLedMatrix(0x09, 0x01, 0x09, 0x01); // BCD On/Off
    writeLedMatrix(0x0a, brite, 0x0a, brite); // brightness
    writeLedMatrix(0x0b, scanLimit, 0x0b, scanLimit); // scan limit
    (onOff == 0x00) ? writeLedMatrix(0x0c, 0x00, 0x0c, 0x00) : writeLedMatrix(0x0c, 0x01, 0x0c, 0x01); // device On/off
    (test == 0x00) ? writeLedMatrix(0x0f, 0x00, 0x0f, 0x00) : writeLedMatrix(0x0f, 0x01, 0x0f, 0x01); // test On/Off
}

/*
 * Sets the display brightness
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void setDisplayBrite(unsigned char brite) {

    // assert the brite value
    if (brite < 0x00 || brite > 0x0f) {
        Serial.print("Error: Param brite= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,15]");
        Serial.println("in MatrixDisplay.setDisplayBrite(unsigned char brite)");
        return;
    }

    writeLedMatrix(0x0a, brite, 0x0a, brite); // brightness
}

/*
 * Sets the display BCD
 * Params:
 * unsigned char bcd        - decode (0x00=false)
 */
void setDisplayBCD(unsigned char bcd) {

    writeLedMatrix(0x0a, bcd, 0x0a, bcd); // brightness
}

/*
 * Sets the display pins
 * Params:
 * int CSPin         - number of CS pin
 * int CLKPin        - number of CLK pin
 * int DINPin        - number of DIN pin
 */
void setDisplayPins(int CS_pin, int CLK_pin, int DIN_pin) {
    CSPin = CS_pin;
    CLKPin = CLK_pin;
    DINPin = DIN_pin;
}

/*
 * Rolls an image on the display one time
 * Params:
 * unsigned char *image            - the first element of image
 * unsigned char linesPerSecond    - lines per second
 */
void rollImage(unsigned char *image, unsigned int imageSize, unsigned char linesPerSecond, int hold) {

    unsigned char field[16];
    int i, j;
    for (i = 0; i <= imageSize - 16; i++) {
      for (j = 0; j < 16; j++) {
        field[j] = image[i + j];
      }
      updateDisplay(field);
      if (i == 0) {
          delay(hold);
      } else {
          delay(1000 / linesPerSecond);
      }
    }
    delay(hold);
}

/*
 * Rolls a number on the display one time
 * Params:
 * unsigned char prefix            - prefix {p,l}
 * int num                         - number to print
 * unsigned char linesPerSecond    - lines per second
 */
void rollNumber(unsigned char prefix, unsigned long int num, unsigned char linesPerSecond, int hold) {

    unsigned long int numCopy = num;
    int imageSize;

    // calculate the num length
    unsigned char digits;
    for (digits = 1; digits < 11; digits++) {
      if (numCopy < 10) {
          break;
      } else {
          numCopy = numCopy / 10;
      }
    }
    
    if (prefix == 'p' || prefix == 'l') {
        digits++;
    }
    
    if(digits < 3){
      imageSize=16;
    }else{
      imageSize=digits * 6;
    }
    
    // creates the image
    unsigned char image[imageSize];
    
    char i;
    unsigned char n;
    for (i = digits * 6 - 1; i >= 5; i -= 6) {
        if (prefix == 'p' && i == 5) {
            image[i] = 0x00;
            image[i - 1] = p[4];
            image[i - 2] = p[3];
            image[i - 3] = p[2];
            image[i - 4] = p[1];
            image[i - 5] = p[0];
        } else if (prefix == 'l' && i == 5) {
            image[i] = 0x00;
            image[i - 1] = l[4];
            image[i - 2] = l[3];
            image[i - 3] = l[2];
            image[i - 4] = l[1];
            image[i - 5] = l[0];
        } else {
            n = num % 10;
            num /= 10;
            image[i] = 0x00;
            image[i - 1] = numbers[n][4];
            image[i - 2] = numbers[n][3];
            image[i - 3] = numbers[n][2];
            image[i - 4] = numbers[n][1];
            image[i - 5] = numbers[n][0];
        }
    }
    
    
    if(digits < 3){
      for(i=digits*6; i<16;i++){
        image[i] = 0x00;
      }
    }

    // print the image
    rollImage(image, imageSize, linesPerSecond, hold);
}

/*  
 * Shifts one byte of data to MAX7219
 * Please read MAXIM-7219 Manual for further information
 * Params:
 * unsigned char DATA        - byte to send
 */
void writeLedMatrixByte(unsigned char DATA) {
    unsigned char i;
    for (i = 8; i >= 1; i--) {
        digitalWrite(CLKPin, LOW); // max1922 clock down
        digitalWrite(DINPin, DATA & 0x80); // Extracting the most significant bit
        DATA = DATA << 1; // shifs one bit
        digitalWrite(CLKPin, HIGH); // max1922 clock up
    }
}

/*  
 * Writes 4 bytes, 2 for each matrix
 * The first 2 bytes are shiffeted out to the upper LED matrix
 * The last 2 bytes stays on the lower LED Matrix
 * Params:
 * unsigned char addrUp        - address of upper matrix line
 * unsigned char dataUp        - data of upper matrix line
 * unsigned char addrDown      - address of lower matrix line
 * unsigned char dataDown      - data of lower matrix line
 */
void writeLedMatrix(unsigned char addrUp, unsigned char dataUp, unsigned char addrDown, unsigned char dataDown) {

    digitalWrite(CSPin, LOW); // load on
    writeLedMatrixByte(addrDown); // address of botton matrix
    writeLedMatrixByte(dataDown); // data of botton matrix
    writeLedMatrixByte(addrUp); // address of top matrix
    writeLedMatrixByte(dataUp); // data of top matrix
    digitalWrite(CSPin, HIGH); // load off
}
 * digits representation
 */
unsigned char numbers[10][5] = {
    {0x3c, 0x24, 0x24, 0x24, 0x3c}, // 0
    {0x18, 0x08, 0x08, 0x08, 0x1c}, // 1
    {0x3c, 0x04, 0x3c, 0x20, 0x3c}, // 2
    {0x3c, 0x04, 0x1c, 0x04, 0x3c}, // 3
    {0x24, 0x24, 0x3c, 0x04, 0x04}, // 4
    {0x3c, 0x20, 0x3c, 0x04, 0x3c}, // 5
    {0x3c, 0x20, 0x3c, 0x24, 0x3c}, // 6
    {0x3c, 0x04, 0x0c, 0x08, 0x10}, // 7
    {0x3c, 0x24, 0x3c, 0x24, 0x3c}, // 8
    {0x3c, 0x24, 0x3c, 0x04, 0x3c}  // 9
};

// leters representation
unsigned char l[5] = {0x20, 0x22, 0x20, 0x22, 0x38}; // L:
unsigned char p[5] = {0x38, 0x2a, 0x38, 0x22, 0x20}; // P:

void writeLedMatrix(unsigned char addrUp, unsigned char dataUp, unsigned char addrDown, unsigned char dataDown);

/*
 * Updates the two displays with a char array[16]
 * Params: 
 * unsigned char *field    - the first element of char array
 */
void updateDisplay(unsigned char* field) {
    char i;
    for (i = 0; i < 8; i++) {
        writeLedMatrix(i + 1, field[i + 8], i + 1, field[i]);
    }
}

/*
 * Initialises the display (brightness control)
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void initDisplay(unsigned char brite) {

    // assert the brite value
    if (brite < 0x00 || brite > 0x0f) {
        Serial.print("Error: value= ");
        Serial.print(brite, DEC);
        Serial.println(" brite is out the range [0,15]");
        Serial.println("in MatrixDisplay.initDisplay(unsigned char brite)");
        return;
    }

    writeLedMatrix(0x09, 0x00, 0x09, 0x00); // BCD Off
    writeLedMatrix(0x0a, brite, 0x0a, brite); // brightness
    writeLedMatrix(0x0b, 0x07, 0x0b, 0x07); // scan limit = 7 (full scan)
    writeLedMatrix(0x0c, 0x01, 0x0c, 0x01); // device On
    writeLedMatrix(0x0f, 0x00, 0x0f, 0x00); // test = 0;
}

/*
 * Initialises the display (full control)
 * Params:
 * unsigned char bcd              - BCD (0x00=false)
 * unsigned char brite            - brightness value [0,15]
 * unsigned char scanLimit        - lines to scan limit [0,7]
 * unsigned char onOff            - display On/Off (0x00=false)
 * unsigned char test             - test displays (0x00=false)
 */
void initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test) {

    // assert the values
    if (brite < 0x00 || brite > 0x0f) {

        Serial.print("Error: value brite= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,15]");
        Serial.println("in MatrixDisplay.initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test)");
        return;
    } else if (scanLimit < 0x00 || scanLimit > 0x07) {
        Serial.print("Error: value scanLimit= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,7]");
        ;
        Serial.println("in MatrixDisplay.initDisplay(unsigned char bcd, unsigned char brite, unsigned char scanLimit, unsigned char onOff, unsigned char test)");
        return;
    }

    (bcd == 0x00) ? writeLedMatrix(0x09, 0x00, 0x09, 0x00) : writeLedMatrix(0x09, 0x01, 0x09, 0x01); // BCD On/Off
    writeLedMatrix(0x0a, brite, 0x0a, brite); // brightness
    writeLedMatrix(0x0b, scanLimit, 0x0b, scanLimit); // scan limit
    (onOff == 0x00) ? writeLedMatrix(0x0c, 0x00, 0x0c, 0x00) : writeLedMatrix(0x0c, 0x01, 0x0c, 0x01); // device On/off
    (test == 0x00) ? writeLedMatrix(0x0f, 0x00, 0x0f, 0x00) : writeLedMatrix(0x0f, 0x01, 0x0f, 0x01); // test On/Off
}

/*
 * Sets the display brightness
 * Params:
 * unsigned char brite        - brightness value [0,15]
 */
void setDisplayBrite(unsigned char brite) {

    // assert the brite value
    if (brite < 0x00 || brite > 0x0f) {
        Serial.print("Error: value brite= ");
        Serial.print(brite, DEC);
        Serial.println(" is out the range [0,15]");
        Serial.println("in MatrixDisplay.setDisplayBrite(unsigned char brite)");
        return;
    }

    writeLedMatrix(0x0a, brite, 0x0a, brite); // brightness
}

/*
 * Sets the display BCD
 * Params:
 * unsigned char bcd        - decode (0x00=false)
 */
void setDisplayBCD(unsigned char bcd) {

    writeLedMatrix(0x0a, bcd, 0x0a, bcd); // brightness
}

/*
 * Sets the display pins
 * Params:
 * int CSPin         - number of CS pin
 * int CLKPin        - number of CLK pin
 * int DINPin        - number of DIN pin
 */
void setDisplayPins(int CS_pin, int CLK_pin, int DIN_pin) {
    CSPin = CS_pin;
    CLKPin = CLK_pin;
    DINPin = DIN_pin;
}

/*
 * Rolls an image on the display one time
 * Params:
 * unsigned char *image            - the first element of image
 * unsigned char linesPerSecond    - lines per second
 */
void rollImage(unsigned char *image, unsigned int imageSize, unsigned char linesPerSecond, int hold) {

    unsigned char field[16];
    int i, j;
    for (i = 0; i <= imageSize - 16; i++) {
      for (j = 0; j < 16; j++) {
        field[j] = image[i + j];
      }
      updateDisplay(field);
      if (i == 0) {
          delay(hold);
      } else {
          delay(1000 / linesPerSecond);
      }
    }
    delay(hold);
}

/*
 * Rolls a number on the display one time
 * Params:
 * unsigned char prefix            - prefix {p,l}
 * int num                         - number to print
 * unsigned char linesPerSecond    - lines per second
 */
void rollNumber(unsigned char prefix, unsigned long int num, unsigned char linesPerSecond, int hold) {

    unsigned long int numCopy = num;
    int imageSize;

    // calculate the num length
    unsigned char digits;
    for (digits = 1; digits < 11; digits++) {
      if (numCopy < 10) {
          break;
      } else {
          numCopy = numCopy / 10;
      }
    }
    
    if (prefix == 'p' || prefix == 'l') {
        digits++;
    }
    
    if(digits < 3){
      imageSize=16;
    }else{
      imageSize=digits * 6;
    }
    
    // creates the image
   unsigned char image[imageSize];

    
    char i;
    unsigned char n;
    for (i = digits * 6 - 1; i >= 5; i -= 6) {
        Serial.println("loop");
        if (prefix == 'p' && i == 5) {
            image[i] = 0x00;
            image[i - 1] = p[4];
            image[i - 2] = p[3];
            image[i - 3] = p[2];
            image[i - 4] = p[1];
            image[i - 5] = p[0];
        } else if (prefix == 'l' && i == 5) {
            image[i] = 0x00;
            image[i - 1] = l[4];
            image[i - 2] = l[3];
            image[i - 3] = l[2];
            image[i - 4] = l[1];
            image[i - 5] = l[0];
        } else {
            n = num % 10;
            num /= 10;
            image[i] = 0x00;
            image[i - 1] = numbers[n][4];
            image[i - 2] = numbers[n][3];
            image[i - 3] = numbers[n][2];
            image[i - 4] = numbers[n][1];
            image[i - 5] = numbers[n][0];
        }
    }
    
    
    if(digits < 3){
      for(i=digits*6; i<16;i++){
        image[i] = 0x00;
      }
    }

    // print the image
    rollImage(image, imageSize, linesPerSecond, hold);
}

/*  
 * Shifts one byte of data to MAX7219
 * Please read MAXIM-7219 Manual for further information
 * Params:
 * unsigned char DATA        - byte to send
 */
void writeLedMatrixByte(unsigned char DATA) {
    unsigned char i;
    for (i = 8; i >= 1; i--) {
        digitalWrite(CLKPin, LOW); // max1922 clock down
        digitalWrite(DINPin, DATA & 0x80); // Extracting the most significant bit
        DATA = DATA << 1; // shifs one bit
        digitalWrite(CLKPin, HIGH); // max1922 clock up
    }
}

/*  
 * Writes 4 bytes, 2 for each matrix
 * The first 2 bytes are shiffeted out to the upper LED matrix
 * The last 2 bytes stays on the lower LED Matrix
 * Params:
 * unsigned char addrUp        - address of upper matrix line
 * unsigned char dataUp        - data of upper matrix line
 * unsigned char addrDown      - address of lower matrix line
 * unsigned char dataDown      - data of lower matrix line
 */
void writeLedMatrix(unsigned char addrUp, unsigned char dataUp, unsigned char addrDown, unsigned char dataDown) {

    digitalWrite(CSPin, LOW); // load on
    writeLedMatrixByte(addrDown); // address of botton matrix
    writeLedMatrixByte(dataDown); // data of botton matrix
    writeLedMatrixByte(addrUp); // address of top matrix
    writeLedMatrixByte(dataUp); // data of top matrix
    digitalWrite(CSPin, HIGH); // load off
}
