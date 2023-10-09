#include "BinaryLotMachine.hpp"

namespace ur{
BinaryLotMachine::BinaryLotMachine(unsigned int seed)
{
    this->seed(seed);
}

void BinaryLotMachine::seed(unsigned int seedValue)
{
    this->twister.seed(seedValue);
}

int BinaryLotMachine::next()
{
    return (this->twister()) & 0b111;
}
}