#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <unordered_map>
#include <stack>
#include <vector>

using namespace std;

enum Side { 
    WHITE, BLACK
};

#define OTHER(x) (x == BLACK) ? WHITE : BLACK

class Move {
   
public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

#endif
