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

#include "Effects.h"
#include "Arduino.h"
#include "MatrixDisplay.h"
#include "TRandom.h"
#include <math.h>

int bigDelay = 2000;    // big delay definition
int smallDelay = 25;    // small delay definition
int blinkDelay = 50;    // blink delay

// intro field map
unsigned char introMap[]={
                        0xff,0x9b,0x83,0xd3,
                        0xd1,0xdb,0x81,0x93,
                        0xd9,0xcb,0x83,0x99,
                        0xdb,0xc3,0xc9,0xff
                      };

// tetris field map
unsigned char tetrisMap[]={
                        0x44,0xea,0x4e,0x48,
                        0x66,0x00,0x46,0xea,
                        0x46,0x6a,0x00,0x46,
                        0x08,0x44,0x42,0x4c
                      };
                
// game over fiel map
unsigned char gameOverMap[]={
                        0x00,0x1c,0x24,0x24,
                        0x1c,0x04,0x38,0x00,
                        0x38,0x04,0x1c,0x24,
                        0x1c,0x00,0x3c,0x2a,
                        0x2a,0x2a,0x2a,0x00,
                        0x18,0x24,0x3c,0x20,
                        0x1c,0x00,0x00,0x00,
                        0x18,0x24,0x24,0x24,
                        0x18,0x00,0x22,0x22,
                        0x22,0x22,0x14,0x08,
                        0x00,0x18,0x24,0x3c,
                        0x20,0x1c,0x00,0x1c,
                        0x20,0x20,0x20,0x20,
                        0x00
                      };
                      
                     
// men pushinh annimation map
unsigned char menPushing[4][16]={{0x80,0x80,0x80,0x80,
                               0x80,0x80,0x80,0x80,
                               0x80,0x80,0xa0,0xf0,
                               0xb0,0xb0,0xd0,0xd0},
                              {0x80,0x80,0x80,0x80,
                               0x80,0x80,0x80,0x80,
                               0x80,0x80,0x90,0xf8,
                               0x98,0x98,0xa8,0xa4},
                              {0x80,0x80,0x80,0x80,
                               0x80,0x80,0x80,0x80,
                               0x80,0x80,0x90,0xf8,
                               0x98,0x8c,0x8a,0x85},
                              {0x80,0x80,0x80,0x80,
                               0x80,0x80,0x80,0x80,
                               0x80,0x80,0x90,0xf8,
                               0x98,0x98,0xa4,0x92}
                           };
                      
unsigned char displayMap[16];       // field to display

// prototipes
unsigned char bitCount(unsigned char *image);
void shiftLeftAll(unsigned char *image);
void pushLeftAll(unsigned char *image);

/*
 * Plays the intro animation
 */
void intro(){
    unsigned char c, i, l, p;
    int counter = bitCount(introMap);
    
    delay(bigDelay);
    // this part creates a dot by dot image of Intro Map randomly
    // it will neve repeat a dot and it stops only when is finished
    while(counter>0){
        l = random(0,16);       // line random
        c = random(0,8);        // collumn random
        p = 0x01;               // one dot line...
        p = p<<c;               // ... shifted to the rigth position
    
        // set the dot on, if is off and is part od the intro map
        if(((introMap[l] & p) != 0x00) && (displayMap[l]&p)==0x00){
            displayMap[l] |= p;
            //setDisplayBrite(15-counter/4.75);
            updateDisplay(displayMap);
            counter--;
            delay(smallDelay);
        }
    }
    
    // this part fills the field with the dots remaining
    // the methode is the same of above
    delay(bigDelay);
    counter=bitCount(introMap);
    
    while(counter<128){
        l = random(0,16);
        c = random(0,8);
        p = 0x01;
        p = p<<c;
    
        if((displayMap[l] & p) == 0x00){
            displayMap[l] |= p;
            updateDisplay(displayMap);
            counter++;
            delay(smallDelay);
        }
    }
    
    // this part removes all dots until the
    // tetris map is showed (same of above))
    counter = 128 - bitCount(tetrisMap);
    while(counter>0){
        l = random(0,16);
        c = random(0,8);
        p = 0x01;
        p = p<<c;
    
        if(((tetrisMap[l] & p) == 0x00) && ((displayMap[l] & p) != 0x00)){
            displayMap[l]  ^= p;
            updateDisplay(displayMap);
            counter--;
            delay(smallDelay);
        }
    }
    
    // this part just inverts all dots in from top and
    // botton to center
    delay(bigDelay);
    for(i=0;i<8;i++){
        displayMap[i] ^= 0xff;
        displayMap[15-i] ^= 0xff;
        updateDisplay(displayMap);
        delay(200);
    }
    
    // this part makes all lines full from 
    // center to botton and top
    for(i=0;i<8;i++){
        displayMap[7-i] = 0xff;
        displayMap[8+i] = 0xff;
        updateDisplay(displayMap);
        delay(100);
    }
    
    // this part makes all lines empty from
    // top and botton to center
    for(i=0;i<8;i++){
        displayMap[i] = 0x00;
        displayMap[15-i] = 0x00;
        updateDisplay(displayMap);
        delay(50);
    }
}

/*
 * Blinks a line in a field
 * Params:
 * unsigned char* field     - source field
 * unsigned char line       - line to blink [0,15]
 */
