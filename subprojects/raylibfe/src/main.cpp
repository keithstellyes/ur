#include <string>
#include <sstream>

#include "raylib.h"

#include "GameState.hpp"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Game of Ur");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 20.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    // TODO don't hardcord assets path, but use something like here https://stackoverflow.com/questions/46473646/meson-working-with-data-assets-and-portable-relative-paths
    const Model boardModel = LoadModel("/home/keith/projects/ur/subprojects/raylibfe/assets/ur-board.obj");
    const Texture2D boardTexture = LoadTexture("/home/keith/projects/ur/subprojects/raylibfe/assets/balsa.png");
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = boardTexture; 

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    ur::GameState gs;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        std::ostringstream oss;

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawModel(boardModel, {0.0f, 0.0f, 0.0f}, 1.0, WHITE);
            EndMode3D();
            oss << "Current player turn: " << (gs.currentPlayerTurn + 1);
            DrawText(oss.str().c_str(), 10, 200, 20, LIGHTGRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}