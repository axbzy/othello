#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include "common.hpp"
using namespace std;

class Board {

private:
    bitset<64> black;
    bitset<64> taken;

    bool occupied(int x, int y);
    bool get(Side side, int x, int y);
    void set(Side side, int x, int y);
    bool onBoard(int x, int y);

public:
    Board();
    ~Board();
    Board *copy();

    bool isDone();
    bool hasMoves(Side side);
    bool checkMove(Move *m, Side side);
    void doMove(Move *m, Side side);
    int count(Side side);
    int countBlack();
    int countWhite();
    int count_moves(Side side);

    int basic_heuristic(int corner, int edge, Side side);
    Move* best_move(Side side, int corner, int edge);

    void setBoard(char data[]);
};

class BoardNode {
private:
    void free_children();
public:
    bool testminimax;
    int score, depth;
    Side side;
    Move *last, *response;
    Board *pos;
    BoardNode *parent;
    vector<BoardNode*> children;

    BoardNode(Board *pos, Side side, BoardNode *parent, Move *last,
            int depth, bool testminimax);
    ~BoardNode();

    void add_children();
    void update_score();

};

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

sBoard do_move(sBoard pos, unsigned char move);
void disp(sBoard pos);



#endif
