#include "gameBoard.h"

/* contructer for gameboard class
*  PARAMS: uint8_t pin
*  HOW: uses the pin to construct a matrix object from the NeoMatrix and initialized
*       with pin that was passed through
*/
GameBoard::GameBoard(uint8_t pin){
    matrix = new Adafruit_NeoMatrix(
        8, 8, pin,
        NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
        NEO_GRB + NEO_KHZ800);
}

/* deconstructer */
GameBoard ::~GameBoard(){
    delete matrix;
}

/* clearing logical gameboard
*  HOW: setting up all the individual parts is set to 0
*/
void GameBoard::clearBoard(){
    for (int row = 0; row < GRID_SIZE; row++)
    {
        for (int col = 0; col < GRID_SIZE; col++)
        {
            board[row][col] = 0;
        }
    }
}

/* checking logical game board */
bool GameBoard::canPlaceBoat(){
    for (int i = 0; i < currentBoat.length; i++)
    {
        int r = currentBoat.row;
        int c = currentBoat.col;

        if (currentBoat.isHorizontal)
            c += i;
        else
            r += i;

        if (r < 0 || r >= GRID_SIZE || c < 0 || c >= GRID_SIZE)
            return false;

        if (board[r][c] != 0)
            return false;

        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++)
            {
                int nr = r + dr;
                int nc = c + dc;

                if (nr >= 0 && nr < GRID_SIZE && nc >= 0 && nc < GRID_SIZE)
                {
                    if (board[nr][nc] != 0)
                        return false;
                }
            }
        }
    }

    return true;
}

/* place boat and setting logical board */
void GameBoard::placeBoat(const Boat &boat){
    for (int i = 0; i < boat.length; i++){
        int r = boat.row;
        int c = boat.col;

        if (boat.isHorizontal) c += i;
        else r += i;
        board[r][c] = 1;
    }
    fullBoatList[currentBoatIndex] = boat;
}

/* loading next boat until all boats are placed */
void GameBoard::loadNextBoat(){
    currentBoatIndex++;

    if (currentBoatIndex < 4){
        currentBoat = Boat(boatSizes[currentBoatIndex]);
    }
    else{
        allBoatsPlaced = true;
    }
}

/* draw logical board on physical board */
void GameBoard::drawBoardAndPreview(){
    matrix->fillScreen(0);

    //placed boats = blue
    for (int row = 0; row < GRID_SIZE; row++){
        for (int col = 0; col < GRID_SIZE; col++){
            if (board[row][col] == 1){ // where 1 represents a boat
                matrix->drawPixel(col, row, matrix->Color(0, 0, 255));
            }
            if(board[row][col] == 2){ // where 2 represents a where the other player hit
                matrix->drawPixel(col, row, matrix->Color(255, 0, 0));
            }
            if(board[row][col] == 3){ // where 3 represents where the other player missed
                matrix->drawPixel(col, row, matrix->Color(255, 255, 0));
            }
        }
    }

    // curser for when choosing a boat
    if (!allBoatsPlaced){
        for (int i = 0; i < currentBoat.length; i++){
            int row = currentBoat.row + (currentBoat.isHorizontal ? 0 : i);
            int col = currentBoat.col + (currentBoat.isHorizontal ? i : 0);

            matrix->drawPixel(col, row, matrix->Color(0, 245, 220));
        }
    }

    matrix->show();
}

/* flash color function for response to the players input */
void GameBoard::flashColor(uint16_t color, int ms){
    matrix->fillScreen(color);
    matrix->show();
    delay(ms);
}

/* checking if the target hit is a hit or not */
bool GameBoard::checkHit2(int hitRow, int hitCol){
    if(board[hitRow][hitCol] == 1){ // checking if there is a boat on the logical board 
        board[hitRow][hitCol] = 2; // set the boat piece to a hit state

        /* checking which of the hit affects which boat's life */
        for(int i = 0; i < 4; i++){
            Boat &b = fullBoatList[i];
            bool hit = false;
            if (b.isHorizontal) {
                if (hitRow == b.row && hitCol >= b.col && hitCol < b.col + b.length){
                    hit = true;
                }
            } else {
                if (hitCol == b.col && hitRow >= b.row && hitRow < b.row + b.length){
                    hit = true;
                } 
            }
            if (hit) {
                b.decreaseAlive();
                break;
            }
        }

        /* checking if all the boats are dead within a board */
        bool tempAllDead = true;
        for (int i = 0; i < 4; i++) {
            if (fullBoatList[i].getNumAlive() > 0) { 
                tempAllDead = false;
                break;
            }
        }
        allDeadBoats = tempAllDead;
        return true;
    } else{
        board[hitRow][hitCol] = 3;
    } 
    return false;
}


/* returns bool of if all the boats are placed */
bool GameBoard::getPlacementBool(){
    return allBoatsPlaced;
}

/* returns a boat, the current boat being worked on*/
Boat& GameBoard::getBoat(){
    return currentBoat;
}

/* returns the logical board */
int GameBoard::getBoard(int row, int col)
{
    return board[row][col];
}

/* returns the bool if the player already chose a hit */
bool GameBoard::getPlaceHit(){
    return placingHit;
}

/* returns bool if all the boats on a board are dead*/
bool GameBoard::getDeadBoats(){
    return allDeadBoats;
}

/* setting the status of if all the boats got placed */
void GameBoard::setBoatsPlaced(bool status){
    allBoatsPlaced = status;
}

/* setting the status if the player is choosing to hit */
void GameBoard::setPlaceHit(bool status){
    placingHit = status;
}


void GameBoard::reset(){
    clearBoard();
    allBoatsPlaced = false;
    allDeadBoats = false;
    currentBoatIndex = 0;
    placingHit = false;
    currentBoat = Boat(boatSizes[0]);
}
