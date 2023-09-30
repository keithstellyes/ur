#include <iostream>
#include "GameState.hpp"
#include "BinaryLotMachine.hpp"
#include <bitset>

int main()
{
    ur::GameState gs;
    // std::cout << gs << std::endl;
    // std::cout << *(gs.settingsPtr) << std::endl;

    ur::BinaryLotMachine blm;
    gs.onBinaryLot(0b101);
    // should be true
    gs.movePieceOntoBoard();
    std::cout << gs << std::endl;
    return 0;
}