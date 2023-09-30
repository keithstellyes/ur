#include <algorithm>

#include "GameState.hpp"
#include "GameSettings.hpp"
#include "coord.hpp"

namespace ur {
GameState::GameState()
{
    this->settingsPtr = std::unique_ptr<GameSettings>(new GameSettings());
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        this->counters.push_back((*(this->settingsPtr->paths[0]))[0]);
    }
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        this->counters.push_back((*(this->settingsPtr->paths[1]))[0]);
    }

    this->piecesThatMayMove = std::vector<bool>(this->settingsPtr->counterCountPerPlayer);
    this->waitingOnBinaryLot = true;
}

std::ostream& operator<<(std::ostream &os, const GameState &gs)
{
    for(int y = 0; y < gs.settingsPtr->rowCount; y++) {
        std::cout << '|';
        for(int x = 0; x < gs.settingsPtr->colCount; x++) {
            coord c = {x, y};
            if(gs.settingsPtr->isVoidSquare(c)) {
                os << " |";
            } else {
                std::cout << (gs.settingsPtr->isRosette(c) ? 'R' : '#') << '|';
            }
        }
        std::cout << '\n';
    }

    os << "Player 1 counters: ";
    int notEnteredCount = 0;
    int finishedCount = 0;
    for(int i = 0; i < gs.settingsPtr->counterCountPerPlayer; i++) {
        if(gs.settingsPtr->pieceWaitingToEnter(gs.counters[i])) {
            notEnteredCount++;
            continue;
        }
        os << gs.counters[i] << ',';
    }
    os << '(' << "not entered: " << notEnteredCount << ", " << "finished: " << gs.finishedPieceCount(PLAYER_1) << ")\n";
    notEnteredCount = 0;
    finishedCount = 0;
    os << "Player 2 counters: ";
    for(int i = 0; i < gs.settingsPtr->counterCountPerPlayer; i++) {
        if(gs.settingsPtr->pieceWaitingToEnter(gs.counters[i + gs.settingsPtr->counterCountPerPlayer])) {
            notEnteredCount++;
            continue;
        }
        os << gs.counters[i + gs.settingsPtr->counterCountPerPlayer] << ',';
    }
    os << '(' << "not entered: " << notEnteredCount << ", " << "finished: " << gs.finishedPieceCount(PLAYER_2) << ")\n";
    return os;
}

bool GameState::onBinaryLot(unsigned int binaryLotResult)
{
    if(!this->waitingOnBinaryLot || this->playerHasWon) {
        return false; // not currently waiting on a binary lot result
    }
    
    this->waitingOnBinaryLot = false;
    this->binaryLotResult = binaryLotResult;
    const int spacesToMove = this->settingsPtr->decodeBinaryLot(binaryLotResult);
    
    //// Compute if a brand new piece may enter the board ////
    const auto& path = this->settingsPtr->paths[this->currentPlayerTurn];

    // ASSERT THAT PATH[3] IS ACTUALLY A ROSETTE
    bool enterDestSpaceIsRosette = binaryLotResult == 0;
    const coord enteringDestSpace = (*path)[spacesToMove];

    bool initialSpaceClear = true; 
    bool enteringDestSpaceHasNoPawns = true;
    // check if any pieces are on the dest space OR on the initial space
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        int index = this->pathIndex(i);
        if(index == 1) {
            initialSpaceClear = false;
        }
        if(this->counters[i + this->currPlayerCounterOffset()] == enteringDestSpace) {
            enteringDestSpaceHasNoPawns = false;
        }
    }

    bool enteringDestSpaceClear = enteringDestSpaceHasNoPawns || enterDestSpaceIsRosette;
    bool pieceMayEnterBoard = enteringDestSpaceClear && initialSpaceClear;
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        int index = this->pathIndex(i);
        if(index == 0) {
            this->piecesThatMayMove[i] = pieceMayEnterBoard;
        }
    }
    //////////////////////////////////////////////////////////
    /*
     * iterate through counters, if it's on the board, then
     * check if it can move those n spaces, or, if it's waiting to enter, then if it meets conditions computed above
     */ 
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        int index = this->pathIndex(i);
        if(index == 0) {
            continue; // we handle these in the previous loop
        }
        if(index + spacesToMove < path->size()) {
            coord destSpace = (*path)[index + spacesToMove];
            if(this->settingsPtr->isRosette(destSpace)) {
                // don't think we can capture enemy pawns here (can they share?)
                this->piecesThatMayMove[i] = true;
            } else {
                bool hasFriendlyCounters = false;

                for(int j = 0; j < this->settingsPtr->counterCountPerPlayer; j++) {
                    if(this->counters[j + this->currPlayerCounterOffset()] == destSpace) {
                        hasFriendlyCounters = true;
                        break;
                    }
                }
                this->piecesThatMayMove[i] = !hasFriendlyCounters || this->settingsPtr->pieceFinished(destSpace);
            }

        } else {
            this->piecesThatMayMove[i] = false;
        }
    }

    bool hasValidMove = std::find(this->piecesThatMayMove.begin(), this->piecesThatMayMove.end(), true) != this->piecesThatMayMove.end();
    if(!hasValidMove) {
        this->waitingOnBinaryLot = true;
        this->nextPlayer();
    }
    return true;
}

bool GameState::currentPlayerMayMovePieceOntoBoard() const
{
    if(this->waitingOnBinaryLot || this->playerHasWon) {
        return false;
    }

    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        coord c = this->counters[i + this->currPlayerCounterOffset()];
        if(!this->settingsPtr->pieceWaitingToEnter(c)) {
            continue;
        }
        return this->piecesThatMayMove[i];
    }
    return false;
}

