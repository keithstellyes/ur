#pragma once
#include <random>
namespace ur{
class BinaryLotMachine {
private:
    std::mt19937 twister;
    int lotCount = 3;
public:
    void seed(unsigned int seedValue);
    int next();
};
}