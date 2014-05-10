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
 * This module is a basic Tetris game engine with levels,
 * score and speed control
 */

#include "Tetris.h"
#include "TRandom.h"
#include "MatrixDisplay.h"
#include "Effects.h"
#include "MIDIPlayer.h"
#include "Arduino.h"

/* timers and counters */

unsigned char timeEnd; // inter counter limiter
int animationDelay = 75; // delay beetween annimations (ms)
unsigned char gamePaused = 0x00; // game paused flag

/* difficulty */
unsigned char level = 0x00; // level [0,15] 
unsigned char difficulty; // difficulty [0,3];
unsigned char gameSpeed; // game speed [0,3]

/* line counters */
unsigned char limit = 0x11; // limit for line check
unsigned char linesMade = 0x00; // lines made
unsigned char goalLines = 1; // lines per level
unsigned char pieceCount = 0; // pieces droped on the actual level
unsigned char launchDifficulty = 4; // launches a dificulty every n pieces

// game variables
unsigned char pieceNum; // actual piece number [0,6]
unsigned char nextPieceNum; // next piece number [0,6]
unsigned char piecePos; // actual piece position [0,4]
char pieceShift; // actual piece shift (left/right) [-3,4]
unsigned char pieceY; // actual piece Y position [0,19]
unsigned char gameStart = 0x00; // if game is started
unsigned char gameEnd = 0x00; // if game is ended

/*
 * tetris pieces
 *
 * each piece is represented on an array of 4 chars.
 * in this three lever array there are represented all pieces
 * in all positions of Tetris: 7 types * 4 positions = 28 pieces.
 * example:
 *   the "I" piece -> position 0 or 2:
 *   {0x00,0x3c,0x00,0x00} =        00000000
 *                                       00111100
 *                                       00000000
 *                                       00000000
 *   the "I" piece -> position 1 or 3:
 *   {0x08,0x08,0x08,0x08} =        00001000
 *                                       00001000
 *                                       00001000
 *                                       00001000
 */
unsigned char pieces[7][4][4] = {
    {
        {0x00, 0x18, 0x18, 0x00}, // Piece O, position 0
        {0x00, 0x18, 0x18, 0x00}, // Piece O, position 1
        {0x00, 0x18, 0x18, 0x00}, // Piece O, position 2
        {0x00, 0x18, 0x18, 0x00}
    }, // Piece O, position 3
    {
        {0x00, 0x3c, 0x00, 0x00}, // Piece I, position 0
        {0x08, 0x08, 0x08, 0x08}, // Piece I, position 1
        {0x00, 0x3c, 0x00, 0x00}, // Piece I, position 2
        {0x08, 0x08, 0x08, 0x08}
    }, // Piece I, position 3
    {
        {0x00, 0x0c, 0x18, 0x00}, // Piece S, position 0
        {0x08, 0x0c, 0x04, 0x00}, // Piece S, position 1
        {0x00, 0x0c, 0x18, 0x00}, // Piece S, position 2
        {0x08, 0x0c, 0x04, 0x00}
    }, // Piece S, position 3
    {
        {0x00, 0x18, 0x0c, 0x00}, // Piece Z, position 0
        {0x04, 0x0c, 0x08, 0x00}, // Piece Z, position 1
        {0x00, 0x18, 0x0c, 0x00}, // Piece Z, position 2
        {0x04, 0x0c, 0x08, 0x00}
    }, // Piece Z, position 3
    {
        {0x00, 0x1c, 0x10, 0x00}, // Piece L, position 0
        {0x08, 0x08, 0x0c, 0x00}, // Piece L, position 1
        {0x04, 0x1c, 0x00, 0x00}, // Piece L, position 2
        {0x18, 0x08, 0x08, 0x00}
    }, // Piece L, position 3
    {
        {0x00, 0x1c, 0x04, 0x00}, // Piece J, position 0
        {0x0c, 0x08, 0x08, 0x00}, // Piece J, position 1
        {0x10, 0x1c, 0x00, 0x00}, // Piece J, position 2
        {0x08, 0x08, 0x18, 0x00}
    }, // Piece J, position 3
    {
        {0x00, 0x1c, 0x08, 0x00}, // Piece T, position 0
        {0x08, 0x0c, 0x08, 0x00}, // Piece T, position 1
        {0x08, 0x1c, 0x00, 0x00}, // Piece T, position 2
        {0x04, 0x0c, 0x04, 0x00}
    } // Piece T, position 3
};

