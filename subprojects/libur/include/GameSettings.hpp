#pragma once
#include <array>
#include <iostream>
#include <memory>
#include <vector>
#include "coord.hpp"

namespace ur {
class GameSettings {
public:
    GameSettings(); // default settings
    std::array<coord, 5> rosettes;
    std::array<std::unique_ptr<std::vector<coord>>, 2> paths;
    const int colCount = 3;
    const int rowCount = 8;
    static const int DEFAULT_COUNTER_COUNT = 5; // some variants use 7
    static const int DEFAULT_BINARY_LOT_COUNT = 3;
    int counterCountPerPlayer;
    int binaryLotCount;
    bool rosetteIsExtraTurn: 1 = false;

    bool isRosette(coord c) const;
    bool isVoidSquare(const coord c) const;
    int decodeBinaryLot(unsigned int bl) const;
    bool pieceOnBoard(coord c) const;
    bool pieceWaitingToEnter(coord c) const;
    bool pieceFinished(coord c) const;

    friend std::ostream& operator<<(std::ostream &os, const GameSettings &gs);
};

void defaultRosettes(std::array<coord, 5>& rosettes);
void defaultPath(std::array<std::unique_ptr<std::vector<coord>>, 2>& coordVector);
}