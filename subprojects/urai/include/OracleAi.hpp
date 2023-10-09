#pragma once
#include "Ai.hpp"

namespace urai {
// uses minimax/alphabeta pruning
class Oracle : public Ai {
private:
long alphabeta(const ur::GameState &gameState, int depth, long alpha, long beta, unsigned int maximizingPlayer) const;
public:
    // 100 so these can be divided based on how likely they are to be entered, when used by minimax algorith
    // e.g. a node with a score will be divided by a higher number if it requires a 000 or 111 result
    int progressOnPathBias = 100;
    // better to move on a rosette than to be on the space after it
    int currentTurnBias = progressOnPathBias + progressOnPathBias / 2;
    int pieceNotEnteredBias = -progressOnPathBias;
    // it's at least better than a normal 1 move
    int pieceFinishedBias = progressOnPathBias;
    int depth = 5;

    Oracle();
    Oracle(unsigned int player);

    // the heuristic value of a game state(node)
    int score(const ur::GameState &gameState, unsigned int maximizingPlayer) const;

    virtual int decide(ur::GameState &gameState);
};
}