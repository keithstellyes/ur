#include "BinaryLotMachine.hpp"

namespace ur{
void BinaryLotMachine::seed(unsigned int seedValue)
{
    this->twister.seed(seedValue);
}

int BinaryLotMachine::next()
{
    return (this->twister()) & 0b111;
}
}