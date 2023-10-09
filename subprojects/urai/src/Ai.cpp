#include "Ai.hpp"
#include "GameState.hpp"
#include "BinaryLotMachine.hpp"

namespace urai {
unsigned int aiVersusAi(Ai &ai1, Ai &ai2, unsigned int blmSeed)
{
    ur::GameState gameState;
    ur::BinaryLotMachine blm(blmSeed);

    while(!gameState.checkIfPlayerHasWon()) {
        if(gameState.waitingOnBinaryLot) {
            gameState.onBinaryLot(blm.next());
        } else {
            if(gameState.currentPlayerTurn == PLAYER_1) {
                gameState.onMovePiece(ai1.decide(gameState));
            } else {
                gameState.onMovePiece(ai2.decide(gameState));
            }
        }
    }

    return gameState.winningPlayer;
}
}