// auxiliary structs
unsigned char field[20]; // The game field. 16+4 hidden lines
unsigned char displayField[16]; // The game field for the display
unsigned char actualPiece[] = {0x00, 0x00, 0x00, 0x00}; // actual piece. used to add/subtract from field
unsigned char tempPiece[] = {0x00, 0x00, 0x00, 0x00}; //temporary piece for collision detection purpose

unsigned long int score = 0;

// prototypes
unsigned char getTimeEnd();
void updateDisplayField();
unsigned char *getField();
void generateField();
void generatPiece();
void endGame();
void startLevel();
void shiftRigth(unsigned char piece[]);
void shiftLeft(unsigned char piece[]);
unsigned char isCollision();
void deletePiece();
void addPiece();
void updateTempPiece();
char getMaxLeftShift();
char getMaxRigthShift();
void checkLines();
void checkGame();
void checkEmptyLines();
void removeLine(char line);
void addDifficulty();
void addLine();
void addBlock();

/*
 * Gets time end flag for speed control
 * Returns:
 * unsigned char     -time end flag.
 */
unsigned char getTimeEnd() {
    return timeEnd;
}

/*
 * updates the display field whith the 
 * actual game field state.
 * displayField[0,15]=field[2,17]
 */
void updateDisplayField() {
    for (int i = 2; i < 18; i++) {
        displayField[i - 2] = field[i];
    }
}

/*
 * gets the interface field[16]
 * Returns:
 * unsigned char     -array of displayField
 */
unsigned char *getField() {
    updateDisplayField();
    return displayField;
}

/*
 * generated a new empty field(display) and updates the 
 * displayField.
 * the field is a 20 char array, 16[2,17] visible for display field,
 * 2[0,1] hidden and empty and 2[18,19] hidden and full for collision
 * detection
 */
void generateField() {

    char i;
    for (i = 0; i < 18; i++) {
        field[i] = 0x00; // the first 18 lines are empty
    }

    field[18] = 0xff; // the last 2 lines are full
    field[19] = 0xff; // for collision detection

    updateDisplayField(); // new field made, updates the display field
}

/*
 * generates a new random piece
 */
void generatePiece() {

    pieceY = 0x00; // reset piece y position
    pieceShift = 0x00; // reset piece x position
    piecePos = 0x00; // reset piece rotate position

    if (pieceCount == launchDifficulty - 1) { // adds a dificulty if is the time for it
        pieceCount = 0;
    } else {
        pieceCount++;
    }
    addDifficulty();

    pieceNum = nextPieceNum; // new piece
    nextPieceNum = tRandom(0, 7); // new piece
    actualPiece[0] = pieces[pieceNum][0][0]; // copy piece to actual piece
    actualPiece[1] = pieces[pieceNum][0][1];
    actualPiece[2] = pieces[pieceNum][0][2];
    actualPiece[3] = pieces[pieceNum][0][3];

}

/*
 * Just starts a new game
 */
void startGame() {
    level = 0; // reset level
    difficulty = 0; // reset difficulty level
    gameSpeed = 0; // reset game seep level 
    timeEnd = pow(2, 5); // sets the time endo to default (2Hz)

    startLevel(); // starts a new level
    playMusic(3);
}

/* ends the game*/
void endGame() {
    gamePaused = 0x01;
    stopMusic();
    gameOver(displayField);
    rollNumber('l', level, 10, 1000);
    rollNumber('p', score, 10, 1000);
    gamePaused = 0x00;

}

/*
 * Just starts a new level
 */
