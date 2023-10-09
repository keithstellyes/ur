#pragma once
#include "raylib.h"
#include "rlgl.h" 
#include <cstddef>

#include "AiThread.hpp"
#include <thread>

#include "GameStateDrawer.hpp"

class DrawThread {
private:
ur::GameState *gameState;
std::thread thread;
AiThread *aiThread;
bool shouldStop = false;

public:
DrawThread(ur::GameState *gameState, AiThread *aiThread);
void threadFunction();
void start();
void join();
~DrawThread();
};