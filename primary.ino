/* libraries used */
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "buttons.h"
#include "boat.h"
#include "gameboard.h"

/* defining matrix led connections to board */
#define PIN1 D6 
#define PIN2 D5
#define GRID_SIZE 8

/* initializing GameBoards for players */
GameBoard p1Board = GameBoard(PIN1);
GameBoard p2Board = GameBoard(PIN2);

/* creating array to hold grid values when player is attacking */
int p1AttackedBoard[GRID_SIZE][GRID_SIZE]={0}; 
int p2AttackedBoard[GRID_SIZE][GRID_SIZE]={0};

/* creating states for the game
*   placeP1, placeP2: when players are choosing their boat placements
*   chooseP1, chooseP2: when players are choosing a place to hit
*   endGame: when game is done, able to restart
*/
unsigned char gameStates;
enum {placeP1, placeP2, chooseP1, chooseP2, endGame};


/* function declaration */
void handleButtonPlacement(GameBoard &currentBoard);
bool getHitSpot(int attacker[GRID_SIZE][GRID_SIZE], GameBoard &hunter, int &hitRow, int &hitCol);
bool checkHit(GameBoard &attacked, GameBoard &hunter, int attacker[GRID_SIZE][GRID_SIZE], int hitRow, int hitCol);

/* global variables for coordinates of where a player attacks */
int hitRow = 0;
int hitCol = 0;

/* runs once when the program gets uploaded */
void setup()
{
    Serial.begin(9600);

    /* initializing matrix */
    p1Board.matrix->begin();
    p2Board.matrix->begin();
    p1Board.matrix->setBrightness(20);
    p2Board.matrix->setBrightness(20);

    /* button initialization */
    buttons_init();

    /* clears out the logical board */
    p1Board.clearBoard();
    p2Board.clearBoard();

    /* clears out the led matrix */
    p1Board.matrix->fill(0);
    p2Board.matrix->fill(0);
    p1Board.matrix->show();
    p2Board.matrix->show();

    /* intiialize gameState */
    gameStates = placeP1;
}

/* looping through this code until not */
void loop(){
    buttons_update();
    /* state machine */
    switch(gameStates){
        case(placeP1):
            if(p1Board.getPlacementBool()){
                gameStates = placeP2;
            } else{
                gameStates = placeP1;
            }
            break;
        case(placeP2):
            if(p2Board.getPlacementBool()){
                gameStates = chooseP1;
            } else{
                gameStates = placeP2;
            }
            break;
        case(chooseP1):
            if(p1Board.getDeadBoats() ||  p2Board.getDeadBoats()){
                gameStates = endGame;
            } else if(p1Board.getPlaceHit()){
                p1Board.drawBoardAndPreview();
                p1Board.setPlaceHit(false);
                gameStates = chooseP2;
                hitRow = 0; hitCol = 0;
            } else{
                gameStates = chooseP1;
            }
            break;
        case(chooseP2):
            if(p1Board.getDeadBoats() ||  p2Board.getDeadBoats()){
                gameStates = endGame;
            }else if(p2Board.getPlaceHit()){
                p2Board.drawBoardAndPreview();
                p2Board.setPlaceHit(false);
                gameStates = chooseP1;
                hitRow = 0; hitCol = 0;
            } else{
                gameStates = chooseP2;
            }
            break;
        case(endGame):
            if(buttons_getEvent(ID_CONFIRM) == BTN_LONG_PRESS){
                p1Board.reset();
                p2Board.reset();

            for (int r = 0; r < GRID_SIZE; r++) {
                for (int c = 0; c < GRID_SIZE; c++) {
                    p1AttackedBoard[r][c] = 0;
                    p2AttackedBoard[r][c] = 0;
                }
            }
            p1Board.matrix->clear();
            p2Board.matrix->clear();
            gameStates = placeP1;
            }
    }

    /* state machine actions (not transitions)*/
    switch(gameStates){
        case(placeP1):
            handleButtonPlacement(p1Board);
            break;
        case(placeP2):
            handleButtonPlacement(p2Board);
            break;
        case(chooseP1):
            if(getHitSpot(p1AttackedBoard, p1Board, hitRow, hitCol)){
                if(!checkHit(p2Board, p1Board, p1AttackedBoard, hitRow, hitCol)){
                    p1Board.setPlaceHit(true);
                }
            } 
            break;
        case(chooseP2):
             if(getHitSpot(p2AttackedBoard, p2Board, hitRow, hitCol)){
                if(!checkHit(p1Board, p2Board, p2AttackedBoard, hitRow, hitCol)){
                    p2Board.setPlaceHit(true);
                }
            } 
            break;
        case(endGame):
            if(p1Board.getDeadBoats()){ //p1 loses 
                p1Board.matrix->fillScreen(p1Board.matrix->Color(100,0,0));
                p2Board.matrix->fillScreen(p2Board.matrix->Color(0,100,0));
                p1Board.matrix->show();
                p2Board.matrix->show();
            }
            if(p2Board.getDeadBoats()){ //p2 wins
                p2Board.matrix->fillScreen(p2Board.matrix->Color(100,0,0));
                p1Board.matrix->fillScreen(p1Board.matrix->Color(0,100,0));
                p1Board.matrix->show();
                p2Board.matrix->show();
            }
            break;
    }


}

/* handles the button placements and makes sure that all boats are placed
*   PARAMS: GameBoard &currentBoard
*           GameBoard &currentBoard: current player board
 */
