#include "game.h"

#include <algorithm>
#include <cmath>

#include "combat.h"
#include "map.h"
#include "spread.h"

Game::Game()
{
    InitWindow(1920, 1080, "phillips Game");
    DisableCursor();
    SetTargetFPS(60);

    camera.up = {0, 1, 0};
    camera.fovy = 75;
    camera.projection = CAMERA_PERSPECTIVE;

    SpawnTargets();
    SpawnPickups();
}

void Game::SpawnTargets()
{
    targets.clear();
    //                           x     y     z     w     h     d
    targets.emplace_back(Box{-6.0f, 0.0f, 7.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth);
    targets.emplace_back(Box{3.0f, 0.0f, -7.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth);
    targets.emplace_back(Box{8.0f, 0.0f, -8.0f, 1.0f, 2.0f, 1.0f}, kTargetMaxHealth, 2.5f);
}

void Game::SpawnPickups()
{
    pickups.clear();
    pickups.emplace_back(Vec3{-6.0f, 0.0f, -5.0f});
    pickups.emplace_back(Vec3{7.0f, 0.0f, -4.0f});
    pickups.emplace_back(Vec3{-7.0f, 0.0f, 6.0f});
    pickups.emplace_back(Vec3{6.0f, 1.0f, 6.0f}); // on the platform: a reward for climbing
}

void Game::Restart()
{
    player = Player{};
    weapon = Weapon{};
    abilities = pgame::AbilitySlots{};
    hasHit = false;
    blinkCooldownTimer = 0.0f;
    stillTimer = 0.0f;
    hidden = false;
    popupTimer = 0.0f;
    SpawnTargets();
    SpawnPickups();
    state = GameState::Playing;
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
    const float dt = GetFrameTime();

    switch (state)
    {
    case GameState::Playing:
        UpdatePlaying(dt);
        break;
    case GameState::Dead:
        if (IsKeyPressed(KEY_ENTER))
            Restart();
        break;
    }
}

void Game::UpdatePlaying(float dt)
{
    // Abilities first, so this frame's effects apply to everything below
    abilities.Update(dt);
    const pgame::AbilityEffects effects = abilities.GetEffects();
    ApplyAbilityEffects(effects);

    PlayerInput input = ReadInput();
    if (effects.invertLook)
    {
        // Flipped here, so Player never needs to know abilities exist
        input.lookYaw = -input.lookYaw;
        input.lookPitch = -input.lookPitch;
    }

    const Vec3 prev = player.GetPosition(); // copy, so we can undo a move
    const float ground = map.GetGroundHeight(prev.x, prev.z, player.GetFeetY());
    player.Update(input, dt, ground);
    ApplyWallCollision(prev);

    blinkCooldownTimer = std::max(0.0f, blinkCooldownTimer - dt);
    if (effects.canBlink && IsKeyPressed(KEY_Q) && blinkCooldownTimer <= 0.0f)
    {
        Blink();
        blinkCooldownTimer = kBlinkCooldown;
    }

    UpdatePickups(dt);
    UpdateCamo(input, effects.camo, dt);
    UpdateTargets(dt);

    weapon.Update(dt);

    if (IsKeyPressed(KEY_R))
        weapon.StartReload();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && weapon.TryFire())
        Shoot();

    popupTimer = std::max(0.0f, popupTimer - dt);

    if (!player.IsAlive())
        state = GameState::Dead;

    UpdateCamera(); // last, so the camera shows this frame's final state (incl. recoil)
}

// Decide (AbilitySlots) vs apply (here): each value goes to the object that owns it
void Game::ApplyAbilityEffects(const pgame::AbilityEffects &effects)
{
    player.SetSpeedMultiplier(effects.speedMultiplier);
    player.SetEyeHeightMultiplier(effects.eyeHeightMultiplier);
    weapon.SetBloomMultiplier(effects.spreadMultiplier);
}

void Game::UpdatePickups(float dt)
{
    const Vec3 pos = player.GetPosition();
    const Vec3 feet = {pos.x, player.GetFeetY(), pos.z};
    std::uniform_int_distribution<int> roll(0, pgame::kAbilityTypeCount - 1);

    for (pgame::Pickup &pickup : pickups)
    {
        pickup.Update(dt);
        if (!pickup.TryCollect(feet))
            continue;

        const auto type = static_cast<pgame::AbilityType>(roll(rng));
        popupResult = abilities.Add(type);
        popupType = type;
        popupTimer = kPopupDuration;
    }
}

// Standing still for a moment hides you. Moving or jumping breaks it (shooting: see Shoot)
void Game::UpdateCamo(const PlayerInput &input, bool hasCamo, float dt)
{
    const bool still = input.moveForward == 0.0f && input.moveRight == 0.0f && player.IsOnGround();
    stillTimer = still ? stillTimer + dt : 0.0f;
    hidden = hasCamo && stillTimer >= kCamoDelay;
}

void Game::UpdateTargets(float dt)
{
    for (Target &target : targets)
    {
        // Hidden: enemies lose track of you. Their own position as goal = stand still.
        const Box &b = target.GetHitbox();
        const Vec3 goal = hidden ? Vec3{b.x, b.y, b.z} : player.GetPosition();
        target.Update(dt, goal, map);

        if (!hidden && player.IsAlive() && target.TryAttack(dt, player.GetPosition()))
            player.TakeDamage(Target::kAttackDamage);
    }
}

// Short horizontal teleport the way you face. Stops before walls.
void Game::Blink()
{
    const float yaw = player.GetYaw();
    const Vec3 dir = {std::sin(yaw), 0.0f, std::cos(yaw)}; // yaw only: never up over walls
    const Vec3 pos = player.GetPosition();                 // copy: we're about to move the player

    // Cast just above step height: things you can step onto shouldn't block the blink
    const Vec3 origin = {pos.x, player.GetFeetY() + Map::kStepHeight + 0.01f, pos.z};
    const float hit = map.RayCast({origin, dir}, kBlinkDistance);

    float distance = kBlinkDistance;
    if (hit >= 0.0f)
        distance = std::max(0.0f, hit - kPlayerRadius);

    player.setHorizontal(pos.x + dir.x * distance, pos.z + dir.z * distance);
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
    DrawPickups();
    DrawGrid(20, 1.0f);
    if (hasHit)
        DrawSphere(hitPoint, 0.1f, YELLOW);
    EndMode3D();

    DrawHud();
    DrawDebugText();

    if (state == GameState::Dead)
        DrawDeathScreen();

    EndDrawing();
}

void Game::DrawHud() const
{
    if (hidden)
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGREEN, 0.15f));

    DrawCrossHair();
    DrawText(TextFormat("HP %d", player.GetHealth()), 20, GetScreenHeight() - 80, 30, RED);

    const char *ammo = weapon.IsReloading() ? "Reloading..."
                                            : TextFormat("%d / %d", weapon.GetAmmoInMagazine(),
                                                         weapon.GetReserveAmmo());
    DrawText(ammo, GetScreenWidth() - 200, GetScreenHeight() - 50, 30, WHITE);

    DrawAbilities();
    DrawPickupPopup();
}

