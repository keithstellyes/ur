#include "MainMenu.hpp"
#include "raylib.h"
#include "rlgl.h" 
#include "MainMenu.hpp"

std::optional<MainMenuDecision> MainMenu::checkMenu()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int marginTopBottom = screenHeight / 20;
    int marginLeftRight = screenWidth / 20;
    int buttonGap = marginTopBottom / 2;

    // Start Game
    Vector2 startGameBegin = {marginLeftRight, marginTopBottom};
    Vector2 startGameEnd = {screenWidth - marginLeftRight, (screenHeight  / 2) - buttonGap};
    int startGameWidth = startGameEnd.x - startGameBegin.x;
    int startGameHeight = startGameEnd.y - startGameBegin.y;
    Rectangle startGameRec = {.x = startGameBegin.x, .y = startGameBegin.y, .width=startGameWidth, .height=startGameHeight};

    // Info
    Vector2 infoButtonBegin = {startGameBegin.x, startGameEnd.y + buttonGap};
    Vector2 infoButtonEnd = {screenWidth - marginLeftRight, screenHeight - marginTopBottom};
    Rectangle infoButtonRec = {.x = infoButtonBegin.x, .y = infoButtonBegin.y, .width=infoButtonEnd.x - infoButtonBegin.x, .height=infoButtonEnd.y - infoButtonBegin.y};

    const Vector2 mousePos = GetMousePosition();

    const bool overStart = CheckCollisionPointRec(mousePos, startGameRec);
    const bool overInfo = CheckCollisionPointRec(mousePos, infoButtonRec);

    DrawRectangleRec(startGameRec, BEIGE);
    DrawRectangleRec(infoButtonRec, GOLD);
    Color startGameTextColor = BEIGE;
    startGameTextColor.r = 255 - startGameTextColor.r;
    startGameTextColor.g = 255 - startGameTextColor.g;
    startGameTextColor.b = 255 - startGameTextColor.b;
    DrawText("Start Game", startGameBegin.x + marginLeftRight * 3, startGameBegin.y + marginTopBottom * 3, 100, startGameTextColor);
    Color infoButtonTextColor = GOLD;
    infoButtonTextColor.r = 255 - infoButtonTextColor.r;
    infoButtonTextColor.g = 255 - infoButtonTextColor.g;
    infoButtonTextColor.b = 255 - infoButtonTextColor.b;
    DrawText("Info", infoButtonBegin.x + marginLeftRight * 3, infoButtonBegin.y + marginTopBottom * 3, 100, infoButtonTextColor);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if(overStart) {
            return MainMenuDecision::StartGame;
        } else if(overInfo) {
            return MainMenuDecision::OpenInfo;
        }
    }

    return std::nullopt;
}