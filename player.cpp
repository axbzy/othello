#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    this->side = side;
    pos = new Board();
    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    delete pos;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {

    pos->doMove(opponentsMove, OTHER(side));

    Move *m;
    //m = pos->best_move(side, 4, 2);

    int desired_depth;
    desired_depth = (testingMinimax) ? 2 : 2;
    root = new BoardNode(pos, side, nullptr, nullptr, 0, testingMinimax);
    int comp = 0;
    for(int depth = 1; depth <= desired_depth; depth++) {
        stack<BoardNode*> s;
        s.push(root);
        while(!s.empty()) {
            BoardNode *top = s.top();
            s.pop();
            //cerr << (comp++) << endl;
            if(top == nullptr || top->depth == desired_depth) continue;
            top->add_children();
            for(auto it = top->children.begin(); it != top->children.end();
                    it++) {
                s.push(*it);
            }
        }
    }
    root->update_score();
    m = root->response;
    //delete root;
    
    pos->doMove(m, side);
    return m;
}


Move* Player::random_move(Board *pos) {
    int n = 0;
    unordered_map<int, int> moves;
    for(int i = 0; i < 64; i++) {
        if(pos->checkMove(new Move(i / 8, i % 8), side)) {
            moves.insert({{n++, i}});
        }
    }
    if(n == 0) return nullptr;
    srand(time(nullptr));
    int r = rand() % n, m = moves[r];
    return new Move(m / 8, m % 8);
}

void Player::setpos(Board *pos) {
    this->pos = pos;
}