void GameState::nextPlayer()
{
    this->currentPlayerTurn = !(this->currentPlayerTurn);
}

bool GameState::onMovePiece(int pieceIndex)
{
    if(this->waitingOnBinaryLot || this->playerHasWon || ! this->piecesThatMayMove[pieceIndex]) {
        return false;
    }

    const auto& path = this->settingsPtr->paths[this->currentPlayerTurn];

    //// CASE: Piece already on board trying to exit
    const int spacesToMove = this->settingsPtr->decodeBinaryLot(binaryLotResult);
    if(this->pathIndex(pieceIndex) + spacesToMove > path->size()) {
        return false; // must roll exact
    }

    // TODO pieces don't actually move forward this way beyond however much they can initially move, i.e. probably want to use an std::find within the path using current counter
    coord dest = pieceDestination(pieceIndex);
    bool destIsRosette = this->settingsPtr->isRosette(dest);
    

    //// CASE: Piece entering board for first time
    if(this->settingsPtr->pieceWaitingToEnter(this->counters[pieceIndex + this->currPlayerCounterOffset()])) {
        coord entryCoord = (*path)[1];
        bool destOccupied = false;
        // search for a friendly piece on entry
        for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
            if(this->counters[pieceIndex + this->currPlayerCounterOffset()] == entryCoord) {
                // we're trying to move a piece onto the board, however the entry space is filled, therefore this move is illegal
                return false;
            }

            if(this->counters[pieceIndex + this->currPlayerCounterOffset()] == dest) {
                destOccupied = true;
            }
        }

        // entry not blocked...
        if(destIsRosette) {
            // piece has guaranteed entry 
            this->counters[pieceIndex + this->currPlayerCounterOffset()] = dest;
            this->waitingOnBinaryLot = true;
            return true;
        } else {
            if(destOccupied) {
                this->waitingOnBinaryLot = false;
                return false;
            }
            this->counters[pieceIndex + this->currPlayerCounterOffset()] = dest;
            this->nextPlayer();
            this->waitingOnBinaryLot = true;
            return true;
        }
    }

    // all paths after this are guaranteed valid
    this->counters[pieceIndex + this->currPlayerCounterOffset()] = dest;
    //// CASE: Piece already on board
    if(destIsRosette) {
        // bonus turn
        this->waitingOnBinaryLot = true;
        return true;
    } else {
        for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
            const coord currPiece = this->counters[i + this->otherPlayerCounterOffset()];
            if(currPiece == dest) {
                // capture
                this->counters[i + this->otherPlayerCounterOffset()] = (*(this->settingsPtr->paths[!this->currentPlayerTurn]))[0];
            }
        }
        this->nextPlayer();
    }
    this->waitingOnBinaryLot = true;
    return true;
}

bool GameState::movePieceOntoBoard()
{
    if(this->waitingOnBinaryLot || this->playerHasWon) {
        return false;
    }

    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        const coord currPiece = this->counters[i + this->currPlayerCounterOffset()];
        if(this->settingsPtr->pieceWaitingToEnter(currPiece)) {
            if(this->piecesThatMayMove[i] == false) {
                return false;
            }

            return this->onMovePiece(i);
        }
    }

    return false;
}

bool GameState::checkIfPlayerHasWon()
{
    int target = this->settingsPtr->counterCountPerPlayer;
    bool player1Won = this->finishedPieceCount(PLAYER_1) == target;
    bool player2Won = this->finishedPieceCount(PLAYER_2) == target;
    this->playerHasWon = player1Won || player2Won;
    if(player1Won) {
        this->winningPlayer = PLAYER_1;
    } else if(player2Won) {
        this->winningPlayer = PLAYER_2;
    }

    return this->playerHasWon;
}

int GameState::currPlayerCounterOffset() const
{
    return this->playerOffset(this->currentPlayerTurn);
}

int GameState::otherPlayerCounterOffset() const
{
    return this->playerOffset(!(this->currentPlayerTurn));
}

int GameState::playerOffset(unsigned int player) const
{
    return player * this->settingsPtr->counterCountPerPlayer;
}

int GameState::pathIndex(int pieceIndex) const
{
    coord c = this->counters[pieceIndex + this->currPlayerCounterOffset()];
    const auto& path = this->settingsPtr->paths[this->currentPlayerTurn];
     
    return std::distance(path->begin(), std::find(path->begin(), path->end(), c));
}

coord GameState::pieceDestination(int pieceIndex) const
{
    // don't know if our logic is correct :/
    return (*this->settingsPtr->paths[this->currentPlayerTurn])[(this->pathIndex(pieceIndex) + this->settingsPtr->decodeBinaryLot(this->binaryLotResult))];
}

int GameState::finishedPieceCount(unsigned int player) const
{
    int count = 0;
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        coord c = this->getNthPieceByPlayer(i, player);
        auto const& path = this->settingsPtr->paths[player];
        if(c == (*path)[path->size() - 1]) {
            count++;
        }
    }

    return count;
}

int GameState::pieceWaitingToEnterCount(unsigned int player) const
{
    int count = 0;
    for(int i = 0; i < this->settingsPtr->counterCountPerPlayer; i++) {
        coord c = this->getNthPieceByPlayer(i, player);
        auto const& path = this->settingsPtr->paths[player];
        if(c == (*path)[0]) {
            count++;
        }
    }

    return count;
}

coord GameState::getNthPieceByPlayer(int pieceIndex, unsigned int player) const
{
    //ASSERT player is 0 or 1
    return this->counters[pieceIndex + (this->settingsPtr->counterCountPerPlayer) * player];
}
}