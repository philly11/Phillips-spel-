#include "game.h"
#include <cmath>

void Game::Run()
{
    InitWindow(1920, 1080, "phillips Game");
    DisableCursor(); // Hide the cursor for first-person camera control
    SetTargetFPS(60);

    camera.up = {0, 1, 0};
    camera.fovy = 75;
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

    PlayerInput input;
    input.moveForward = (IsKeyDown(KEY_W) ? 1.0f : 0.0f) - (IsKeyDown(KEY_S) ? 1.0f : 0.0f);
    input.moveRight = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);
    input.jump = IsKeyPressed(KEY_SPACE);
    input.jumpHeld = IsKeyDown(KEY_SPACE);

    Vector2 mouse = GetMouseDelta();
    input.lookYaw = -mouse.x;
    input.lookPitch = -mouse.y;

    const Vec3 &playerPos = player.GetPosition();
    float ground = map.GetGroundHeight(playerPos.x, playerPos.z);
    player.Update(input, GetFrameTime(), ground);

    float playerYaw = player.GetYaw();
    float playerPitch = player.GetPitch();

    Vector3 look = {
        std::sin(playerYaw) * std::cos(playerPitch),
        -std::sin(playerPitch),
        std::cos(playerYaw) * std::cos(playerPitch)};

    camera.position = {playerPos.x, playerPos.y, playerPos.z};
    camera.target = {playerPos.x + look.x, playerPos.y + look.y, playerPos.z + look.z};
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(DARKBLUE);

    // Draw game elements here
    BeginMode3D(camera);

    for (const Box &box : map.GetBoxes())
    {
        // game.cpp, in Draw:
        Vector3 center = {box.x, box.y + box.height * 0.5f, box.z};
        DrawCube(center, box.width, box.height, box.depth, RED);
        DrawCubeWires(center, box.width, box.height, box.depth, MAROON);
    }

    DrawGrid(10, 1.0f);
    EndMode3D();

    // const Vec3 &p = player.GetPosition();
    /*  DrawText(TextFormat("x %.1f  y %.1f  z %.1f  ground %.1f",
                         p.x, p.y, p.z, map.GetGroundHeight(p.x, p.z)),
              10, 10, 20, WHITE); */

    DrawText(TextFormat("space: %d  ground: %d  vy: %.2f  y: %.2f",
                        IsKeyDown(KEY_SPACE), player.IsOnGround(),
                        player.GetVelocityY(), player.GetPosition().y),
             10, 10, 20, WHITE);

    EndDrawing();
}