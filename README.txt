Alex Bourzutschky (me) worked solo.

I made one major change: I changed the board representation to a struct
rather than a class, and my board included the side and the positions of
the two sides' stones. This supposedly makes it faster to store and use.

I also implemented negamax with alpha-beta pruning and a lookup table,
following the pseudocode on Wikipedia's page. I have raw negamax
with alpha-beta, negamax with a lookup tree for just leaf nodes, and
negamax with a lookup tree for nodes of arbitrary depth, and I used this
one in the end. I iterate the depth by even numbers, so that refutations
are always seen if present.

All of my negamaxes actually store the whole tree. This allows for nice
carryover from move to move, for part of the tree is already generated. I
tried doing without the tree, but this fared a good amount slower, to my
surprise-- the massive amounts of allocations and deallocations for the
tree would have caused overhead, I'd thought, but having to regenerate
the tree entirely move to move was not worth the cost.

To avoid running out of memory, I cap the tree at 8 million board nodes
and the lookup table at 1.8 million entries. These were gauged by hand--
I would not be surprised to see my AI go over 768M by a bit in a game or
two in the tournament.

My time management seems fairly robust, since it sets target times for
normal play and for the endgame where the aim is to depth out and find
forced wins. Given the large amount of space I am using to gain time, I
can typically find forced wins with 13-15 pieces to go. The limiting factor
is usually space in the tree, but sometimes it is space in the table or
time left.

My board evaluation depends on the positions of the stones with a mask
roughly given by that at othellomaster.com/OM/Report/HTML/img13.png where
the corners have great positive weight, the squares next to them have
great negative weight, and so forth. It also depends on the number of
moves available, and there is great speed efficiency in that when finding
the heuristic it checks the moves available and adds the children then
and there, together. The checking of available moves and performing of
moves mimics that done in the provided class.

I think my mobility heuristic greatly helps my AI reach positions where it
may have few stones but the opponent has so few moves that mine will pull
ahead. The depthing out at 13-15 stones to go will help mine find forced
wins sooner than most others.
