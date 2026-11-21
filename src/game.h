#pragma once
#include <optional>
#include <random>
#include <vector>

#include "ability.h"
#include "map.h"
#include "pickup.h"
#include "player.h"
#include "raylib.h"
#include "target.h"
#include "wave.h"
#include "weapon.h"

enum class GameState
{
    Playing,
    Dead,
};

class Game
{
public:
    Game();
    ~Game();
    void Run();

private:
    void Update();
    void UpdatePlaying(float dt);
    void Restart();
    void SpawnEnemy();
    std::optional<pgame::AbilityType> RollEnemyAbility(float chance);
    void SpawnPickups();

    PlayerInput ReadInput() const;
    void ApplyWallCollision(const Vec3 &prev);
    void ApplyAbilityEffects(const pgame::AbilityEffects &effects);
    void UpdatePickups(float dt);
    void UpdateCamo(const PlayerInput &input, bool hasCamo, float dt);
    void UpdateTargets(float dt);
    void RemoveDeadTargets();
    void UpdateWaves(float dt);
    void Blink();
    void UpdateCamera();
    void Shoot();

    void Draw();
    void DrawMap() const;
    void DrawTargets() const;
    void DrawPickups() const;
    void DrawHud() const;
    void DrawAbilities() const;
    void DrawPickupPopup() const;
    void DrawWaveInfo() const;
    void DrawCrossHair() const;
    void DrawDebugText() const;
    void DrawDeathScreen() const;

    static constexpr float kPlayerRadius = 0.3f;
    static constexpr float kMinSpawnDistance = 8.0f; // enemies never appear closer than this
    static constexpr float kBlinkDistance = 5.0f;
    static constexpr float kBlinkCooldown = 0.0f; // 0 = spammable, tune later
    static constexpr float kCamoDelay = 0.5f;     // seconds still before you're hidden
    static constexpr float kPopupDuration = 1.5f;

    GameState state = GameState::Playing;
    Player player;
    Camera3D camera = {};
    Map map;
    Weapon weapon;
    std::vector<Target> targets;
    std::vector<pgame::Pickup> pickups;
    pgame::WaveDirector waves;

    pgame::AbilitySlots abilities;
    float blinkCooldownTimer = 0.0f;
    float stillTimer = 0.0f;
    bool hidden = false;

    // Popup stores WHAT happened, not the text (see DrawPickupPopup)
    pgame::AbilityType popupType = pgame::AbilityType::Camo;
    pgame::AddResult popupResult = pgame::AddResult::Added;
    float popupTimer = 0.0f;

    bool hasHit = false;
    Vector3 hitPoint = {};
    std::mt19937 rng{std::random_device{}()}; // seeded once from the OS
};