#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
using namespace std;

class Player {

    Board *pos;
    Side side;
    BoardNode *root;
    sBoardNode sroot;
    bool overflow = false;
    int n_nodes = 0;
    const sBoard opening = { (1ull << (8*3 + 3)) | (1ull << (8*4 + 4)), 
                             (1ull << (8*3 + 4)) | (1ull << (8*4 + 3)), BLACK };
    unordered_map<sBoard, Entry, BoardHash> table;

    Move* random_move(Board *pos);

    int scoreab(sBoardNode &node, int desired_depth, int a, int b,
            int side_multiplier);
    int rawnegamax(sBoardNode &node, int desired_depth, int a, int b,
            int side_multiplier);
    int negamax0(sBoardNode &node, int desired_depth, int a, int b,
            int side_multiplier);
    int rawminimax(sBoardNode &node, int desired_depth, int side_multiplier);
    int deepen_eval(sBoardNode &p);
    void prune(sBoardNode *r);

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    void setpos(Board *pos);

};

#endif
