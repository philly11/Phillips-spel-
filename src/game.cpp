#include "game.h"
#include <cmath>
#include "map.h"

Game::Game()
{
    InitWindow(1920, 1080, "phillips Game");
    DisableCursor();
    SetTargetFPS(60);

    camera.up = {0, 1, 0};
    camera.fovy = 75;
    camera.projection = CAMERA_PERSPECTIVE;

    //                           x     y     z     w     h     d
    targets.emplace_back(Box{-6.0f, 0.0f, 7.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth);
    targets.emplace_back(Box{3.0f, 0.0f, -7.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth);
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
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        Shoot();
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
    const Vec3 look = player.GetLookDirection();

    camera.position = {pos.x, pos.y, pos.z};
    camera.target = {pos.x + look.x, pos.y + look.y, pos.z + look.z};
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(DARKBLUE);

    BeginMode3D(camera);
    DrawMap();
    DrawTargets();
    DrawGrid(20, 1.0f);
    if (hasHit)
        DrawSphere(hitPoint, 0.1f, YELLOW);
    EndMode3D();

    DrawCrossHair();
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

void Game::Shoot()
{
    const Vec3 origin = player.GetPosition();
    const Vec3 dir = player.GetLookDirection();
    const pgame::Ray ray{origin, dir};
    const float maxDistance = 100.0f;

    // 1. How far is the nearest wall?  (-1 if none)
    const float wallDist = map.RayCast(ray, maxDistance);

    // 2. Which target is nearest?  Track the index and distance.
    int nearestIndex = -1;
    float nearestDist = -1.0f;
    for (size_t i = 0; i < targets.size(); ++i)
    {
        if (!targets[i].IsAlive())
            continue; // dead targets don't block or take hits

        const float d = targets[i].RayCast(ray, maxDistance);
        if (d >= 0.0f && (nearestDist < 0.0f || d < nearestDist))
        {
            nearestDist = d;
            nearestIndex = static_cast<int>(i);
        }
    }

    // 3. The target only counts if no wall is in front of it
    const bool targetHit =
        nearestIndex >= 0 && (wallDist < 0.0f || nearestDist < wallDist);

    if (targetHit)
        targets[nearestIndex].TakeDamage(1);

    // 4. Marker at whichever thing was hit first
    float hitDist = -1.0f;
    if (targetHit)
        hitDist = nearestDist;
    else
        hitDist = wallDist;

    hasHit = hitDist >= 0.0f;
    if (hasHit)
        hitPoint = {origin.x + dir.x * hitDist,
                    origin.y + dir.y * hitDist,
                    origin.z + dir.z * hitDist};
}

void Game::DrawCrossHair() const
{
    const int cx = GetScreenWidth() / 2;
    const int cy = GetScreenHeight() / 2;
    DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
    DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
}

void Game::DrawTargets() const
{
    for (const Target &t : targets)
    {
        if (!t.IsAlive())
            continue;

        const Box &b = t.GetHitbox();
        const Vector3 center = {b.x, b.y + b.height * 0.5f, b.z};

        // Green when healthy, shifting to red as health drops
        const float ratio = static_cast<float>(t.GetHealth()) / kTargetMaxHealth;
        const Color color = {static_cast<unsigned char>(255 * (1.0f - ratio)),
                             static_cast<unsigned char>(255 * ratio), 0, 255};

        DrawCube(center, b.width, b.height, b.depth, color);
        DrawCubeWires(center, b.width, b.height, b.depth, BLACK);
    }
}