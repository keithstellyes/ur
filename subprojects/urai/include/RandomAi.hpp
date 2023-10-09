#pragma once

#include <random>

#include "Ai.hpp"

namespace urai {
class Random : public Ai {
private:
    std::mt19937 twister;
public:
    Random();
    Random(int seed);
    virtual int decide(ur::GameState &gameState);
};
}