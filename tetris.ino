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
 * The main class handles the pin configuration and the 
 * game engine.
 * It also sets the timmer and hardware interrupts.
 */

#include "TRandom.h"
#include "MatrixDisplay.h"
#include "Tetris.h"
#include "Effects.h"
#include "MIDIPlayer.h"
#include "LEDControl.h"

/***** Port definitions *****/
// MAX7219
int ledMatrixCLK = 8;       // MAX7219 clock port
int ledMatrixCS = 9;        // MAX7219 load port
int ledMatrixDIN = 10;      // MAX7219 Data In port

// press buttons
int goRigthSwitchPin = 4;   // go rigth switch pin
int goDownSwitchPin = 6;    // go down switch pin
int rotateSwitchPin = 7;    // rotate switch pin
int goLeftSwitchPin = 3;    // go left switch pin
int pausePin = 5;           // pause switch pin

//level counter led pins
int counterLed_0 = 14;      // counter LED #0
int counterLed_1 = 15;      // counter LED #1
int counterLed_2 = 16;      // counter LED #2
int counterLed_3 = 17;      // counter LED #3

// next piece led pins
int nextPieceLed_0 = 13;    // next piece LED #0
int nextPieceLed_1 = 18;    // next piece LED #1
int nextPieceLed_2 = 19;    // next piece LED #2

//level counter led status
unsigned char counterLedStatus[5] = {0, 0, 0, 0, 0};
// next piece led status
unsigned char nextPieceLedStatus[3] = {0, 0, 0};

// button flags
unsigned char goRigthFlag = 0x00;       // go rigth interrupt request
unsigned char goLeftFlag = 0x00;        // go left interrupt request
unsigned char goDownFlag = 0x00;        // go down interrupt request
unsigned char rotateFlag = 0x00;        // rotate piece interrupt request

// switches control
unsigned char lastGoRigthSwitchStatus = 0x00;       // last go rigth switch pin status
unsigned char lastGoDownSwitchStatus = 0x00;        // last go down switch pin status
unsigned char lastRotateSwitchStatus = 0x00;        // last rotate switch pin status
unsigned char lastGoLeftSwitchStatus = 0x00;        // last go left switch pin status

// speaker
int speakerPin = 12;                // speaker pin

// timer variables
int timer1_counter;                 // global timer
unsigned char gameTimeStep = 0x00;  // game timer step

/*
 * interrupt service routine 
 * lauch freq equals to 64Hz
 */
ISR(TIMER1_OVF_vect) {

    TCNT1 = timer1_counter;     // preload timer
    noInterrupts();             // disable all interrupts

    // update the next piece and level LED status
    setCounter(counterLedStatus, getLevel());
    setPiece(nextPieceLedStatus, getNextPiece());
    digitalWrite(counterLed_0, counterLedStatus[0]);
    digitalWrite(counterLed_1, counterLedStatus[1]);
    digitalWrite(counterLed_2, counterLedStatus[2]);
    digitalWrite(counterLed_3, counterLedStatus[3]);

    digitalWrite(nextPieceLed_0, nextPieceLedStatus[0]);
    digitalWrite(nextPieceLed_1, nextPieceLedStatus[1]);
    digitalWrite(nextPieceLed_2, nextPieceLedStatus[2]);

    // sets the counter limit (game frequency control))
    if (gameTimeStep == getTimeEnd()) {
        gameTimeStep = 0x00;
    } else {
        gameTimeStep++;
    }

    // sets the go down flag to true
    if (gameTimeStep == 0x00 && isGamePaused() == 0x00) {
        goDownFlag = 0x01;
    }

    MidiCLKTrigger();               // triggs the MIDIPlayer

    interrupts();                   // enable all interrupts

    if (isGamePaused() == 0x00) {
        updateDisplay(getField());  // updates field if game is not paused
    }
}

/*
 * Sets the microcontroller pin mode, matrix LCD,
 * timers and starts the game
 */
