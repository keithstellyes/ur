#pragma once
#include <random>
namespace ur{
class BinaryLotMachine {
private:
    std::mt19937 twister;
    int lotCount = 3;
public:
    BinaryLotMachine() = default;
    BinaryLotMachine(unsigned int seed);
    void seed(unsigned int seedValue);
    int next();
};
}