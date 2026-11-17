#pragma once
#include <random>
#include <vector>

#include "map.h"
#include "player.h"
#include "raylib.h"
#include "target.h"
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
    void SpawnTargets();

    PlayerInput ReadInput() const;
    void ApplyWallCollision(const Vec3 &prev);
    void UpdateCamera();
    void Shoot();

    void Draw();
    void DrawMap() const;
    void DrawTargets() const;
    void DrawCrossHair() const;
    void DrawDebugText() const;
    void DrawDeathScreen() const;

    static constexpr float kPlayerRadius = 0.3f;
    static constexpr int kTargetMaxHealth = 5;

    GameState state = GameState::Playing;
    Player player;
    Camera3D camera = {};
    Map map;
    Weapon weapon;
    std::vector<Target> targets;
    bool hasHit = false;
    Vector3 hitPoint = {};
    std::mt19937 rng{std::random_device{}()}; // seeded once from the OS
};