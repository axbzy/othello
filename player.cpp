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
    //pos->doMove(opponentsMove, OTHER(side));
    //unsigned char m;
    //m = pos->best_move(side, 4, 2);

    unsigned char o_move = 64;
    vector<sBoardNode*>::iterator it, end;
    if(opponentsMove != nullptr) {
        o_move = 8*opponentsMove->x + opponentsMove->y;
        //cerr << opponentsMove->x << " " << opponentsMove->y << endl;
    }
    if( !(sroot.pos == opening) || o_move != 64 ) {
        if(sroot.children.size() == 0) deepen_eval(sroot);
        //cerr << "Desired: " << endl;
        //disp(do_move(sroot.pos, o_move));
        end = sroot.children.end();
        for(it = sroot.children.begin(); it != end; it++) {
            //cerr << "Found: " << endl;
            //disp( (*it)->pos );
            //cerr << ((*it)->pos == do_move(sroot.pos, o_move)) << endl;
            if(!((*it)->pos == do_move(sroot.pos, o_move))) {
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
    //cerr << "Selected: " << endl;
    //disp(sroot.pos);
    //cerr << table.size() << endl;

    unsigned char m;
    int desired_depth = 8, oldk = 0, k;
    int nstones = __builtin_popcountll(sroot.pos.black) + __builtin_popcountll(sroot.pos.white);
    if(nstones >= 4 && nstones <= 42) desired_depth = 6;
    desired_depth = 8;
    if(nstones >= 46) desired_depth = 67 - nstones;
    for(int depth = 0; depth <= desired_depth; depth += 2) {
        table.clear();
        k = negamax0(sroot, depth, MINF, INF, 1);
        if(overflow) k = oldk;
        oldk = k;
        if(!overflow) m = sroot.children[sroot.best_move_index]->last_move;
        if(overflow) cerr << "Overflow!" << endl;
        cerr << "Depth " << depth << ": " << k 
            << "\tTable size: " << table.size() << "\tAllocated tree nodes: " 
            << n_nodes << endl;
        if(k == INF) {
            break;
        }
        //cerr << (int)sroot.best_move_index << endl;
        //cerr << "Table size: " << table.size() << " Allocated: " << n_nodes << endl;
        if(overflow) break;
    }
    cerr << endl;
    overflow = false;
    m = sroot.children[sroot.best_move_index]->last_move;
    Move *move = nullptr;
    if(m != 64) {
        move = new Move(m / 8, m % 8);
    }
    //cerr << "Desired: " << endl;
    //disp(do_move(sroot.pos, m));
    end = sroot.children.end();
    for(it = sroot.children.begin(); it != end; it++) {
        //cerr << "Found :" << endl;
        //disp( (*it)->pos );
        //cerr << ((*it)->pos == do_move(sroot.pos, m)) << endl;
        if(!((*it)->pos == do_move(sroot.pos, m))) {
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
    //cerr << "Selected: " << endl;
    //disp(sroot.pos);
    //cerr << table.size() << endl;
    return move;
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

int Player::rawminimax(sBoardNode &node, int desired_depth, int side_multiplier) {
    //cerr << "Called for depth " << desired_depth << " with a " << a
    //<< " b " << b << " side " << side_multiplier << endl;
    //disp(node.pos);
    /*int original_alpha = a;
    auto it = table.find(node.pos);
    Entry en;
    if(it != table.end() && desired_depth == 0) {
        if(desired_depth == 0 && node.pos == sroot.pos) {
            cerr << "Here: ";
            cerr << (int)en.type << endl;
            cerr << (int)en.score << endl;
        }
        return table[node.pos].score;
    }*/

    if(desired_depth == 0) {
        //en.type = 0;
        //en.depth = 0;
        //en.score  = (side_multiplier * deepen_eval(node));
        return (side_multiplier * deepen_eval(node));
        //table[node.pos] = en;
        //return en.score;
    }

    int best = MINF, current;
    if(node.children.size() == 0) deepen_eval(node);
    //cerr << "Now a " << a << " b " << b << endl;
    for(unsigned int i = 0; i < node.children.size(); i++) {
        //if(desired_depth == 0) break;
        current = -rawminimax(*(node.children[i]), desired_depth - 1,
                -side_multiplier);
        if(current > best) {
            best = current;
            //a = max(a, current);
            //cerr << "a now " << a << " move " << (int)node.children[i]->last_move << endl;
            node.best_move_index = (unsigned char)i;
        }
        //if(a >= b) break;
    }

    return best;
}

int Player::rawnegamax(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    //cerr << "Called for depth " << desired_depth << " with a " << a
    //<< " b " << b << " side " << side_multiplier << endl;
    //disp(node.pos);
    //int original_alpha = a;
    //auto it = table.find(node.pos);
    //Entry en;
    /*if(it != table.end() && desired_depth == 0) {
        if(desired_depth == 0 && node.pos == sroot.pos) {
            cerr << "Here: ";
            cerr << (int)en.type << endl;
            cerr << (int)en.score << endl;
        }
        return table[node.pos].score;
    }*/

    if(desired_depth == 0) {
        return (side_multiplier * deepen_eval(node));
        /*en.type = 0;
        en.depth = 0;
        en.score = side_multiplier * deepen_eval(node);
        table[node.pos] = en;
        return en.score;*/
    }

    int best = MINF, current;
    //cerr << "Now a " << a << " b " << b << endl;
    if(node.children.size() == 0) {
        deepen_eval(node);
    }
    for(unsigned int i = 0; i < node.children.size(); i++) {
        //if(desired_depth == 0) break;
        current = -rawnegamax(*(node.children[i]), desired_depth - 1, -b, -a,
                -side_multiplier);
        if(current > best) {
            best = current;
            a = max(a, current);
            //cerr << "a now " << a << " move " << (int)node.children[i]->last_move << endl;
            node.best_move_index = (unsigned char)i;
            if(a >= b) break;
        }
    }
    if(best == INF && false) {
        cerr << "Called for depth " << desired_depth << " with a " << a
        << " b " << b << " side " << side_multiplier << endl;
        cerr << "Found forced win: " << endl;
        disp(node.pos);
        unsigned char m = node.children[node.best_move_index]->last_move;
        cerr << "with move: " << (m/8) << " " << (m % 8) << endl;
        cerr << (-negamax0(*(node.children[node.best_move_index])
                    , desired_depth - 1, -b, -a, -side_multiplier)) << endl;
    }
    return best;
}

int Player::negamax0(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    //cerr << "Called for depth " << desired_depth << " with a " << a
    //<< " b " << b << " side " << side_multiplier << endl;
    //disp(node.pos);
    //int original_alpha = a;
    if(overflow) return 0;
    auto it = table.find(node.pos);
    Entry en;
    if(it != table.end() && desired_depth == 0) {
        /*if(desired_depth == 0 && node.pos == sroot.pos) {
            cerr << "Here: ";
            cerr << (int)en.type << endl;
            cerr << (int)en.score << endl;
        }*/
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
    //cerr << "Now a " << a << " b " << b << endl;
    if(node.children.size() == 0) {
        deepen_eval(node);
    }
    for(unsigned int i = 0; i < node.children.size(); i++) {
        //if(desired_depth == 0) break;
        current = -negamax0(*(node.children[i]), desired_depth - 1, -b, -a,
                -side_multiplier);
        if(current > best) {
            best = current;
            a = max(a, current);
            //cerr << "a now " << a << " move " << (int)node.children[i]->last_move << endl;
            node.best_move_index = (unsigned char)i;
        }
        if(a >= b) break;
    }
    return best;
}

int Player::scoreab(sBoardNode &node, int desired_depth, int a, int b, 
        int side_multiplier) {
    //cerr << "Called for depth " << desired_depth << " with a " << a
    //<< " b " << b << " side " << side_multiplier << endl;
    //disp(node.pos);
    int original_alpha = a;
    auto it = table.find(node.pos);
    Entry en;
    if(it != table.end() && (en = it->second).depth >= desired_depth) {
        /*if(desired_depth == 0 && node.pos == sroot.pos) {
            cerr << "Here: ";
            cerr << (int)en.type << endl;
            cerr << (int)en.score << endl;
        }*/
        if(en.type == 0) return en.score;
        else if(en.type == (unsigned char)(-1)) a = max(a, en.score);
        else b = min(b, en.score);
        if (a >= b) return en.score;
    }


    //if(desired_depth == 0) return side_multiplier * deepen_eval(node);
    if(desired_depth == 0) {
        en.type = 0;
        en.depth = 0;
        en.score = side_multiplier * deepen_eval(node);
        //if(table[node.pos].score != 0) {
        //cerr << table[node.pos].score << " " << en.score << endl;
        //}
        table[node.pos] = en;
        return en.score;
    }
    /*if(desired_depth <= 0) {
    cerr << "Called for depth " << desired_depth << " with a " << a
    << " b " << b << " side " << side_multiplier << endl;
    disp(node.pos);
        cerr << (it == table.end()) << endl;
        cerr << table[node.pos].score << " " << (int)table[node.pos].type << endl;
    }*/
    
    int best = MINF, current;
    if(node.children.size() == 0) {
        deepen_eval(node);
    }
    //cerr << "Now a " << a << " b " << b << endl;
    for(unsigned int i = 0; i < node.children.size(); i++) {
        //if(desired_depth == 0) break;
        current = -scoreab(*(node.children[i]), desired_depth - 1, -b, -a,
                -side_multiplier);
        if(current > best) {
            best = current;
            a = max(a, current);
            //cerr << "a now " << a << " move " << (int)node.children[i]->last_move << endl;
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
    //if(table[node.pos].score == MINF && false) {
        //cerr << "Called for depth " << desired_depth << " with a " << a
        //    << " b " << b << " side " << side_multiplier << endl;
        //disp(node.pos);
        //cerr << node.pos.white << " " << node.pos.black << endl;
    //}
    //cerr << "Returned best " << best << endl;
    //cerr << table[node.pos].score << " " << (int)table[node.pos].depth << 
     //   " " << (int)table[node.pos].type << endl;
    //cerr << node.children.size() << endl;
    return best;
}

void Player::prune(sBoardNode *r) {
    if(r == nullptr) return;
    //cerr << r << endl;
    //cerr << "Pruning node: " << r->pos.white << " " << r->pos.black << endl;
    //cerr << r->children.size() << endl;
    if(r->children.size() != 0) {
        //cerr << r->children[0] << endl;
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
    //p.children.resize(p.children.capacity());
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
    int ourcorners = us[0] + us[7] + us[56] + us[63];
    int theircorners = them[0] + them[7] + them[56] + them[63];
    return ((STONES * (ours - theirs)) / (ours + theirs) + 
            (MOBILITY * max(ENDM - w - b, 0) * (n_moves - n_other_moves))
            / (n_moves + n_other_moves)
            + CORNERS * max(ENDC - w - b, 0) * (ourcorners - theircorners));
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
