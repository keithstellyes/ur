#include <algorithm>

#include "OracleAi.hpp"

namespace urai {
int Oracle::score(const ur::GameState &gameState, unsigned int maximizingPlayer) const
{
    int counterCount = gameState.settingsPtr->counterCountPerPlayer;
    if(gameState.finishedPieceCount(maximizingPlayer) == counterCount) {
        return INT_MAX;
    } else if(gameState.finishedPieceCount(!maximizingPlayer) == counterCount) {
        return INT_MIN;
    }

    int score = 0;
    int indexOffset = gameState.playerOffset(maximizingPlayer);

    for(int i = 0; i < counterCount; i++) {
        const ur::coord c = gameState.counters[i + indexOffset];
        int progress = gameState.pathIndex(i, maximizingPlayer);
        score += progress * progressOnPathBias;
    }
    score += gameState.finishedPieceCount(maximizingPlayer) * pieceFinishedBias;
    score += gameState.pieceWaitingToEnterCount(maximizingPlayer) * pieceNotEnteredBias;
    // score += gameState.currentPlayerTurn == maximizingPlayer ? currentTurnBias : 0;

    return score;
}

Oracle::Oracle(unsigned int player)
{
    this->player = player;
}

Oracle::Oracle()
{
    this->player = PLAYER_2;
}

// 000 001 010 011 100 101 110 111
// Move 4 spaces: 1/8 000
// Move 3 spaces: 1/8 111
// Move 2 spaces: 3/8 011 101 110
// Move 1 spaces: 3/8 001 010 100

long Oracle::alphabeta(const ur::GameState &gameState, int depth, long alpha, long beta, unsigned int maximizingPlayer) const
{
    if(depth <= 0) {
        return score(gameState, maximizingPlayer);
    }

    if(gameState.currentPlayerTurn == maximizingPlayer) {
        long value = INT_MIN;
        std::vector<int> possibleMoves;
        // would be redundant to try every piece that can enter, so let's just add the first one to possibleMoves
        // we could do this for rosettes, but wouldn't be worth the extra work for our purposes
        if(gameState.currentPlayerMayMovePieceOntoBoard()) {
            for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
                if(gameState.pathIndex(i) == 0) {
                    possibleMoves.push_back(i);
                    break;
                }
            }
        }

        for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
            if(gameState.piecesThatMayMove[i] && gameState.pathIndex(i) != 0) {
                possibleMoves.push_back(i);
            } 
        }

        for(int option : possibleMoves) {
            ur::GameState childStatePostMovePreBl(gameState);
            childStatePostMovePreBl.onMovePiece(option);
            // TODO assert check for overflow
            ur::GameState childStatePostMovePostBl(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b000);
            value = std::max(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? maximizingPlayer : !maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b111);
            value = std::max(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? maximizingPlayer : !maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b001);
            value = std::max(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? maximizingPlayer : !maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b011);
            value = std::max(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? maximizingPlayer : !maximizingPlayer));
            if(value >= beta) {
                break; // beta cutoff
            }
            alpha = std::max(alpha, value);
        }

        return value;
    } else {
        long value = INT_MAX;
        std::vector<int> possibleMoves;
        // would be redundant to try every piece that can enter, so let's just add the first one to possibleMoves
        // we could do this for rosettes, but wouldn't be worth the extra work for our purposes
        if(gameState.currentPlayerMayMovePieceOntoBoard()) {
            for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
                if(gameState.pathIndex(i) == 0) {
                    possibleMoves.push_back(i);
                    break;
                }
            }
        }

        for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
            if(gameState.piecesThatMayMove[i] && gameState.pathIndex(i) != 0) {
                possibleMoves.push_back(i);
            } 
        }

        for(int option : possibleMoves) {
            ur::GameState childStatePostMovePreBl(gameState);
            childStatePostMovePreBl.onMovePiece(option);
            // TODO assert check for overflow
            ur::GameState childStatePostMovePostBl(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b000);
            value = std::min(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? !maximizingPlayer : maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b111);
            value = std::min(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? !maximizingPlayer : maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b001);
            value = std::min(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? !maximizingPlayer : maximizingPlayer));
            childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
            childStatePostMovePostBl.onBinaryLot(0b011);
            value = std::min(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, childStatePostMovePostBl.currentPlayerTurn == maximizingPlayer ? !maximizingPlayer : maximizingPlayer));
            if(value <= alpha) {
                // alpha cutoff
                break;
            }
            alpha = std::min(alpha, value);
        }

        return value;
    }
}

int Oracle::decide(ur::GameState &gameState)
{
    if(gameState.pieceWaitingToEnterCount(player) == 5)
    {
        return 0;
    }
    std::vector<int> possibleMoves;

    for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
        if(gameState.pathIndex(i) == 0) {
            possibleMoves.push_back(i);
            break;
        }
    }

    for(int i = 0; i < gameState.settingsPtr->counterCountPerPlayer; i++) {
        if(gameState.piecesThatMayMove[i] && gameState.pathIndex(i) != 0) {
            possibleMoves.push_back(i);
        } 
    }
    int chosenMove;
    unsigned int maximizingPlayer = gameState.currentPlayerTurn;
    int depth = 5;
    long alpha = INT_MIN;
    long beta = INT_MAX;
    long value = INT_MIN;

    for(int option : possibleMoves) {
        ur::GameState childStatePostMovePreBl(gameState);
        childStatePostMovePreBl.onMovePiece(option);
        // TODO assert check for overflow
        ur::GameState childStatePostMovePostBl(childStatePostMovePreBl);
        childStatePostMovePostBl.onBinaryLot(0b000);
        value = std::max(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, maximizingPlayer == childStatePostMovePostBl.currentPlayerTurn));
        childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
        childStatePostMovePostBl.onBinaryLot(0b111);
        value = std::max(value, alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, maximizingPlayer == childStatePostMovePostBl.currentPlayerTurn));
        childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
        childStatePostMovePostBl.onBinaryLot(0b001);
        value = std::max(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, maximizingPlayer == childStatePostMovePostBl.currentPlayerTurn));
        childStatePostMovePostBl = ur::GameState(childStatePostMovePreBl);
        childStatePostMovePostBl.onBinaryLot(0b011);
        value = std::max(value, 3 * alphabeta(childStatePostMovePostBl, depth - 1, alpha, beta, maximizingPlayer == childStatePostMovePostBl.currentPlayerTurn));
        alpha = std::max(alpha, value);
        chosenMove = option;
    }

    return chosenMove;
}
}