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
 * This module generates music tones using the tone() library
 * and two arrays, one with the note definition and other with 
 * the time of play (1/time) seconds 
 */

#include <Arduino.h>
#include "MIDIPlayer.h"

int timeStep = 0;
int *noteArray;
int *timeArray;
int arraySize;
int nextNoteTimeRemain;
int actualNote;
int speakerPinNum;
int playFlag = 0;

/********* start level *********/

// notes in the melody:
int startLevelMelody[] = {
    NOTE_H3, NOTE_E4, NOTE_FS4, NOTE_GS4, NOTE_A4, NOTE_GS4, NOTE_CS5, NOTE_FS4, NOTE_A4, NOTE_GS4, NOTE_DS4, NOTE_A4, NOTE_G4, NOTE_E5, 0
};

// note durations (parts of a second)
int startLevelNoteDurations[] = {
    16, 16, 16, 8, 16, 16, 8, 16, 16, 16, 16, 16, 16, 2, 16
};

// array length
int startLevelLenght = 15;


/********* sound one *********/

// notes in the melody:
int soundOneMelody[] = {
    NOTE_E5, NOTE_E3, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_C5,
    NOTE_B4, NOTE_A4, NOTE_A3, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_A3, NOTE_D5,
    NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_E3, NOTE_E5,
    NOTE_E3, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_A3, NOTE_A4, NOTE_A3, NOTE_B2,
    NOTE_C3, NOTE_D3, NOTE_D5, NOTE_F5, NOTE_A5, NOTE_C5, NOTE_C5, NOTE_G5,
    NOTE_F5, NOTE_E5, NOTE_C3, 0, NOTE_C5, NOTE_E5, NOTE_A4, NOTE_G4, NOTE_D5,
    NOTE_C5, NOTE_B4, NOTE_E4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_G4, NOTE_E5,
    NOTE_G4, NOTE_C5, NOTE_E4, NOTE_A4, NOTE_E3, NOTE_A4, 0
};

// note durations (parts of a second)
int soundOneNoteDurations[] = {
    8, 8, 8, 8, 8, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 4, 8, 8, 16, 16, 8, 8, 8, 8, 8, 8, 8, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4
};

// array lenght
int soundOneLenght = 64;

/********* dancing *********/

// notes in the melody:
int dancingMelody[] = {
    NOTE_D4, NOTE_D4, NOTE_D4, NOTE_CS4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_D4,
    NOTE_D4, NOTE_CS4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_CS4, NOTE_D4, NOTE_D4,
    NOTE_D4, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_A4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_B4, NOTE_D4,
    0, NOTE_A4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_CS4, NOTE_D4,
    NOTE_F4, NOTE_A4, NOTE_CS5, NOTE_D5, 0, NOTE_D4
};

// note durations (parts of a second)
int dancingNoteDurations[] = {
    8, 4, 8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    4, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8, 4,
    4, 4, 4, 4, 4, 2
};

// array length
int dancingLenght = 51;

/*
 * sets the speaker pin
 * Params:
 * int pin      - speaker pin number
 */
void setSpeakerPinNum(int pin) {
    speakerPinNum = pin;
}

/*
 * plays a music
 * Params:
 * unsigned char music      - music number to play
 */
void playMusic(unsigned char music) {
    // the music value selects the reference of the note and 
    // time arrays and sets the length of the arrays to play
    
    stopMusic();
    switch (music) {
        case 0:
            noteArray = startLevelMelody;
            timeArray = startLevelNoteDurations;
            arraySize = startLevelLenght;
            break;
        case 1:
            //noteArray = endLevelMelody;
            //timeArray = endLevelNoteDurations;
            //arraySize = endLevelLenght;
            break;
        case 2:
            noteArray = dancingMelody;
            timeArray = dancingNoteDurations;
            arraySize = dancingLenght;
            break;
        case 3:
            noteArray = soundOneMelody;
            timeArray = soundOneNoteDurations;
            arraySize = soundOneLenght;
            break;
        default:;
    }
    
    playFlag = 1;
}

/*
 * plays one note
 * Params:
 * int note     - note to play (Hz)
 * inte time    - time to play (ms)
 */
void playNote(int note, int time) {
    stopMusic();
    tone(speakerPinNum, note, time);
}

/*
 * stop the actual music
 */
void stopMusic() {
    // just reset the iterator and the play flag
    actualNote = 0;
    playFlag = 0;
}

/*
 * pause the actual music
 */
void pauseMusic() {
    // just sets the play flag to zero
    playFlag = !playFlag;
}

/*
 * the module engine trigged every 1/64 seconds.
 */
void MidiCLKTrigger() {

    // the note and time arrays are iterated and played 
    // one by one. the time beetween each note is calculated 
    // with the last time duration played
    
    // plays if the play flag is set to 1 
    if (playFlag != 0) {
        // divides the frequency (16Hz))
        if (timeStep == 3) {
            timeStep = 0;
        } else {
            timeStep++;
        }

        if (timeStep == 0x00) {
            // resets the iterator
            if (actualNote == arraySize) {
                actualNote = 0;
                nextNoteTimeRemain = 0;
            }
            // if is time to play, play it
            if (nextNoteTimeRemain == 0) {
                // plays the note on speaker pin
                tone(speakerPinNum, noteArray[actualNote], 1000 / timeArray[actualNote]);
                // canculates the next note time remain
                nextNoteTimeRemain = 16 / timeArray[actualNote];
                actualNote++;
            } else {
                nextNoteTimeRemain--;
            }
        }
    }
}
