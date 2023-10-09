#pragma once

#include "raylib.h"
#include <optional>

enum class StartGameDecision {
    VersusOracle, VersusRandom, VersusHotseat
};

class StartGameMenu {
private:
    Image random;
    Image hotSeat;
    Image oracle;
    Texture2D randomText;
    Texture2D hotSeatText;
    Texture2D oracleText;
public:
    StartGameMenu();
    std::optional<StartGameDecision> checkMenu();
};