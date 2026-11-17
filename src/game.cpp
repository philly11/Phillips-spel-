#include "game.h"
#include <cmath>
#include "map.h"
#include "combat.h"

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
    targets.emplace_back(Box{8.0f, 0.0f, -8.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth, 2.5f);
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

    const float dt = GetFrameTime();
    for (Target &t : targets)
        t.Update(dt, player.GetPosition(), map);

    ApplyWallCollision(prev);
    UpdateCamera();

    weapon.Update(GetFrameTime());

    if (IsKeyPressed(KEY_R))
        weapon.StartReload();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && weapon.TryFire())
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
    const Vec3 r = map.ResolveMove(prev, player.GetPosition(), player.GetFeetY(), kPlayerRadius);
    player.setHorizontal(r.x, r.z);
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

    const char *text = weapon.IsReloading()
                           ? "Reloading..."
                           : TextFormat("%d / %d", weapon.GetAmmoInMagazine(), weapon.GetReserveAmmo());
    DrawText(text, GetScreenWidth() - 200, GetScreenHeight() - 50, 30, WHITE);
}

void Game::Shoot()
{
    const Vec3 origin = player.GetPosition();
    const Vec3 dir = player.GetLookDirection();
    const pgame::ShotResult shot =
        pgame::ResolveShot({origin, dir}, map, targets, 100.0f);

    if (shot.kind == pgame::ShotResult::Kind::Target)
        targets[shot.targetIndex].TakeDamage(weapon.GetDamage());

    hasHit = shot.kind != pgame::ShotResult::Kind::None;
    if (hasHit)
        hitPoint = {origin.x + dir.x * shot.distance,
                    origin.y + dir.y * shot.distance,
                    origin.z + dir.z * shot.distance};
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