#pragma once
#include <array>
#include <iostream>
#include <memory>
#include "coord.hpp"
#include "GameSettings.hpp"


#define PLAYER_1 0
#define PLAYER_2 1

namespace ur {

class GameState {
private:    
    void nextPlayer();
public:
    std::shared_ptr<GameSettings> settingsPtr;
    std::vector<coord> counters;
    unsigned int currentPlayerTurn:1;
    bool waitingOnBinaryLot:1;
    bool playerHasWon:1;
    unsigned int winningPlayer:1;
    unsigned int binaryLotResult:5;
    // only applies to pieces for the current player
    std::vector<bool> piecesThatMayMove;
    GameState();
    GameState(const GameState &gameState);

    //// game steps
    //// true on was valid move, false otherwise
    bool onBinaryLot(unsigned int binaryLotResult);
    // 0 <= pieceIndex <= settingsPtr->counterCountPerPlayer
    bool onMovePiece(int pieceIndex);
    bool movePieceOntoBoard();
    bool checkIfPlayerHasWon();
    /////
    // does no modification of state, visible or otherwise, mostly for frontend convenience
    // false if waiting on a binary lot...
    bool currentPlayerMayMovePieceOntoBoard() const;
    // 0 <= pieceIndex <= settingsPtr->counterCountPerPlayer
    // given a pieceIndex, return the coords of its destination if it can move
    coord pieceDestination(int pieceIndex) const;
    int pathIndex(int pieceIndex) const;
    int pathIndex(int pieceIndex, unsigned int player) const;
    int finishedPieceCount(unsigned int player) const;
    int pieceWaitingToEnterCount(unsigned int player) const;
    coord getNthPieceByPlayer(int pieceIndex, unsigned int player) const;
    int currPlayerCounterOffset() const;
    int otherPlayerCounterOffset() const;
    int playerOffset(unsigned int player) const;
    friend std::ostream& operator<<(std::ostream &os, const GameState &gs);
};
}