void startLevel() {
    pieceCount = 0x00;
    gamePaused = 0x01;

    if (level % 4 == 0 && level != 0) {
        playMusic(2);
        pushLeftAll(displayField);
        rollNumber('l', level, 10, 1000);
        rollNumber('p', score, 10, 1000);
    }else{
        shiftLeftAll(displayField);
    }
    
    playMusic(0);
    delay(1900);
    playMusic(3);
    
    generateField(); // new field
    generatePiece(); // new piece
    addPiece(); // adds the new piece to fild
    updateDisplayField(); // updates the display field
    limit = 0x11; // sets the checkLines limit to default

    gamePaused = 0x00;
 }

/*
 * Rotates the actual piece CCW with collision detection
 * the rotation is just a piece layout change with the actual piecePos,
 * rotates only if is possible.
 */
void rotate() {

    char i = 0;
    // try to rotate piece only if it still fits in the field after rotation
    if (pieceNum != 0x00 && getMaxLeftShift() <= pieceShift && pieceShift <= getMaxRigthShift()) {

        /*
         * first deletes the actual piece from the field and then
         * checks is there is no collision with the new position. if the
         * piece fits beetween the other pieces adds it to fild. else retores the original
         * piece if collides with other pieces 
         */
        deletePiece(); // deletes the actual piece from field
        piecePos++; // increments the piece y position
        if (piecePos == 0x04) { // controls the rotation position range
            piecePos = 0x00;
        }
        tempPiece[0] = pieces[pieceNum][piecePos][0]; // updates the temp piece
        tempPiece[1] = pieces[pieceNum][piecePos][1];
        tempPiece[2] = pieces[pieceNum][piecePos][2];
        tempPiece[3] = pieces[pieceNum][piecePos][3];

        if (pieceShift < 0x00) { // shifts the temp piece to actal x position
            for (i = (-1) * pieceShift; i > 0; i--) {
                shiftLeft(tempPiece);
            }
        } else {
            for (i = pieceShift; i > 0; i--) {
                shiftRigth(tempPiece);
            }
        }

        if (isCollision() == 0x00) { // no collision? aplies the new rotation position
            actualPiece[0] = pieces[pieceNum][piecePos][0];
            actualPiece[1] = pieces[pieceNum][piecePos][1];
            actualPiece[2] = pieces[pieceNum][piecePos][2];
            actualPiece[3] = pieces[pieceNum][piecePos][3];

            if (pieceShift < 0x00) {
                for (i = (-1) * pieceShift; i > 0; i--) {
                    shiftLeft(actualPiece);
                }
            } else {
                for (i = pieceShift; i > 0; i--) {
                    shiftRigth(actualPiece);
                }
            }
        } else { // collision? just go back and aplies the last piece
            if (piecePos == 0x00) {
                piecePos = 0x03;
            } else {
                piecePos--;
            }
        }

        addPiece();
        updateDisplayField(); // updates the display field
    }
}

/*
 * Moves the piece to rigth with collision detection
 * moving a piece is just a shift of the 4 chars of the piece
 */
void goRigth() {
    /* 
     * try to move piece only if it still fits in the field after rotation
     * the trick is check if one of the less significant bits of each char is equal to 1 
     */
    if (((actualPiece[0] | actualPiece[1] | actualPiece[2] | actualPiece[3]) & 0x01) == 0x00) {
        /*
         * first deletes the actual piece from the field and then
         * checks is there is no collision with the new position. if the
         * piece fits beetween the other pieces adds it to fild. else retores the original
         * piece if collides with other pieces 
         */
        deletePiece(); // deletes the actual piece from field
        updateTempPiece(); // updates temp piece
        shiftRigth(tempPiece); // shifts the temp piece
        if (isCollision() == 0x00) { // no collision? shifts the actual piece
            shiftRigth(actualPiece); //  and adds it to the field
            addPiece();
            updateTempPiece();
            pieceShift++;
        }
        addPiece(); // no collision? replace the actual piece on field
        updateDisplayField(); // updates the display field
    }
}

