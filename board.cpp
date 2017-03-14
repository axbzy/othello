#include "board.hpp"

sBoard do_move(sBoard pos, unsigned char move) {
    if(move == 64) {
       pos.side = OTHER(pos.side);
       return pos;
    }

    int X = move / 8, Y = move % 8, x, y;
    unsigned long long flipped = 0ull;
    bitset<64> us(pos.black), them(pos.white);
    if(pos.side == WHITE) {
        us ^= them;
        them ^= us;
        us ^= them;
    }

    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            if(dy == 0 && dx == 0) continue;
            x = X;
            y = Y;
            do {
                x += dx;
                y += dy;
            } while(in(x, y) && them[8*x + y]);

            if(in(x, y) && us[8*x + y]) {
                x = X;
                y = Y;
                flipped |= (1ull << (8*x + y));
                x += dx;
                y += dy;
                while(them[8*x + y]) {
                    flipped |= (1ull << (8*x + y));
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    if(pos.side == BLACK) {
        pos.black |= flipped;
        pos.white &= ~flipped;
    } else {
        pos.white |= flipped;
        pos.black &= ~flipped;
    }
    pos.side = OTHER(pos.side);
    return pos;
}


void disp(sBoard pos) {
    cerr << ((pos.side == WHITE) ? "White" : "Black") << " to move:" << endl;
    for(int y = 0; y < 8; y++) {
        for(int x = 0; x < 8; x++) {
            if(pos.white & (1ull << (8*x + y))) cerr << 'O';
            else if(pos.black & (1ull << (8*x + y))) cerr << '8';
            else cerr << '-';
        }
        cerr << endl;
    }
}

int eval(sBoard pos) {
    bitset<64> us(pos.black), them(pos.white);
    int b = __builtin_popcountll(pos.black),
        w = __builtin_popcountll(pos.white),
        n_moves = 0, n_other_moves = 0, x, y, X, Y, ours, theirs;
    ours = (pos.side == BLACK) ? b : w;
    theirs = (pos.side == WHITE) ? b : w;
    if(ours == 0) return MINF;
    if(theirs == 0) return INF;

    if(pos.side == WHITE) {
        us ^= them;
        them ^= us;
        us ^= them;
    }
    unsigned long long ourull = us.to_ullong(), theirull = them.to_ullong();

    for(unsigned char move = 0; move < 64; move++) {
        if(us[move] || them[move]) continue;
        X = move / 8;
        Y = move % 8;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dy == 0 && dx == 0) continue;
                // Is there a capture in that direction?
                x = X + dx;
                y = Y + dy;
                if (in(x, y) && them[8*x + y]) {
                    do {
                        x += dx;
                        y += dy;
                    } while (in(x, y) && them[8*x + y]);
                    if (in(x, y) && us[8*x + y]) {
                        n_moves++;
                    }
                }
                if (in(x, y) && us[8*x + y]) {
                    do {
                        x += dx;
                        y += dy;
                    } while (in(x, y) && us[8*x + y]);
                    if (in(x, y) && them[8*x + y]) {
                        n_other_moves++;
                    }
                }
            }
        }
    }

    if(n_moves == 0 && n_other_moves == 0) {
        if(ours > theirs) return INF;
        if(ours < theirs) return MINF;
        return 0;
    }
    int stones = (STONES * (ours - theirs)) / (ours + theirs),
        mobility = (MOBILITY * max(ENDM - w - b, 0) 
                 * (n_moves - n_other_moves)) / (n_moves + n_other_moves),
        board_heur = 0;
    board_heur += __builtin_popcountll(ourull & CENTER_MASK)
           - 15 * __builtin_popcountll(ourull & RING_MASK)
           + 30 * __builtin_popcountll(ourull & EDGE_MASK)
           - 100 * __builtin_popcountll(ourull & DANGER_MASK)
           + 300 * __builtin_popcountll(ourull & CORNER_MASK);
    board_heur -= __builtin_popcountll(theirull & CENTER_MASK)
           - 15 * __builtin_popcountll(theirull & RING_MASK)
           + 30 * __builtin_popcountll(theirull & EDGE_MASK)
           - 100 * __builtin_popcountll(theirull & DANGER_MASK)
           + 300 * __builtin_popcountll(theirull & CORNER_MASK);
    board_heur *= HEUR * max(ENDH - w - b, 0);
    return (stones + mobility + board_heur);
}

void move_list(sBoard pos, vector<unsigned char> &v) {
    bitset<64> us(pos.black), them(pos.white);
    int x, y, X, Y;

    if(pos.side == WHITE) {
        us ^= them;
        them ^= us;
        us ^= them;
    }

    for(unsigned char move = 0; move < 64; move++) {
        if(us[move] || them[move]) continue;
        X = move / 8;
        Y = move % 8;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dy == 0 && dx == 0) continue;
                // Is there a capture in that direction?
                x = X + dx;
                y = Y + dy;
                if (in(x, y) && them[8*x + y]) {
                    do {
                        x += dx;
                        y += dy;
                    } while (in(x, y) && them[8*x + y]);
                    if (in(x, y) && us[8*x + y]) {
                        v.push_back(move);
                    }
                }
            }
        }
    }
}
