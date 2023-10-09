#pragma once

#include "GameState.hpp"
#include "raylib-extra.hpp"
#include "config.hpp"
#include "BinaryLotMachine.hpp"
#include "OracleAi.hpp"
#include "AiThread.hpp"
#include <optional>
#include <future>

#include <memory>
#include <thread>
#include <vector>

struct BoardSpace {
    Texture2D texture;
    BoundingBox boundingBox;
    ur::coord coord;
};

class GameStateDrawer {
private:
    ur::GameState *gameState;
    AiThread *aiThread;
    std::optional<std::future<int>> aiDecision;
    std::vector<BoardSpace> boardSpaces;
    BoundingBox boardBounds;
    int spaceLength;
    Model boardModel;
    Camera3D *camera;
    ur::BinaryLotMachine blm;
    float camDist = 5;  // how far away from the target the camera is (radius)
    float rotAngle = 45; // the rotation angle around the target  (around Y)
    float tiltAngle = 45; // the tilt tangle of the camera (up/down)

    float rotSpeed = 0.25f / 10; // to scale the mouse input
    float moveSpeed = 10.0f / 10; // to scale the linear input
    Vector3 moveVec = { 0,0,0 };
    urai::Oracle ai;

public:
    GameStateDrawer(ur::GameState *gameState, Camera3D *camera, AiThread *aiThread);
    GameStateDrawer();
    ~GameStateDrawer();

    void draw(bool playingAi);
};