/* 
 * just shifts the given piece to rigth (one position) 
 */
void shiftRigth(unsigned char piece[]) {
    piece[0] = piece[0] >> 1;
    piece[1] = piece[1] >> 1;
    piece[2] = piece[2] >> 1;
    piece[3] = piece[3] >> 1;
}

/*
 * Moves the piece to left with collision detection
 * moving a piece is just a shift of the 4 chars of the piece
 */
void goLeft() {
    /* 
     * try to move piece only if it still fits in the field after rotation
     * the trick is check if one of the most significant bits of each char is equal to 1 
     */
    if (((actualPiece[0] | actualPiece[1] | actualPiece[2] | actualPiece[3]) & 0x80) == 0x00) {
        /*
         * see goRigth() comments for similar description
         */
        deletePiece();
        updateTempPiece();
        shiftLeft(tempPiece);
        if (isCollision() == 0x00) {
            shiftLeft(actualPiece);
            addPiece();
            updateTempPiece();
            pieceShift--;
        }
        addPiece();
        updateDisplayField();
    }
}

/* 
 * just shifts the given piece to left (one position) 
 */
void shiftLeft(unsigned char piece[]) {
    piece[0] = piece[0] << 1;
    piece[1] = piece[1] << 1;
    piece[2] = piece[2] << 1;
    piece[3] = piece[3] << 1;
}

/*
 * Moves the piece down with collision detection
 */
void goDown() {
    /* 
     * again the trick is delete the actual piece from field
     * and test collisin with the new position and temp piece
     */
    deletePiece();
    updateTempPiece();
    pieceY++;
    if (isCollision() == 0x00) { // no collision? adds the piece
        addPiece();
    } else if (isCollision() != 0x00 && pieceY > 0x02) { // collision? restores the piece
        pieceY--;
        addPiece();
        checkGame(); // game status check
    } else { // game over
        endGame();
        startGame();
    }

    updateDisplayField();
}

/*
 * just adds actual piece to field (bitwise OR operation)
 */
void addPiece() {
    field[pieceY] = field[pieceY] | actualPiece[0];
    field[pieceY + 1] = field[pieceY + 1] | actualPiece[1];
    field[pieceY + 2] = field[pieceY + 2] | actualPiece[2];
    field[pieceY + 3] = field[pieceY + 3] | actualPiece[3];
}

/*
 * just deletes the actual piece from field (bitwise XOR operation)
 */
void deletePiece() {
    field[pieceY] = field[pieceY] ^ actualPiece[0];
    field[pieceY + 1] = field[pieceY + 1] ^ actualPiece[1];
    field[pieceY + 2] = field[pieceY + 2] ^ actualPiece[2];
    field[pieceY + 3] = field[pieceY + 3] ^ actualPiece[3];
}

/*
 * just verifies if there is collition beetween temp piece and field
 * (bitwise AND/OR operation)
 */
unsigned char isCollision() {

    return (((field[pieceY] & tempPiece[0]) |
            (field[pieceY + 1] & tempPiece[1]) |
            (field[pieceY + 2] & tempPiece[2]) |
            (field[pieceY + 3] & tempPiece[3])));
}

/*
 * just updates the temp piece 
 */
void updateTempPiece() {
    tempPiece[0] = actualPiece[0];
    tempPiece[1] = actualPiece[1];
    tempPiece[2] = actualPiece[2];
    tempPiece[3] = actualPiece[3];
}

/*
 * returns the piece max left shift
 * please check the pieces layouts for further information and understanding
 */
char getMaxLeftShift() {
    if ((((piecePos + 1) % 4 == 0x01 || (piecePos + 1) % 4 == 0x03) &&
            (pieceNum != 0x01 && pieceNum != 0x03 && pieceNum != 0x04)) ||
            ((piecePos + 1) % 4 == 0x01 && ((pieceNum == 0x05 || pieceNum == 0x04)))) {
        return 0xfc;
    }
    if (pieceNum == 0x01 && ((piecePos + 1) % 4 == 0x00 || (piecePos + 1) % 4 == 0x02)) {
        return 0xfe;
    }

    return 0xfd;
}

