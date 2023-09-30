#include <algorithm>
#include <bit>
#include "GameSettings.hpp"

namespace ur{
GameSettings::GameSettings()
{
    defaultRosettes(this->rosettes);
    this->paths[0] = std::unique_ptr<std::vector<coord>>(new std::vector<coord>());
    this->paths[1] = std::unique_ptr<std::vector<coord>>(new std::vector<coord>());
    defaultPath(this->paths);
    this->counterCountPerPlayer = GameSettings::DEFAULT_COUNTER_COUNT;
    this->binaryLotCount = GameSettings::DEFAULT_BINARY_LOT_COUNT;
}

bool GameSettings::isRosette(coord c) const
{
    for(int i = 0; i < this->rosettes.size(); i++) {
        if(this->rosettes[i] == c) {
            return true;
        }
    }

    return false;
}

bool GameSettings::isVoidSquare(coord c) const
{
    if(c.x != 0 && c.x != 2) {
        return false;
    }
    return c.y == 4 || c.y == 5;
}

int GameSettings::decodeBinaryLot(unsigned int bl) const
{
    if(bl == 0) {
        return 4;
    }

    return std::popcount(bl);
}

void printPath(std::ostream &os, const std::unique_ptr<std::vector<coord>> &path, const GameSettings &gs)
{
    const char PATHCHARS[17] = "0123456789ABCDEF";

    for(int y = 0; y < gs.rowCount; y++) {
        for(int x = 0; x < gs.colCount; x++) {
            coord c = {x, y};
            auto findResult = std::find(path->begin(), path->end(), c);
            if(findResult != path->end()) {
                if(findResult == path->end() - 1) {
                    std::cout << 'Z';
                } else if(findResult == path->begin()) {
                    std::cout << 'S';
                } else {
                    std::cout << (PATHCHARS[std::distance(path->begin(), findResult) - 1]);
                }
            } else if(gs.isRosette(c)) {
                std::cout << 'R';
            } else if(gs.isVoidSquare(c)) {
                std::cout << ' ';
            } else {
                std::cout << '#';
            }
            
        }
        std::cout << '\n';
    }
}

std::ostream& operator<<(std::ostream &os, const GameSettings &gs)
{
    std::cout << "Player 1 path:\n";
    printPath(os, gs.paths[0], gs);
    std::cout << "Player 2 path:\n";
    printPath(os, gs.paths[1], gs);
    std::cout << "Counter count: " << gs.counterCountPerPlayer << ", Binary lots: " << gs.binaryLotCount;
    return os;
}

bool GameSettings::pieceWaitingToEnter(coord c) const
{
    return c == (*(this->paths[0]))[0] || c == (*(this->paths[1]))[0];
}

bool GameSettings::pieceFinished(coord c) const
{
    int pathlen = this->paths[0]->size();
    return c == (*(this->paths[0]))[pathlen-1] || c == (*(this->paths[1]))[pathlen-1];
}

bool GameSettings::pieceOnBoard(coord c) const
{
    return !this->pieceWaitingToEnter(c) && !this->pieceFinished(c);
}

void defaultRosettes(std::array<coord, 5>& rosettes)
{
    rosettes[0] = {0, 0};
    rosettes[1] = {2, 0};
    rosettes[2] = {1, 3};
    rosettes[3] = {0, 6};
    rosettes[4] = {2, 6};
}

void defaultPath(std::array<std::unique_ptr<std::vector<coord>>, 2>& coordVector)
{
    coordVector[0]->insert(coordVector[0]->end(), {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}});
    coordVector[0]->insert(coordVector[0]->end(), {{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}});
    coordVector[0]->insert(coordVector[0]->end(), {{2, 6}, {2, 7}, {1, 7}, {0, 7}, {0, 6}, {0, 5}});

    coordVector[1]->insert(coordVector[1]->end(), {{2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0}});
    coordVector[1]->insert(coordVector[1]->end(), {{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}});
    coordVector[1]->insert(coordVector[1]->end(), {{0, 6}, {0, 7}, {1, 7}, {2, 7}, {2, 6}, {2, 5}});

}
}