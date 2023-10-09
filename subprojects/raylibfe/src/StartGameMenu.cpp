#include "StartGameMenu.hpp"
#include "config.hpp"
#include <iostream>

StartGameMenu::StartGameMenu()
{
    random = LoadImage(ASSET_PATH "random.png");
    hotSeat = LoadImage(ASSET_PATH "hotseat.png");
    oracle = LoadImage(ASSET_PATH "oracle.png");

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Image randomW = ImageCopy(random);
    Image hotSeatW = ImageCopy(hotSeat);
    Image oracleW = ImageCopy(oracle);

    // essentially, every button is a "block" with blocks on either side of them, making
    // so, we have 3 * 2 - 1 (subtracting 1 for fencepost problem) blocks
    int buttonSize = screenWidth / 7;
    ImageResize(&randomW, buttonSize, buttonSize);
    ImageResize(&hotSeatW, buttonSize, buttonSize);
    ImageResize(&oracleW, buttonSize, buttonSize);

    randomText = LoadTextureFromImage(randomW);
    hotSeatText = LoadTextureFromImage(hotSeatW);
    oracleText = LoadTextureFromImage(oracleW);
}

std::optional<StartGameDecision> StartGameMenu::checkMenu()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonSize = screenWidth / 7;
    int xBegin = buttonSize;
    int yBegin = screenHeight / 2 - (buttonSize / 2);
    Rectangle hotSeatRec = {.x=xBegin, .y=yBegin, .width=buttonSize, .height=buttonSize};
    DrawTexture(hotSeatText, xBegin, yBegin, WHITE);
    xBegin += buttonSize * 2;
    Rectangle randomRec = {.x=xBegin, .y=yBegin, .width=buttonSize, .height=buttonSize};
    DrawTexture(randomText, xBegin, yBegin, WHITE);
    xBegin += buttonSize * 2;
    Rectangle oracleRec = {.x=xBegin, .y=yBegin, .width=buttonSize, .height=buttonSize};
    DrawTexture(oracleText, xBegin, yBegin, WHITE);

    const Vector2 mousePos = GetMousePosition();
    // if to practice reducing collision checks :)
    if(mousePos.y >= yBegin && mousePos.y <= yBegin + buttonSize) {
        bool overHotseat = CheckCollisionPointRec(mousePos, hotSeatRec);
        bool overRandom = CheckCollisionPointRec(mousePos, randomRec);
        bool overOracle = CheckCollisionPointRec(mousePos, oracleRec);
        const char *text = overHotseat ? "Hot Seat:\n\nPlay against another human on this machine" : (overRandom ? "Random:\n\nComputer makes random moves" : "The Oracle [of Delphi]:\n\nclassical AI that sees the future");
        DrawText(text, buttonSize / 2, yBegin + buttonSize + buttonSize / 2, 30, LIGHTGRAY);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(overHotseat) {
                return StartGameDecision::VersusHotseat;
            } else if(overRandom) {
                return StartGameDecision::VersusRandom;
            } else if(overOracle) {
                return StartGameDecision::VersusOracle;
            }
        }
    }

    return std::nullopt;
}