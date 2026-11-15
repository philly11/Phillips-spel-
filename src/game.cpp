#include "game.h"
#include <cmath>

Game::Game()
{
    InitWindow(1920, 1080, "phillips Game");
    DisableCursor();
    SetTargetFPS(60);

    camera.up = {0, 1, 0};
    camera.fovy = 75;
    camera.projection = CAMERA_PERSPECTIVE;
}

Game::~Game()
{
    CloseWindow();
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
}

void Game::Update()
{
    const PlayerInput input = ReadInput();

    const Vec3 prev = player.GetPosition(); // copy, so we can undo a move
    const float ground = map.GetGroundHeight(prev.x, prev.z, player.GetFeetY());
    player.Update(input, GetFrameTime(), ground);

    ApplyWallCollision(prev);
    UpdateCamera();
}

PlayerInput Game::ReadInput() const
{
    PlayerInput input;
    input.moveForward = (IsKeyDown(KEY_W) ? 1.0f : 0.0f) - (IsKeyDown(KEY_S) ? 1.0f : 0.0f);
    input.moveRight = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);
    input.jump = IsKeyPressed(KEY_SPACE);
    input.jumpHeld = IsKeyDown(KEY_SPACE);

    const Vector2 mouse = GetMouseDelta();
    input.lookYaw = -mouse.x;
    input.lookPitch = -mouse.y;
    return input;
}

// Undo movement per axis, so the player slides along walls instead of sticking
void Game::ApplyWallCollision(const Vec3 &prev)
{
    const Vec3 now = player.GetPosition();
    const float feetY = player.GetFeetY();
    float x = now.x;
    float z = now.z;

    // A move is blocked only if it pushes deeper into a wall than before
    if (map.WallOverlap(x, prev.z, feetY, kPlayerRadius) >
        map.WallOverlap(prev.x, prev.z, feetY, kPlayerRadius))
        x = prev.x;
    if (map.WallOverlap(x, z, feetY, kPlayerRadius) >
        map.WallOverlap(x, prev.z, feetY, kPlayerRadius))
        z = prev.z;

    player.setHorizontal(x, z);
}

void Game::UpdateCamera()
{
    const Vec3 &pos = player.GetPosition();
    const float yaw = player.GetYaw();
    const float pitch = player.GetPitch();

    const Vector3 look = {
        std::sin(yaw) * std::cos(pitch),
        -std::sin(pitch),
        std::cos(yaw) * std::cos(pitch)};

    camera.position = {pos.x, pos.y, pos.z};
    camera.target = {pos.x + look.x, pos.y + look.y, pos.z + look.z};
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(DARKBLUE);

    BeginMode3D(camera);
    DrawMap();
    DrawGrid(10, 1.0f);
    EndMode3D();

    DrawDebugText();
    EndDrawing();
}

void Game::DrawMap() const
{
    for (const Box &box : map.GetBoxes())
    {
        const Vector3 center = {box.x, box.y + box.height * 0.5f, box.z};
        DrawCube(center, box.width, box.height, box.depth, RED);
        DrawCubeWires(center, box.width, box.height, box.depth, MAROON);
    }
}

void Game::DrawDebugText() const
{
    DrawText(TextFormat("ground: %d  vy: %.2f  y: %.2f",
                        player.IsOnGround(), player.GetVelocityY(), player.GetPosition().y),
             10, 10, 20, WHITE);
    DrawText(TextFormat("x: %.2f  z: %.2f", player.GetPosition().x, player.GetPosition().z), 10, 40, 20, WHITE);
}