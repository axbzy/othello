#include "player.hpp"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    //pos = new Board();
    sroot.pos.white = (1ull << (8*3 + 3)) | (1ull << (8*4 + 4));
    sroot.pos.black = (1ull << (8*3 + 4)) | (1ull << (8*4 + 3));
    sroot.pos.side = BLACK;
    /*for(int i = 0; i < 12; i += 2) {
        break;
        int k = scoreab(sroot, i, MINF, INF, 1);
        cerr << "Depth " << i << ": " << k << endl;
        cerr << table.size() << endl;
    }*/
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    for(auto it = sroot.children.begin(); 
             it != sroot.children.end(); it++) {
        prune(*it);
    }
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
    start = system_clock::now();
    int nstones = __builtin_popcountll(sroot.pos.black) 
                + __builtin_popcountll(sroot.pos.white);
    milliseconds total(msLeft);
    overall = microseconds(total);
    if(nstones != 46) early = (overall) / (46 - nstones);
    if(nstones == 46 || early.count() < 0) early = (3 * overall) / 4;
    late = overall / 2;
    vector<sBoardNode*>::iterator it, end;
    sBoard desired;
    if(penultimate_move != -1) {
        end = sroot.children.end();
        for(it = sroot.children.begin(); it != end; it++) {
            desired = do_move(sroot.pos, penultimate_move);
            if(!((*it)->pos == desired)) {
                prune(*it);
                *it = nullptr;
            }
        }
        for(it = sroot.children.begin(); it != end; it++) {
            if(*it != nullptr) {
                sroot = *(*it);
                break;
            }
        }
    }
    unsigned char last_move = 64;
    if(opponentsMove != nullptr) {
        last_move = 8*opponentsMove->x + opponentsMove->y;
    }
    if( !(sroot.pos == opening) || last_move != 64 ) {
        if(sroot.children.size() == 0) deepen_eval(sroot);
        end = sroot.children.end();
        desired = do_move(sroot.pos, last_move);
        for(it = sroot.children.begin(); it != end; it++) {
            if(!((*it)->pos == desired)) {
                prune(*it);
                *it = nullptr;
            }
        }
        for(it = sroot.children.begin(); it != end; it++) {
            if(*it != nullptr) {
                sroot = *(*it);
                break;
            }
        }
    }

    unsigned char m = 0;
    int desired_depth = 8, oldk = 0, k;
    if(nstones >= 4 && nstones <= 42) desired_depth = 6;
    desired_depth = 8;
    if(nstones >= 46) desired_depth = 66 - nstones;
    if(desired_depth >= 12 && desired_depth <= 21) target = late;
    else target = early;
    cerr << "Target time: " << target.count() << "us" << endl;
    if(target.count() > (long)msLeft * 1000l) {
        milliseconds timeLeft(msLeft);
        target = microseconds(timeLeft);
    }
    for(int depth = 0; depth <= desired_depth; depth += 2) {
        table.clear();
        k = scoreab(sroot, depth, MINF, INF, 1);
        if(overflow) {
            k = oldk;
            cerr << "Overflow!" << endl;
        } else {
            m = sroot.children[sroot.best_move_index]->last_move;
        }
        oldk = k;
        system_clock::time_point now = system_clock::now();
        microseconds us = duration_cast<microseconds>(now - start);
        cerr << "Depth " << depth << ": " << k 
            << "\tTable size: " << table.size() 
            << "\tTree size: " << n_nodes
            << "\tTime taken: " << us.count() << "us" << endl;
        if(k == INF || overflow) break;
        if(depth + 2 > desired_depth && us.count() * 10 < target.count()
           && desired_depth + nstones <= 66) {
            desired_depth += 2;
        }
    }
    cerr << endl;
    overflow = false;
    penultimate_move = m;
    Move *move = nullptr;
    if(m != 64) {
        move = new Move(m / 8, m % 8);
    }
    return move;
}


int Player::rawminimax(sBoardNode &node, int desired_depth, int side_multiplier) {
    if(desired_depth == 0) return (side_multiplier * deepen_eval(node));
    int best = MINF, current;
    if(node.children.size() == 0) deepen_eval(node);
    for(unsigned int i = 0; i < node.children.size(); i++) {
        current = -rawminimax(*(node.children[i]), desired_depth - 1,
                -side_multiplier);
        if(current > best) {
            best = current;
            node.best_move_index = (unsigned char)i;
        }
    }
    return best;
}

int Player::rawnegamax(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    if(desired_depth == 0) return (side_multiplier * deepen_eval(node));
    int best = MINF, current;
    if(node.children.size() == 0) deepen_eval(node);
    for(unsigned int i = 0; i < node.children.size(); i++) {
        current = -rawnegamax(*(node.children[i]), desired_depth - 1, 
                              -b, -a, -side_multiplier);
        if(current > best) {
            best = current;
            a = max(a, current);
            node.best_move_index = (unsigned char)i;
            if(a >= b) break;
        }
    }
    return best;
}

int Player::negamax0(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    if(overflow) return 0;
    auto it = table.find(node.pos);
    Entry en;
    if(it != table.end() && desired_depth == 0) {
        return table[node.pos].score;
    }
    if(desired_depth == 0) {
        en.type = 0;
        en.depth = 0;
        en.score = side_multiplier * deepen_eval(node);
        table[node.pos] = en;
        if(table.size() > TABLE_CUTOFF) {
            overflow = true;
            return 0;
        }
        return en.score;
    }
    int best = MINF, current;
    if(node.children.size() == 0) deepen_eval(node);
    for(unsigned int i = 0; i < node.children.size(); i++) {
        current = -negamax0(*(node.children[i]), desired_depth - 1, -b, -a,
                -side_multiplier);
        if(current > best) {
            best = current;
            a = max(a, current);
            node.best_move_index = (unsigned char)i;
        }
        if(a >= b) break;
    }
    return best;
}

