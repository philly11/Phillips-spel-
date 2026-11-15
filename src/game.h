#pragma once
#include "raylib.h"
#include "player.h"
#include "map.h"

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
};