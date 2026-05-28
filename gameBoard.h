#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "boat.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <vector>
using namespace std;

#define GRID_SIZE 8

class GameBoard
{
public:
    GameBoard(uint8_t pin); // constructer -- takes in pin number
    ~GameBoard(); // deconstructer 
    
    Adafruit_NeoMatrix *matrix; // neomatrix object

    // helper functions
    void clearBoard();
    int getBoard(int row, int col); 
    bool canPlaceBoat();
    void placeBoat(const Boat &boat);
    void loadNextBoat();
    void drawBoardAndPreview();
    void flashColor(uint16_t color, int ms);
    bool checkHit2(int hitRow, int hitCol);
    void reset();
    
    // getter functions 
    bool getPlacementBool();
    Boat& getBoat();
    bool getPlaceHit();
    bool getDeadBoats();

    // setter functions
    void setBoatsPlaced(bool status);
    void setPlaceHit(bool status);

private:
    // private vars
    int boatSizes[4] = {4, 3, 2, 2};
    int currentBoatIndex = 0;
    Boat fullBoatList[4];
    Boat currentBoat;
    bool allBoatsPlaced = false;
    int board[GRID_SIZE][GRID_SIZE];
    bool placingHit = false;
    bool allDeadBoats = false;
};

#endif