int Player::scoreab(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    if(overflow || table.size() > TABLE_CUTOFF) {
        overflow = true;
        return 0;
    }
    system_clock::time_point now = system_clock::now();
    microseconds taken = duration_cast<microseconds>(now - start);
    if(target.count() > 0 &&
       taken.count() * 100l > target.count() * 95l) {
        overflow = true;
        return 0;
    }
    int original_alpha = a;
    auto it = table.find(node.pos);
    Entry en;
    if(it != table.end() && (en = it->second).depth >= desired_depth) {
        if(en.type == 0) return en.score;
        else if(en.type == (unsigned char)(-1)) a = max(a, en.score);
        else b = min(b, en.score);
        if (a >= b) return en.score;
    }
    if(desired_depth == 0) {
        en.type = 0;
        en.depth = 0;
        en.score = side_multiplier * deepen_eval(node);
        table[node.pos] = en;
        return en.score;
    }
    int best = MINF, current;
    if(node.children.size() == 0) deepen_eval(node);
    for(unsigned int i = 0; i < node.children.size(); i++) {
        current = -scoreab(*(node.children[i]), desired_depth - 1, -b, -a,
                -side_multiplier);
        if(overflow) return 0;
        if(current > best) {
            best = current;
            a = max(a, current);
            node.best_move_index = (unsigned char)i;
        }
        if(a >= b) break;
    }
    en.score = best;
    if(best <= original_alpha) en.type = 1;
    else if(best >= b) en.type = -1;
    else en.type = 0;
    en.depth = desired_depth;
    table[node.pos] = en;
    return best;
}

int Player::scoreabN(sBoard pos, int desired_depth, int a, int b, 
        int side_multiplier) {
    if(overflow || table.size() > TABLE_CUTOFF) {
        overflow = true;
        return 0;
    }
    int original_alpha = a;
    auto it = table.find(pos);
    Entry en;
    if(it != table.end() && (en = it->second).depth >= desired_depth) {
        if(en.type == 0) return en.score;
        else if(en.type == (unsigned char)(-1)) a = max(a, en.score);
        else b = min(b, en.score);
        if (a >= b) return en.score;
    }
    if(desired_depth <= 0) {
        en.type = 0;
        en.depth = 0;
        en.score = side_multiplier * eval(pos);
        table[pos] = en;
        return en.score;
    }
    int best = MINF, current;
    vector<unsigned char> moves;
    move_list(pos, moves);
    for(auto it = moves.begin(); it != moves.end(); it++) {
        current = -scoreabN(do_move(pos, *it), desired_depth - 1, -b, -a,
                            -side_multiplier);
        if(overflow) return 0;
        if(current > best) {
            best = current;
            a = max(a, current);
        }
        if(a >= b) break;
    }
    en.score = best;
    if(best <= original_alpha) en.type = 1;
    else if(best >= b) en.type = -1;
    else en.type = 0;
    en.depth = desired_depth;
    table[pos] = en;
    return best;
}

void Player::prune(sBoardNode *r) {
    if(r == nullptr) return;
    if(r->children.size() != 0) {
        for(unsigned int i = 0; i < r->children.size(); i++) {
            prune(r->children[i]);
        }
    }
    table.erase(r->pos);
    delete r;
    n_nodes--;
}

int Player::deepen_eval(sBoardNode &p) {
    bitset<64> us(p.pos.black), them(p.pos.white);
    int b = __builtin_popcountll(p.pos.black),
        w = __builtin_popcountll(p.pos.white),
        n_moves = 0, n_other_moves = 0, x, y, X, Y, ours, theirs;
    ours = (p.pos.side == BLACK) ? b : w;
    theirs = (p.pos.side == WHITE) ? b : w;
    if(ours == 0) return MINF;
    if(theirs == 0) return INF;
    unordered_set<unsigned char> move_list;

    if(p.pos.side == WHITE) {
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
                        move_list.insert(move);
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

    p.children.reserve(max(1, n_moves));
    if(n_moves == 0) {
        sBoardNode *child = new sBoardNode();
        n_nodes++;
        child->pos = p.pos;
        child->pos.side = OTHER(child->pos.side);
        child->last_move = 64;
        child->children.shrink_to_fit();
        p.children.push_back(child);
    }
    for(auto it = move_list.begin(); it != move_list.end(); it++) {
        sBoardNode *child = new sBoardNode();
        n_nodes++;
        if(n_nodes > NODE_CUTOFF) {
            overflow = true;
            return 0;
        }
        child->pos = do_move(p.pos, *it);
        child->last_move = *it;
        child->children.shrink_to_fit();
        p.children.push_back(child);
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

    /*int desired_depth;
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
    
    //pos->doMove(m, side);
    //return nullptr;*/
        /*else {
            k = MINF;
            vector<unsigned char> v;
            move_list(sroot.pos, v);
            for(auto it = v.begin();
                     it != v.end(); it++) {
                int current = -scoreabN(do_move(sroot.pos, *it), depth - 1,
                                        MINF, INF, -1);
                if(current > k) {
                    k = current;
                    m = *it;
                }
            }
        }*/
        //cerr << (int)sroot.best_move_index << endl;
        //cerr << "Table size: " << table.size() << " Allocated: " << n_nodes << endl;
