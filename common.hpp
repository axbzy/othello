#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <vector>

using namespace std;

enum Side { 
    WHITE, BLACK
};

#define OTHER(x) (x == BLACK) ? WHITE : BLACK
#define in(x, y) x >= 0 && x < 8 && y >= 0 && y < 8
#define INF (numeric_limits<int>::max() - 1)
#define MINF -INF
#define STONES 900
#define MOBILITY 300
#define CORNERS 2000
#define ENDM 42
#define ENDC 48
#define TABLE_CUTOFF 5000000
#define NODE_CUTOFF 5000000

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
