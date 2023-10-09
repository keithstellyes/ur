#include "DrawThread.hpp"
#include "MainMenu.hpp"
#include "InfoMenu.hpp"
#include "StartGameMenu.hpp"
#include "RandomAi.hpp"
#include <iostream>
#include <sstream>

DrawThread::DrawThread(ur::GameState *gameState, AiThread *aiThread)
{
    this->gameState = gameState;
    this->aiThread = aiThread;
}

enum class AppState {
    MainMenu, InfoMenu, StartGameMenu, InGame
};

void DrawThread::threadFunction()
{
    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 20.0f, 15.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, WINDOW_TITLE);
    SetTargetFPS(60);
    GameStateDrawer gameStateDrawer(gameState, &camera, aiThread);
    MainMenu mainMenu;
    InfoMenu infoMenu;
    StartGameMenu startGameMenu;
    AppState state = AppState::MainMenu;
    bool playingAi = false;
    urai::Oracle oracle;
    urai::Random randomAi;

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // gameStateDrawer.draw();
            std::optional<MainMenuDecision> mainMenuDecision = std::nullopt;
            std::optional<InfoMenuDecision> infoMenuDecision = std::nullopt;
            std::optional<StartGameDecision> startGameDecision = std::nullopt;
            switch(state) {
                case AppState::MainMenu:
                    mainMenuDecision = mainMenu.checkMenu();
                    if(mainMenuDecision.has_value()) {
                        if(mainMenuDecision.value() == MainMenuDecision::StartGame) {
                            state = AppState::StartGameMenu;
                        } else if(mainMenuDecision.value() == MainMenuDecision::OpenInfo) {
                            state = AppState::InfoMenu;
                        }
                    }
                    break;
                case AppState::InGame:
                    gameStateDrawer.draw(playingAi);
                    break;
                case AppState::InfoMenu:
                    infoMenuDecision = infoMenu.checkMenu();
                    if(infoMenuDecision.has_value()) {
                        state = AppState::MainMenu;
                    }
                    break;
                case AppState::StartGameMenu:
                    startGameDecision = startGameMenu.checkMenu();
                    if(startGameDecision.has_value()) {
                        StartGameDecision dec = startGameDecision.value();
                        playingAi = dec != StartGameDecision::VersusHotseat;
                        if(dec == StartGameDecision::VersusOracle) {
                            aiThread->setAi(&oracle);
                        } else if(dec == StartGameDecision::VersusRandom) {
                            aiThread->setAi(&randomAi);
                        }
                        state = AppState::InGame;
                    }
                    break;
            }

        EndDrawing();
    }

    CloseWindow();
}

void DrawThread::start()
{
    thread = std::thread(&DrawThread::threadFunction, this);
}

DrawThread::~DrawThread()
{

}

void DrawThread::join()
{
    thread.join();
}