/*
 * returns the piece max rigth shift
 * please check the pieces layouts for further information and understanding
 */
char getMaxRigthShift() {
    if (pieceNum == 0x01 || ((piecePos + 1) % 4 == 0x03 &&
            (pieceNum == 0x01 | pieceNum == 0x05 || pieceNum == 0x05)) ||
            (pieceNum == 0x01 || (piecePos + 1) % 4 == 0x00)) {
        return 0x03;
    }

    return 0x02;
}

/*
 * checks made lines and deletes them
 */
void checkLines() {
  gamePaused=0x01;
    char i;
    char j = 0;
    for (i = pieceY; i < pieceY + 4 && i <= limit; i++) {
        if (field[i] == 0xff) {
            j++;
            blinkLine(displayField, i-2);
            removeLine(i);
            updateDisplayField();
            linesMade++;
        }
    }

    switch (j) {
        case 1:
            score += 50 * (level + 1);
            break;
        case 2:
            score += 100 * (level + 1);
            break;
        case 3:
            score += 300 * (level + 1);
            break;
        case 4:
            score += 1200 * (level + 1);
            break;
        default:;
    }
    
    gamePaused=0x00;
}

/*
 * checks the game state difficulty
 */
void checkGame() {
    checkLines();
    if (linesMade >= goalLines) {
        linesMade = 0;
        checkEmptyLines();
        if (level < 0x0f) {
            level++;
        } else {
            endGame();
        }
        difficulty = level % 4;
        gameSpeed = level >> 2;
        timeEnd = pow(2, 5 - gameSpeed);
        startLevel();
    } else {
        generatePiece();
        addPiece();
    }
}

/*
    counts empty lines
 */

void checkEmptyLines() {
    gamePaused = 0x01;
    stopMusic();
    char i;
    for (i = 0; i < 18; i++) {
        if (field[i] != 0x00) {
            gamePaused = 0x00;
            score += (i - 2)*100;
            return;
        } else {
            field[i] = 0xff;
            updateDisplayField();
            updateDisplay(displayField);
            playNote(i * 100 + 1000, animationDelay);
            delay(animationDelay);
        }
    }
    gamePaused = 0x00;
    score += 1600;
}

/*
    removes a line and shifts down the rest
 */
void removeLine(char line) {
    char i;
    for (i = line; i > 0; i--) {
        field[i] = field[i - 1];
    }
    field[0] = 0x00;
}

void addDifficulty() {
  gamePaused=0x01;
    if (pieceCount % launchDifficulty == 0x00) {
        pieceCount = 0x01;
        switch (difficulty) {
            case 1:
                addLine();
                break;
            case 2:
                addBlock();
                break;
            case 3:
                addLine();
                addBlock();
                break;
            default:;
        }
    }
  gamePaused=0x00;
}

/*
    add a new botton line and shifts up the rest
 */
void addLine() {
    gamePaused=0x01;
    char i;
    for (i = 0; i < 17; i++) {
        field[i] = field[i + 1];
    }
    field[17] = 0xff;
    updateDisplayField();
    blinkLine(displayField, 15);
    limit--;
    gamePaused=0x00;
}

// add a new block

void addBlock() {
  gamePaused=0x01;
  
    char i;
    char c;
    char collumn = tRandom(0, 8);

    for (i = 17; i > 1; i--) {
        c = field[i];
        c = c >> collumn;
        c &= 0x01;
        if (c == 0x00) {
            blinkBlock(displayField, i-2, collumn);
            c = 0x01;
            c = c << collumn;
            field[i] |= c;
            gamePaused=0x00;
            return;
        }
    }
    gamePaused=0x00;
}

unsigned char isGamePaused() {
    return gamePaused;
}

unsigned char getLevel(){
  return level;
}

unsigned int getScorePoints(){return score;}

unsigned char getNextPiece(){
  return nextPieceNum;
}

