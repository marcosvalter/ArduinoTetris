ArduinoTetris
=============

This is a small programming project just for fun
You may freely use and modify this code, NO WARRANTY, blah blah blah,
as long as you give proper credit to the original and subsequent authors.

Date:        Author:
2014-04-09   Marcos Valter Vidal Russo Alegre <m.valter@ua.pt>

Abstract:

Tetris is a very popular video game designed and programmed by Alexey Pajitnov back in the early 80's.
In this implementation the game is represented by a 16x8 LED matrix in one color. There are 16 levels with variations of dificulty and speed. some annimations and music are also performed during the game.
The game is written in C++ and uses an Arduino Nano microcontroller, two 8x8 LED Matrix (monocollor), two MAXIM 7219 7SEG controller, four press switches and a buzzer speaker.

Implementation:

Due a lack of RAM on the Arduino Nano (2048 bytes), the game has a fixed field size of 16 lines x 8 collumns represented by 16 lenght char array (display field) plus 20 lenght char array (game field) and 112 lenght char array for all pieces in all positions. each char represents a line of the field and each bit sets the LED to ON or OFF.

The game field contains the 16 display field lines plus two lines on top for playability and two on button for collision test purpose. The state of this field is set by the game itself (goDown, speed, difficultie, etc) and by the player (goLeft, goDown, goLeft and rotate). The display field is a copy of the game field minus the two top and two button lines and refreshed every 1/16 seconds (16Hz) only when the game is not paused. The game pause state is only used to stop the display refresh, allowing annimations beetween the levels.

This game plays monophonic tones using the tone() function of Arduino. it generates a square wave of the specified frequency (and 50% duty cycle) on a specified pin. Intead of using the "normal" delay() function to set the time beetween notes a clock trigger and a counter that redefines itself is used allowing to play the game and the music "simultaneously".

All modules are designed from scratch including the MAXIME 7219 display driver.



I'd like to thank to all who supported me including:

- My mates Aléxio Simões and José Moreira for all the C language and Linux support;
- My teachers Miguel Oliveira e Silva, Tomás Oliveira e Silva, João Rodrigues and Arnaldo Oliveira for all the know-how and headaches during the exams;
- Mr José Vieira Simões for all technical and moral suport given :)
