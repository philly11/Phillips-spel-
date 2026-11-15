#pragma once
#include "raylib.h"
#include "player.h"
#include "map.h"

class Game
{
public:
    void Run();

private:
    Player player;
    Camera3D camera = {};
    Map map;

    void Update();
    void Draw();
};