void handleButtonPlacement(GameBoard &currentBoard){
    
    Boat &b = currentBoard.getBoat();
    if (!currentBoard.getPlacementBool()){
        if (buttons_getEvent(ID_UP) == BTN_SHORT_PRESS && b.row > 0) b.row--;
        if (buttons_getEvent(ID_DOWN) == BTN_SHORT_PRESS){
            if (b.isHorizontal && b.row < 7) b.row++;
            else if (!b.isHorizontal && b.row + b.length - 1 < 7) b.row++;
        }

        if (buttons_getEvent(ID_LEFT) == BTN_SHORT_PRESS && b.col > 0) b.col--;
        if (buttons_getEvent(ID_RIGHT) == BTN_SHORT_PRESS) {
            if (b.isHorizontal && b.col + b.length - 1 < 7) b.col++;
            else if (!b.isHorizontal && b.col < 7) b.col++;
        }

        // initializing confirm button
        ButtonEvent confirmEvent = buttons_getEvent(ID_CONFIRM);

        // short press = rotate
        if (confirmEvent == BTN_SHORT_PRESS){
            b.rotate();
            if (!b.canFit()){
                if (b.isHorizontal) b.col = 8 - b.length;
                else b.row = 8 - b.length;
            }
        }

        // long press = place boat if valid
        if (confirmEvent == BTN_LONG_PRESS){
            if (currentBoard.canPlaceBoat()){
                currentBoard.placeBoat(b); 
                currentBoard.flashColor(currentBoard.matrix->Color(0, 100, 0), 250);
                currentBoard.loadNextBoat();
            } else {
                currentBoard.flashColor(currentBoard.matrix->Color(100, 0, 100), 250);
            }
        }
    } else {
        // all boats from player is placed
        currentBoard.setBoatsPlaced(true);
        delay(400);
        return;
    }
    currentBoard.drawBoardAndPreview();
}

/* getting hit coordinate chose by a player
*   PARAMS: int attacker[GRID_SIZE][GRID_SIZE], GameBoard &hunter, int &hitRow, int &hitCol
*           int attacker[GRID_SIZE][GRID_SIZE] -- game logic board for the attacker 
*           GameBoard &hunter -- the player's attacking matrix
*           int &hitRow, int &hitCol -- the hit coordinates globals
*   RETURN: BOOL
*           return true if valid hit target
*           return false if invalid hit target           
*/
bool getHitSpot(int attacker[GRID_SIZE][GRID_SIZE], GameBoard &hunter, int &hitRow, int &hitCol){
    if (buttons_getEvent(ID_UP) == BTN_SHORT_PRESS && hitRow > 0) hitRow--;
    if (buttons_getEvent(ID_DOWN) == BTN_SHORT_PRESS && hitRow < 7) hitRow++;
    if (buttons_getEvent(ID_LEFT) == BTN_SHORT_PRESS && hitCol > 0) hitCol--;
    if (buttons_getEvent(ID_RIGHT) == BTN_SHORT_PRESS && hitCol < 7) hitCol++;
    ButtonEvent confirmEvent = buttons_getEvent(ID_CONFIRM);
    
    hunter.matrix->clear();

    // drawing logical game board onto the attacker game board
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (attacker[r][c] == 3) hunter.matrix->drawPixel(c, r, hunter.matrix->Color(255, 0, 0)); // Hit
            if (attacker[r][c] == 4) hunter.matrix->drawPixel(c, r, hunter.matrix->Color(255, 255, 0)); // Miss
        }
    }
    
    // cursor for choosing the hit
    hunter.matrix->drawPixel(hitCol, hitRow, hunter.matrix->Color(100, 0, 200));
    hunter.matrix->show();

    // checking to make sure the chosen hit target was not chosen before 
    if (confirmEvent == BTN_LONG_PRESS){
        if(attacker[hitRow][hitCol] == 3 || attacker[hitRow][hitCol] == 4){
            hunter.flashColor(hunter.matrix->Color(100,0,100), 100);
            return false;
        } else{
            hunter.flashColor(hunter.matrix->Color(0,100,0), 100);
            return true;
        }
    }
    return false;
}

/* checking the hit target from the attacked players board
*   PARAMS: GameBoard &attacked, GameBoard &hunter, int attacker[GRID_SIZE][GRID_SIZE], int hitRow, int hitCol
*           GameBoard &attacked, GameBoard &hunter: players boards
*           int attacker[GRID_SIZE][GRID_SIZE]: logical game board of the attacker 
*           int hitRow, int hitCol: global vars of hit target coords
*   RETURN: BOOL 
*           return true if the hit
*           return false if miss
*/
bool checkHit(GameBoard &attacked, GameBoard &hunter, int attacker[GRID_SIZE][GRID_SIZE], int hitRow, int hitCol){
    bool check, newTurn;
    check = attacked.checkHit2(hitRow, hitCol);
    if(check){
        attacker[hitRow][hitCol] = 3; // where 3 represents a hit
        newTurn = true;
    } else{
        attacker[hitRow][hitCol] = 4; // where 4 represents a miss
        newTurn = false;
    }

    // drawing on the attackers board
    hunter.matrix->clear();
    for (int row = 0; row < GRID_SIZE; row++){
        for (int col = 0; col < GRID_SIZE; col++){
            if (attacker[row][col] == 3){
                hunter.matrix->drawPixel(col, row, hunter.matrix->Color(255, 0, 0)); //hit
                attacked.drawBoardAndPreview();
            } else if(attacker[row][col] == 4){
                hunter.matrix->drawPixel(col, row, hunter.matrix->Color(255, 255, 0)); // miss
                attacked.drawBoardAndPreview();
            }
        }
    }
    hunter.matrix->show();
    delay(1000);
    return newTurn;
}