#pragma once
#include "raylib.h"
#include "player.h"
#include "map.h"
#include <vector>
#include "target.h"

class Game
{
public:
    Game();
    ~Game();
    void Run();

private:
    Player player;
    Camera3D camera = {};
    Map map;

    static constexpr float kPlayerRadius = 0.3f;

    void Update();
    void Draw();

    PlayerInput ReadInput() const;
    void ApplyWallCollision(const Vec3 &prev);
    void UpdateCamera();
    void DrawMap() const;
    void DrawDebugText() const;
    bool hasHit = false;
    Vector3 hitPoint = {};
    void Shoot();
    void DrawCrossHair() const;
    void DrawTargets() const;
    std::vector<Target> targets;
    static constexpr int kTargetMaxHealth = 5;
};