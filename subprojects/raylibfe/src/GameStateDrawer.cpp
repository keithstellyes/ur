#include "GameStateDrawer.hpp"

#include "raylib.h"
#include "raymath.h"

#include "OracleAi.hpp"

#include <chrono>
#include <iostream>
#include <optional>
#include <sstream>

BoundingBox boundingBoxForLogicalCoord(const ur::coord c, const BoundingBox bounds, const int spaceLength);

GameStateDrawer::GameStateDrawer()
{
}

GameStateDrawer::GameStateDrawer(ur::GameState *gameState, Camera3D *camera, AiThread *aiThread)
{
    this->gameState = gameState;
    this->boardModel = LoadModel(ASSET_PATH "ur-board.obj");
    this->camera = camera;
    this->aiThread = aiThread;

    const Texture2D boardTexture = LoadTexture(ASSET_PATH "balsa.png");
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = boardTexture; 
    this->boardBounds = GetMeshBoundingBox(boardModel.meshes[0]); 
    // assumes x on the model is x of the logical ur game board, and z on the model is y of the logical ur game board
    this->spaceLength = ((this->boardBounds.max.x - this->boardBounds.min.x) / 3);
    const Texture2D rosetteTexture = LoadTexture(ASSET_PATH "rosette.png");

    const Texture2D space0Texture = LoadTexture(ASSET_PATH "space0.png");
    const Texture2D space1Texture = LoadTexture(ASSET_PATH "space1.png");
    const Texture2D space4Texture = LoadTexture(ASSET_PATH "space4.png");
    const Texture2D space5Texture = LoadTexture(ASSET_PATH "space5.png");
    const Texture2D space9Texture = LoadTexture(ASSET_PATH "space9.png");

    std::vector<ur::coord> space0Coords {{0, 3}, {0, 1}, {2, 3}, {2, 1}, {1, 6}};
    std::vector<ur::coord> space1Coords {{0, 2}, {2, 2}, {1, 1}, {1, 4}, {1, 7}};
    std::vector<ur::coord> space4Coords {{1, 0}};
    std::vector<ur::coord> space5Coords {{1, 2}, {1, 5}};
    std::vector<ur::coord> space9Coords {{0, 7}, {2, 7}};

    for(const auto& rosette : this->gameState->settingsPtr->rosettes) {
        BoundingBox bb = boundingBoxForLogicalCoord(rosette, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({rosetteTexture, bb, rosette});
    }

    for(const auto& coord : space0Coords) {
        BoundingBox bb = boundingBoxForLogicalCoord(coord, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({space0Texture, bb, coord});
    }

    for(const auto& coord : space1Coords) {
        BoundingBox bb = boundingBoxForLogicalCoord(coord, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({space1Texture, bb, coord});
    }

    for(const auto& coord : space4Coords) {
        BoundingBox bb = boundingBoxForLogicalCoord(coord, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({space4Texture, bb, coord});
    }

    for(const auto& coord : space5Coords) {
        BoundingBox bb = boundingBoxForLogicalCoord(coord, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({space5Texture, bb, coord});
    }

    for(const auto& coord : space9Coords) {
        BoundingBox bb = boundingBoxForLogicalCoord(coord, this->boardBounds, spaceLength);
        this->boardSpaces.push_back({space9Texture, bb, coord});
    }
}

void GameStateDrawer::draw(bool playingAi)
{
    unsigned int aiPlayer = playingAi ? PLAYER_2 : 0xCAFEBABE;
    // if (IsKeyDown(KEY_W))
    //     moveVec.z = -moveSpeed * GetFrameTime();
    // if (IsKeyDown(KEY_S))
    //     moveVec.z = moveSpeed * GetFrameTime();

    // if (IsKeyDown(KEY_A))
    //     moveVec.x = -moveSpeed * GetFrameTime();
    // if (IsKeyDown(KEY_D))
    //     moveVec.x = moveSpeed * GetFrameTime();

    // referenced https://gist.github.com/JeffM2501/000787070aef421a00c02ae4cf799ea1 for rotation/orbiting logic
    float rotDir = 0;
    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        rotDir = IsKeyDown(KEY_A) ? 1 : -1;
        rotAngle += rotDir * rotSpeed;

        // if (tiltAngle > 89)
        //     tiltAngle = 89;
        // if (tiltAngle < 1)
        //     tiltAngle = 1;
    }

    float tiltDir = 0;
    if(IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {
        tiltDir = IsKeyDown(KEY_W) ? 1 : -1;
        tiltAngle += tiltDir * moveSpeed;

        // if (tiltAngle > 89)
        //     tiltAngle = 89;
        // if (tiltAngle < 1)
        //     tiltAngle = 1;
    }

    bool playerHasWon = gameState->checkIfPlayerHasWon();

    if(!playerHasWon) {
        while(gameState->waitingOnBinaryLot) {
            gameState->onBinaryLot(this->blm.next());
        }

        if(IsKeyPressed(KEY_ENTER) && gameState->currentPlayerTurn != aiPlayer) {
            gameState->movePieceOntoBoard();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && gameState->currentPlayerTurn != aiPlayer)
        {
            const auto mouseRay = GetMouseRay(GetMousePosition(), *camera);
            std::optional<const ur::coord> selectedCoord;
            for(BoardSpace bs : boardSpaces) {
                if(GetRayCollisionBox(mouseRay, bs.boundingBox).hit) {
                    selectedCoord.emplace(bs.coord);
                    break;
                }
            }

            if(selectedCoord.has_value()) {
                const ur::coord val = selectedCoord.value();
                for(int i = 0; i < gameState->piecesThatMayMove.size(); i++) {
                    if(gameState->piecesThatMayMove[i]) {
                        if(val == gameState->counters[i + gameState->currPlayerCounterOffset()]) {
                            gameState->onMovePiece(i);
                        }
                    }
                }
            }
        }
    }
    
    Vector3 camPos = camera->position;
    Matrix tiltMat = MatrixRotateX(tiltAngle * GetFrameTime() * 0.1 * tiltDir); // a matrix for the tilt rotation
    Matrix rotMat = MatrixRotateY(rotAngle * GetFrameTime() * 0.1 * rotDir); // a matrix for the plane rotation
    Matrix mat = MatrixMultiply(tiltMat, rotMat); // the combined transformation matrix for the camera position

    camPos = Vector3Transform(camPos, mat); // transform the camera position into a vector in world space
    moveVec = Vector3Transform(moveVec, rotMat); // transform the movement vector into world space, but ignore the tilt so it is in plane

    camera->target = Vector3Add(camera->target, moveVec); // move the target to the moved position

    camera->position = Vector3Add(camera->target, camPos);

    BeginMode3D(*camera);
        DrawModel(this->boardModel, {0.0f, 0.0f, 0.0f}, 1.0, WHITE);
        for(const BoardSpace bs : this->boardSpaces) {
            DrawCubeTexture(bs.texture, 
                {this->boardBounds.min.x + (spaceLength / 2 + spaceLength * bs.coord.x), 
                this->boardBounds.max.y + 0.1, this->boardBounds.min.z + (spaceLength / 2 + spaceLength * bs.coord.y)}, 
                spaceLength, spaceLength / 10, spaceLength, WHITE);
        }

        for(int i = 0; i < gameState->settingsPtr->counterCountPerPlayer; i++) {
            const ur::coord c1 = gameState->counters[i];
            const ur::coord c2 = gameState->counters[i + gameState->settingsPtr->counterCountPerPlayer];
            Vector3 v3_1 = {this->boardBounds.min.x + ((spaceLength / 2 + spaceLength * c1.x) - spaceLength / 3.0), 
                this->boardBounds.max.y, (this->boardBounds.min.z + (spaceLength / 2 + spaceLength * c1.y)- spaceLength / 3.0)};
            Vector3 v3_2 = {this->boardBounds.min.x + ((spaceLength / 2 + spaceLength * c2.x) + spaceLength / 3.0), 
                this->boardBounds.max.y, (this->boardBounds.min.z + (spaceLength / 2 + spaceLength * c2.y)- spaceLength / 3.0)};
            if((i & 1) != 0) {
                // v3.x += spaceLength / 3.0;
                v3_1.y += spaceLength / 5.0;
                v3_1.z += spaceLength / 6.0;
                v3_2.y += spaceLength / 5.0;
                v3_2.z += spaceLength / 6.0;
            }

            v3_1.z += (spaceLength / 3.0) * (i % 3);
            v3_2.z += (spaceLength / 3.0) * (i % 3);
            DrawCylinder(v3_1, spaceLength / 6.0, spaceLength / 6.0, spaceLength / 5.0, 10, LIGHTGRAY);
            DrawCylinder(v3_2, spaceLength / 6.0, spaceLength / 6.0, spaceLength / 5.0, 10, BLACK);
        }

        // DrawBoundingBox(this->boardBounds, GREEN);
    EndMode3D();
    std::ostringstream oss;

    if(!playerHasWon && playingAi && gameState->currentPlayerTurn == aiPlayer) {
        oss << "AI is thinking...\n";
        // todo should be a separate thread...
        // gameState->onMovePiece(ai.decide(*gameState));
        if(!aiDecision.has_value()) {
            aiDecision = aiThread->decide(*gameState);
        } else {
            auto status = aiDecision.value().wait_for(std::chrono::hours::zero());
            if(status == std::future_status::ready) {
                const int move = aiDecision.value().get();
                gameState->onMovePiece(move);
                aiDecision = std::nullopt;
            }
        }
    } else if(playerHasWon) {
        oss << "Winner: " << (1 + gameState->winningPlayer) << '\n';
    }

    if(!playerHasWon && ((playingAi && gameState->currentPlayerTurn != aiPlayer) || !playingAi)) {
        if(gameState->currentPlayerMayMovePieceOntoBoard()) {
            oss << "May move piece onto board (Enter)\n";
        }
        oss << "May move " << gameState->settingsPtr->decodeBinaryLot(gameState->binaryLotResult) << " spaces.\n";
    }
    
    DrawText(oss.str().c_str(), 10, 200, 20, LIGHTGRAY);
}

BoundingBox boundingBoxForLogicalCoord(const ur::coord c, BoundingBox bounds, const int spaceLength)
{
    BoundingBox bb;
    // don't understand why we have to halve for the mins, but if it works it works...
    // there was some other time I think I had to consider having to half the spaceLength? so I guess it would make sense
    bb.min.x = bounds.min.x + (spaceLength / 2 + spaceLength * c.x) - spaceLength / 2;
    bb.min.y = 0.1 + bounds.max.y - spaceLength / 20;
    bb.min.z = bounds.min.z + (spaceLength / 2 + spaceLength * c.y) - spaceLength / 2;
    bb.max.x = bb.min.x + spaceLength;
    bb.max.y = 0.1 + bb.min.y + spaceLength / 10;
    bb.max.z = bb.min.z + spaceLength;

    return bb;
}

GameStateDrawer::~GameStateDrawer()
{
    // UnloadModel(boardModel);
    // UnloadTexture(space0Texture);
    // UnloadTexture(space1Texture);
    // UnloadTexture(space4Texture);
    // UnloadTexture(space5Texture);
    // UnloadTexture(space9Texture);
}