void blinkLine(unsigned char* field, unsigned char line){
    
    // assert the line value
    if (line < 0x00 || line > 0x0f) {
        Serial.print("Error: value= ");
        Serial.print(line, DEC);
        Serial.println(" line is out the range [0,15]");
        Serial.println("in Effects.changeLine(unsigned char* field, unsigned char line)");
        return;
    }
    
    // just adds and removes the line two times
    field[line]=0x00;
    updateDisplay(field);
    delay(blinkDelay);
    field[line]=0xff;
    updateDisplay(field);
    delay(blinkDelay);
    
    field[line]=0x00;
    updateDisplay(field);
    delay(blinkDelay);
    field[line]=0xff;
    updateDisplay(field);
    delay(blinkDelay);
    
}

/*
 * Blinks a block in a field
 * Params:
 * unsigned char* field     - source field
 * unsigned char line       - line of block to blink [0,15]
 * unsigned char collumn    - collum of block to blink
 */
void blinkBlock(unsigned char* field, unsigned char line, unsigned char collumn){
    
    // assert the line value
    if (line < 0x00 || line > 0x0f) {
        Serial.print("Error: value= ");
        Serial.print(line, DEC);
        Serial.println(" line is out the range [0,15]");
        Serial.println("in Effects.addBlock(unsigned char* field, unsigned char line, unsigned char collumn)");
        return;
    }
    
    // assert the collumn value
    if (collumn < 0x00 || collumn > 0x07) {
        Serial.print("Error: value= ");
        Serial.print(collumn, DEC);
        Serial.println(" collumn is out the range [0,8]");
        Serial.println("in Effects.addBlock(unsigned char* field, unsigned char line, unsigned char collumn)");
        return;
    }
    
    // just adds and removes the block two times
    unsigned char c= 0x01<<collumn;
    field[line]^=c;
    updateDisplay(field);
    delay(blinkDelay);
    field[line]^=c;
    updateDisplay(field);
    delay(blinkDelay);
    
    field[line]^=c;
    updateDisplay(field);
    delay(blinkDelay);
    field[line]^=c;
    updateDisplay(field);
    delay(blinkDelay);
}

/*
 * Makes a transition beetween the actual field and 
 * game over field
 * Params:
 * unsigned char* field     - source field
 */
void gameOver(unsigned char* field){
    unsigned char res[16];
    unsigned char i, l, c, counter, p;
    
    updateDisplay(field);
   
    // copies the game over map to res
    for(i=0;i<16;i++){
        res[i]=gameOverMap[i];
    }
    
    // first full the actual field 
    counter = bitCount(field);
    while(counter<128){
        l = random(0,16);
        c = random(0,8);
        p = 0x01;
        p = p<<c;
    
        if(((field[l] & p) == 0x00)){
            field[l] |= p;
            updateDisplay(field);
            counter++;
            delay(smallDelay);
        }
    }
    
    // then shows the game over field (upper part))
    counter = 128 - bitCount(res);
    while(counter>0){
        l = random(0,16);
        c = random(0,8);
        p = 0x01;
        p = p<<c;
    
        if(((res[l] & p) == 0x00) && (field[l]&p)!=0x00){
            field[l] ^= p;
            updateDisplay(field);
            counter--;
            delay(smallDelay);
        }
    }
    
    // rolls the rest of game over field
    rollImage(gameOverMap, 53, 5, 1000);
}

/*
 * Counts all active dots in a field. this method
 * uses a part of the DEC2BIN algorithm but instead of adding
 * 1' and 0' it increments the counter on every active bit (1's)
 */
unsigned char bitCount(unsigned char *image){
    unsigned char i;
    unsigned char j;
    unsigned char count = 0;
    unsigned char num;
    
    for(i=0;i<16;i++){
        num= image[i];                  // each line is a number
        j=8;                            // and it can be decomposed
        while(num>0){                   // in powers of two.
            if(num>=(0x01<<(j-1))){
                num-=(0x01<<(j-1));
                count++;
            }
            j--;
        }
    }
    
    return count;
}

/*
 * Annimation beetween levels
 * Params:
 * unsigned char* field     - source field
 */
void shiftLeftAll(unsigned char *image){
    // just shifts an image to left until it desapears 
    char i,j;
    updateDisplay(image);
    delay(100);
    for(i=0;i<8;i++){
        for(j=0;j<16;j++){
            image[j]=image[j]<<1;
        }
        updateDisplay(image);
        delay(100); 
    }
}

/*
 * Annimation beetween speed changes
 * Params:
 * unsigned char* field     - source field
 */
void pushLeftAll(unsigned char *image){
    
    // this methode sifts an image to left with annimation
    unsigned char c, i,j,k;
    unsigned char left[16];
    unsigned char right[16];
    
    // for each frame is calculated based on the actual position
    // of the annimation. there are two field, the rigth with
    // the men pushing animation, and the left with the actual field.
    // the men pushing is made with four frames loop and the actual 
    // frame is chosen by counter%4 and this is why we need to 
    // calculate and shift the images on every frame.
    
    // image copy to the left field
    for(i=0;i<16;i++){
        left[i]=image[i];
    }

    updateDisplay(image);
    delay(1000); 
    
    for(i=0;i<13;i++){
        // copies the men pushing frame
        for(j=0;j<16;j++){
            right[j] = menPushing[i%4][j];
        }    
        
        // reload the image field
        for(j=0;j<16;j++){
            image[j]=left[j];
        }
        // shift the rigth image to left i times
        for(k=i;k>0;k--){
            for(j=0;j<16;j++){
                c = right[j] & 0x80;            // msb of current rigth line
                c = c >> 7;                     // msb to lsb
                image[j] = image[j] << 1;       // shift left image to left
                right[j] = right[j] << 1;       // shift rigth image to left
                image[j] |= c;                  // adds the new bit on the left image
            }
        }
        updateDisplay(image);           // display the left image
        delay(200);
    }
}
