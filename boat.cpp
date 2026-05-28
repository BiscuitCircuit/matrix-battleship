// boat.cpp
#include "boat.h"
using std::vector;

Boat::Boat(int len){
    length = len;
    row = 0;
    col = 0;
    isPlaced = false;
    isHorizontal = true;
    numAlive = length;
}


void Boat::rotate(){
    isHorizontal = !isHorizontal;
}

bool Boat::canFit() const{
    if (isHorizontal){
        return (col + length - 1 < 8);
    }
    else{
        return (row + length - 1 < 8);
    }
}

int Boat::getNumAlive(){
    return numAlive;
}

int Boat::decreaseAlive(){
    return numAlive--;
}