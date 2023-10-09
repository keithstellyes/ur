#pragma once

#include "GameState.hpp"

namespace urai {
class Ai {
protected:
unsigned int player:1;
public:
    Ai() = default;
    ~Ai() = default;
    virtual int decide(ur::GameState &gameState) = 0;
};

unsigned int aiVersusAi(Ai &ai1, Ai &ai2, unsigned int blmSeed);
}