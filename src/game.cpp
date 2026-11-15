#include "game.h"
#include "raylib.h"

void Game::Run()
{
    InitWindow(800, 600, "phillips Game");
    SetTargetFPS(60);

    camera.position = {4, 4, 4};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }

    CloseWindow();
}

void Game::Update()
{
    // Update game logic here
    UpdateCamera(&camera, CAMERA_FREE);
    ShowCursor();
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(DARKBLUE);

    // Draw game elements here
    BeginMode3D(camera);

    DrawCube({0, 0, 0}, 2, 2, 2, RED);
    DrawGrid(10, 1.0f);
    EndMode3D();

    EndDrawing();
}