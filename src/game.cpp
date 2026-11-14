#include "game.h"
#include "raylib.h"

void Game::Run()
{
    InitWindow(800, 600, "phillips Game");
    SetTargetFPS(60);

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
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw game elements here

    EndDrawing();
}