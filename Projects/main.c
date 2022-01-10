#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#include "WIN.h"
#include "LOSE.h"
#include "empieza.h"
#include "ballz.h"

enum gba_state {
  START,
  PLAY,
  WINS,
  LOSES,
};

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;
  // reset variable
  int reset = 0;
  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;
  struct state cur, past;
  struct pongBall *gameBall, *outBall;
  struct paddle *userPad, *oldUserPAD;
  // Load initial application state
  enum gba_state state = START;
  waitForVBlank();
  //fillScreenDMA(BLUE);
  drawFullScreenImageDMA(empieza);
  int score = 0;
  int highScore = 0;
  int preHighest = 0;

  while (1) {

    currentButtons = BUTTONS; // Load the current state of the buttons
    past = cur;

    switch (state) {
    case START:
      if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
        state = PLAY;
        //initiate ball
        int rowStart = randint(20, 130);
        int colStart = randint(20, 110);
        gameBall = &cur.balls;
        gameBall->size = 5;
        gameBall->row = rowStart;
        gameBall->col = colStart;
        gameBall->rdis = 2;
        gameBall->cdis = 2;
        gameBall->color = RED;
        //initiate user paddle
        userPad = &cur.paddleU;
        userPad->length = 4;
        userPad->width = 20;
        userPad->row = 3;
        userPad->col = 3;
        userPad->rdis = 1;
        userPad->color = BLUE;

        waitForVBlank();
        fillScreenDMA(BLACK);
        drawRectDMA(gameBall->row, gameBall->col, gameBall->size, gameBall->size, gameBall->color);
        drawRectDMA(userPad->row, userPad->col, userPad->length, userPad->width, userPad->color);
      }
      break;
    case PLAY:
      // check if user wants out
      if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
        reset = 1;
        break;
      }
      // update ball
      gameBall = &cur.balls;
      userPad = &cur.paddleU;

      int lastRow = gameBall->row;
      int lastCol = gameBall->col;
      gameBall->row += gameBall->rdis;
      gameBall->col += gameBall->cdis;
      if (gameBall->col > (WIDTH-gameBall->size)) {
        gameBall->col = (WIDTH-gameBall->size);
        gameBall->cdis = -(gameBall->cdis);
        score += 1;
        if (score > highScore) {
          preHighest = highScore;
          highScore = score;
        }
      }
      if (gameBall->col < 0) { // Move to LOSE
        gameBall->col = 0;
        waitForVBlank();
        drawFullScreenImageDMA(LOSE);
        drawString(60, 93, "You Lose", RED);
        char buffer[51];
        sprintf(buffer, "Final Score: %d", score);
        drawString(80, 70, buffer, RED);
        sprintf(buffer, "Highest Score: %d", highScore);
        drawString(90, 70, buffer, RED);
        drawString(150, 127, "Press Up To Replay", RED);
        state = LOSES;
        break;
      }
      if (score == 10) {
        waitForVBlank();
        drawFullScreenImageDMA(WIN);
        drawString(10, 90, "You Win!", RED);
        char buffer[51];
        sprintf(buffer, "Final Score: %d", score);
        drawString(20, 70, buffer, RED);
        drawString(30, 65, "Congratulations!", RED);
        drawString(150, 127, "Press Up To Replay", RED);
        state = WINS;
        break;
      }
      if (gameBall->row > HEIGHT) {
        gameBall->row = HEIGHT;
        gameBall->rdis = -(gameBall->rdis);
      }
      if (gameBall->row < 0) {
        gameBall->row = 0;
        gameBall->rdis = -(gameBall->rdis);
      }
      // update user paddle
      if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
        userPad->row += userPad->rdis;
      }
      if (KEY_DOWN(BUTTON_UP, currentButtons)) {
        userPad->row -= userPad->rdis;
      }
      if (userPad->row > (HEIGHT-userPad->width)) {
        userPad->row = (HEIGHT-userPad->width);
      }
      if (userPad->row < 0) {
        userPad->row = 0;
      }
      //check if ball hit paddle
      if ((gameBall->col == (userPad->col + userPad->length)) &&
      (((gameBall->row + gameBall->size) >= userPad->row) && (gameBall->row <=
       (userPad->row + (userPad->width/2))))) { // update r
        gameBall->row  = lastRow;
        gameBall->col = lastCol;
        gameBall->rdis = -(gameBall->rdis);
        gameBall->cdis = -(gameBall->cdis);
      }
      if ((gameBall->col == (userPad->col + userPad->length)) &&
      (((gameBall->row + gameBall->size) >= (userPad->row + (userPad->width/2)))
       && (gameBall->row <= (userPad->row + userPad->width)))) {// dont update r
        gameBall->row  = lastRow;
        gameBall->col = lastCol;
        gameBall->cdis = -(gameBall->cdis);
      }
      // draw all
      waitForVBlank(); //drawImageDMA(15, 15, BALLZ_WIDTH, BALLZ_HEIGHT, ballz);
      // draw ball
      outBall = &past.balls;
      drawRectDMA(outBall->row, outBall->col, outBall->size, outBall->size, BLACK);
      gameBall = &cur.balls;
      drawRectDMA(gameBall->row, gameBall->col, gameBall->size, gameBall->size, gameBall->color);
      // draw user paddle
      oldUserPAD = &past.paddleU;
      drawRectDMA(oldUserPAD->row, oldUserPAD->col, oldUserPAD->length, oldUserPAD->width, BLACK);
      userPad = &cur.paddleU;
      drawRectDMA(userPad->row, userPad->col, userPad->length, userPad->width, userPad->color);
      // draw score
      char buffer[51];
      sprintf(buffer, "Your Score: %d", score - 1);
      drawString(2, 75, buffer, BLACK);
      sprintf(buffer, "Your Score: %d", score);
      drawString(2, 75, buffer, RED);
      sprintf(buffer, "Highest Score: %d", preHighest);
      drawString(HEIGHT-10, 75, buffer, BLACK);
      sprintf(buffer, "Highest Score: %d", highScore);
      drawString(HEIGHT-10, 75, buffer, RED);
      break;
    case WINS:
      if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
        reset = 1;
        break;
      }
      if (KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          state = START;
          waitForVBlank();
          drawFullScreenImageDMA(empieza);
          score = 0;
          break;
      }
      break;
    case LOSES:
      if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
        reset = 1;
        break;
      }
      if (KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          state = START;
          waitForVBlank();
          drawFullScreenImageDMA(empieza);
          score = 0;
          break;
      }
      break ;
    }
    if (reset) {
      break;
    }
    previousButtons = currentButtons;
  }
  main();
  return 0;
}

