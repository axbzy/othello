#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include "common.hpp"
using namespace std;

struct sBoard {
    unsigned long long int white, black;
    Side side;
    bool operator==(const sBoard& other) const {
        return (white == other.white && black == other.black
                && side == other.side);
    }
};

struct BoardHash {
    size_t operator()(sBoard const& board) const {
        size_t hash1 = hash<unsigned long long int>{}(board.white);
        size_t hash2 = hash<unsigned long long int>{}(board.black);
        size_t hash3 = (size_t) (board.side == BLACK);
        return hash1 ^ (hash2 << 1) ^ (hash3 << 2);
    }
};

struct Entry {
    int score;
    unsigned char depth, type;
};

struct sBoardNode {
    unsigned char last_move, best_move_index;
    sBoard pos;
    vector<sBoardNode*> children;
};

int eval(sBoard pos);
void move_list(sBoard pos, vector<unsigned char> &v);
sBoard do_move(sBoard pos, unsigned char move);
void disp(sBoard pos);



#endif