void setup() {

    Serial.begin(9600);

    // LED matrix SETUP
    pinMode(ledMatrixCLK, OUTPUT);          // clock pin
    pinMode(ledMatrixCS, OUTPUT);           // load pin
    pinMode(ledMatrixDIN, OUTPUT);          // data in pin

    // switches setup
    pinMode(goRigthSwitchPin, INPUT);       // rigth switch
    pinMode(goDownSwitchPin, INPUT);        // down sitch
    pinMode(rotateSwitchPin, INPUT);        // rotate switch
    pinMode(goLeftSwitchPin, INPUT);        // left switch
    pinMode(pausePin, INPUT);               // pause switch

    // counterLed stup
    pinMode(counterLed_0, OUTPUT);          // counter LED #0
    pinMode(counterLed_1, OUTPUT);          // counter LED #1
    pinMode(counterLed_2, OUTPUT);          // counter LED #2
    pinMode(counterLed_3, OUTPUT);          // counter LED #3

    // nextPieceLed setup
    pinMode(nextPieceLed_0, OUTPUT);        // next piece LED #0
    pinMode(nextPieceLed_1, OUTPUT);        // next piece LED #1
    pinMode(nextPieceLed_2, OUTPUT);        // next piece LED #2

    setSpeakerPinNum(speakerPin);           // speaker pin

    // wait 0.05 seconds to set and initializes LED matrix
    delay(50);
    setDisplayPins(ledMatrixCS, ledMatrixCLK, ledMatrixDIN);
    initDisplay(0x01);

    intro();                // start intro
    setDisplayBrite(0);
    startGame();            // start game

    // initialize timer1 
    noInterrupts();         // disable all interrupts
    TCCR1A = 0;             // sets Timer/Counter1 Control Register A to zero
    TCCR1B = 0;             // sets Timer/Counter1 Control Register B to zero


    // Set timer1_counter to the correct value for our interrupt interval
    timer1_counter = 64560; // preload timer 65536-16MHz/256/64Hz
    TCNT1 = timer1_counter; // preload timer
    TCCR1B |= (1 << CS12);  // 256 prescaler 
    TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt


    interrupts(); // enable all interrupts

    /* hardware hazard - not implemted yet */
    //attachInterrupt(0, getInput, RISING);     // hardware interrups

}

/*
 * reads the buttons status and sets the flags accordingly
 * it only changes the flag status if there is a change of button status to HIGH
 * to avoid repetitions
 * this function is only used by polling
 */
void getInput() {
    // go rigth witch
    if (digitalRead(goRigthSwitchPin) != lastGoRigthSwitchStatus) {
        if (lastGoRigthSwitchStatus == LOW) {
            lastGoRigthSwitchStatus = HIGH;
            goRigthFlag = 0x01;
            return;
        } else {
            lastGoRigthSwitchStatus = LOW;
        }
    }
    // go down switch
    if (digitalRead(goDownSwitchPin) != lastGoDownSwitchStatus) {
        if (lastGoDownSwitchStatus == LOW) {
            lastGoDownSwitchStatus = HIGH;
            goDownFlag = 0x01;
            return;
        } else {
            lastGoDownSwitchStatus = LOW;
        }
    }
    // rotate switch
    if (digitalRead(rotateSwitchPin) != lastRotateSwitchStatus) {
        if (lastRotateSwitchStatus == LOW) {
            lastRotateSwitchStatus = HIGH;
            rotateFlag = 0x01;
            return;
        } else {
            lastRotateSwitchStatus = LOW;
        }
    }
    // go left switch
    if (digitalRead(goLeftSwitchPin) != lastGoLeftSwitchStatus) {
        if (lastGoLeftSwitchStatus == LOW) {
            lastGoLeftSwitchStatus = HIGH;
            goLeftFlag = 0x01;
            return;
        } else {
            lastGoLeftSwitchStatus = LOW;
        }
    }
}

/* hardware hazard - not implemted yet */

/*
 * reads the buttons status and sets the flags accordingly
 * this function is only used by the hardware interrupt
 */
/*void getInput(){
    
        if(digitalRead(goRigthSwitchPin)== HIGH){
                goRigthFlag = 0x01;
        }
        if(digitalRead(goDownSwitchPin)== HIGH){
                goDownFlag = 0x01;
        }
        if(digitalRead(rotateSwitchPin)== HIGH){
                rotateFlag = 0x01;
        }
        if(digitalRead(goLeftSwitchPin)== HIGH){
                goLeftFlag = 0x01;
        }
}*/

/*
 * the loop event gets the buttons states and interacts 
 * with the game according to the button flags
 */
void loop() {

    /* hardware hazard - not implemted yet */
    getInput(); // buttons polling

    // go down check
    if (goDownFlag == 0x01) {
        goDownFlag = 0x00;
        goDown();
    }

    // go rigth check
    if (goRigthFlag == 0x01) {
        goRigthFlag = 0x00;
        goRigth();
    }

    // go left check
    if (goLeftFlag == 0x01) {
        goLeftFlag = 0x00;
        goLeft();
    }

    // rotate check
    if (rotateFlag == 0x01) {
        rotateFlag = 0x00;
        rotate();
    }
}
