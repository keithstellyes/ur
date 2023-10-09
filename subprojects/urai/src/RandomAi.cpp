#include "RandomAi.hpp"

namespace urai {
Random::Random()
{

}

Random::Random(int seed)
{
    twister = std::mt19937(seed);
}

int Random::decide(ur::GameState &gameState)
{
    auto value = twister() & 0b111;
    const int maxLegalValue = gameState.piecesThatMayMove.size() - 1;
    while(value > maxLegalValue && !gameState.piecesThatMayMove[value]) {
        value = twister() & 0b111;
    }

    return value;
}
}