void Game::DrawAbilities() const
{
    constexpr int kBarWidth = 200;
    constexpr int kBarHeight = 8;
    const int x = 20;
    int y = GetScreenHeight() - 140; // stacked upward, above HP

    for (const auto &slot : abilities.GetSlots())
    {
        if (!slot)
            continue;

        const float ratio = slot->timeLeft / pgame::kAbilityDuration;
        DrawText(pgame::AbilityName(slot->type), x, y, 24, WHITE);
        DrawRectangle(x, y + 28, kBarWidth, kBarHeight, Fade(BLACK, 0.5f));
        DrawRectangle(x, y + 28, static_cast<int>(kBarWidth * ratio), kBarHeight, WHITE);
        y -= 50;
    }
}

void Game::DrawPickupPopup() const
{
    if (popupTimer <= 0.0f)
        return;

    // TextFormat returns a shared buffer that the next call overwrites,
    // so we store type + result and build the text here, at draw time.
    const char *name = pgame::AbilityName(popupType);
    const char *text =
        popupResult == pgame::AddResult::Refreshed ? TextFormat("%s REFRESHED", name) : name;

    constexpr int kFontSize = 60;
    const float alpha = popupTimer / kPopupDuration;
    const int x = (GetScreenWidth() - MeasureText(text, kFontSize)) / 2;
    DrawText(text, x, GetScreenHeight() / 3, kFontSize, Fade(WHITE, alpha));
}

void Game::DrawPickups() const
{
    // Bobbing is drawing only: the pickup's logic position never moves
    const float bob = 0.1f * std::sin(static_cast<float>(GetTime()) * 3.0f);

    for (const pgame::Pickup &p : pickups)
    {
        if (!p.IsAvailable())
            continue;

        const Vec3 &pos = p.GetPosition();
        const Vector3 center = {pos.x, pos.y + 0.5f + bob, pos.z};
        DrawCube(center, 0.4f, 0.4f, 0.4f, GOLD);
        DrawCubeWires(center, 0.4f, 0.4f, 0.4f, ORANGE);
    }
}

void Game::DrawDeathScreen() const
{
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    DrawRectangle(0, 0, w, h, Fade(BLACK, 0.6f));

    const char *title = "YOU DIED";
    const char *hint = "Press ENTER to restart";
    DrawText(title, (w - MeasureText(title, 80)) / 2, h / 2 - 60, 80, RED);
    DrawText(hint, (w - MeasureText(hint, 30)) / 2, h / 2 + 40, 30, WHITE);
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
    DrawText(TextFormat("ground: %d  vy: %.2f  y: %.2f", player.IsOnGround(), player.GetVelocityY(),
                        player.GetPosition().y),
             10, 10, 20, WHITE);
    DrawText(TextFormat("x: %.2f  z: %.2f", player.GetPosition().x, player.GetPosition().z), 10, 40,
             20, WHITE);
}

void Game::Shoot()
{
    // Shooting breaks camo
    stillTimer = 0.0f;
    hidden = false;

    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::uniform_real_distribution<float> side(-1.0f, 1.0f);

    const Vec3 origin = player.GetPosition();
    const Vec3 aim = player.GetLookDirection();
    const Vec3 dir = pgame::ApplySpread(aim, weapon.GetShotSpread(), unit(rng), unit(rng));

    const pgame::ShotResult shot = pgame::ResolveShot({origin, dir}, map, targets, 100.0f);

    if (shot.kind == pgame::ShotResult::Kind::Target)
        targets[shot.targetIndex].TakeDamage(weapon.GetDamage());

    hasHit = shot.kind != pgame::ShotResult::Kind::None;
    if (hasHit)
        hitPoint = {origin.x + dir.x * shot.distance, origin.y + dir.y * shot.distance,
                    origin.z + dir.z * shot.distance};

    // Kick after the bullet has left: this shot used the aim from before the kick
    player.AddRecoil(weapon.GetRecoilKick(), side(rng) * weapon.GetRecoilSideways());
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